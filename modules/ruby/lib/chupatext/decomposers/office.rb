# Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
# Copyright (C) 2010  Kouhei Sutou <kou@clear-code.com>
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA  02110-1301  USA

require 'tempfile'
require 'digest/md5'
require 'pathname'
require 'chupatext/external_command'

module Chupa
  class OfficeDecomposer < BaseDecomposer
    class UnoConv < ExternalCommand
      def initialize
        super("unoconv")
      end

      def prepare
      end

      def convert(input, output)
        run("--stdout", input.path,
            {:spawn_options => {:out => output.fileno}})
      end
    end

    class LibreOffice < ExternalCommand
      def initialize(options={})
        super(options[:command] || "libreoffice")
        home_dir = options[:home_dir] || "~/.chupatext"
        @home_dir = Pathname.new(home_dir).expand_path
      end

      def prepare
      end

      def convert(input, output)
        output_directory = File.dirname(output.path)
        pipe_read, pipe_write = IO.pipe
        run("-headless",
            "-nologo",
            "-convert-to", "pdf",
            "-outdir", output_directory,
            input.path,
            {
              :env => {"HOME" => @home_dir.to_s},
              :spawn_options => {
                :out => pipe_write,
                :err => [:child, :out],
              },
            })
        pipe_write.close
        libre_office_start_time = Time.now
        pdf_path = input.path.gsub(/\.[a-z]+\z/i, ".pdf")
        unless File.exist?(pdf_path)
          output = pipe_read.read
          tag = "[libreoffice][convert][exited][abnormally]"
          raise BaseDecomposer::DecomposeError.new("#{tag}: <#{output}>")
        end
        FileUtils.mv(pdf_path, output.path)
      end
    end

    class OpenOffice < ExternalCommand
      SCRIPTS = [{
                   :name => "script.xlb",
                   :content => <<EOS
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE library:library PUBLIC "-//OpenOffice.org//DTD OfficeDocument 1.0//EN" "library.dtd">
<library:library xmlns:library="http://openoffice.org/2000/library" library:name="Standard" library:readonly="false" library:passwordprotected="false">
 <library:element library:name="Export"/>
</library:library>
EOS
                 },
                 {
                 :name => "Export.xba",
                 :content => <<EOS
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE script:module PUBLIC "-//OpenOffice.org//DTD OfficeDocument 1.0//EN" "module.dtd">
<script:module xmlns:script="http://openoffice.org/2000/script" script:name="Export" script:language="StarBasic">
sub WritePDF(url as string)
  dim document   as object
  dim dispatcher as object
  document   = ThisComponent.CurrentController.Frame
  dispatcher = createUnoService(&quot;com.sun.star.frame.DispatchHelper&quot;)

  dim args1(1) as new com.sun.star.beans.PropertyValue
  args1(0).Name = &quot;URL&quot;
  args1(0).Value = url
  args1(1).Name = &quot;FilterName&quot;
  args1(1).Value = &quot;writer_pdf_Export&quot;

  dispatcher.executeDispatch(document, &quot;.uno:ExportDirectToPDF&quot;, &quot;&quot;, 0, args1())

  document.close(true)
end sub

sub Terminate()
  StarDesktop.Terminate()
end sub
</script:module>
EOS
                 }]

      def initialize(options={})
        super(options[:command] || "ooffice")
        home_dir = options[:home_dir] || "~/.chupatext"
        @home_dir = Pathname.new(home_dir).expand_path
      end

      def prepare
        unless File.directory?(@home_dir)
          FileUtils.mkdir_p(@home_dir)
        end
        base_directory = guess_base_directory
        if base_directory.nil?
          random_id = "#{Time.now}#{Object.new.object_id}"
          unique_key = Digest::MD5.hexdigest(random_id)
          run(unique_key, run_options)
          base_directory = guess_base_directory
        end
        script_directory = @home_dir + base_directory + "3/user/basic/Standard"
        SCRIPTS.each do |script|
          File.open(script_directory + script[:name], "w") do |file|
            file.puts(script[:content])
          end
        end
      end

      def convert(input, output)
        FileUtils.rm(output.path)
        pipe_read, pipe_write = IO.pipe
        additional_run_options = {
          :spawn_options => {:out => pipe_write, :err => [:child, :out]},
        }
        run(input.path,
            "macro:///Standard.Export.WritePDF(\"file://#{output.path}\")",
            "macro:///Standard.Export.Terminate()",
            run_options.merge(additional_run_options))
        pipe_write.close
      end

      def home_dir
        @home_dir.to_s
      end

      private
      def run(*arguments)
        super("-headless", "-nologo", *arguments)
      end

      def run_options
        {:env => {"HOME" => @home_dir.to_s}}
      end

      def guess_base_directory
        [".libreoffice", ".openoffice.org"].find do |base_directory|
          script_dir = @home_dir + base_directory
          script_dir.exist?
        end
      end
    end

    class << self
      def open_office(command=nil)
        OpenOffice.new(:command => command,
                       :home_dir => ENV['CHUPA_HOME'])
      end

      def libre_office(command=nil)
        LibreOffice.new(:command => command,
                        :home_dir => ENV['CHUPA_HOME'])
      end

      def unoconv
        UnoConv.new
      end

      def convertor
        converters = libre_office_converters + open_office_converters
        converters << unoconv
        converters.find do |converter|
          converter.exist?
        end
      end

      private
      def libre_office_converters
        libre_office_paths = [ENV["LIBREOFFICE"],
                              "libreoffice",
                              "/opt/libreoffice.org3/program/soffice"]
        libre_office_paths.compact.collect do |path|
          libre_office(path)
        end
      end

      def open_office_converters
        open_office_paths = [ENV["OOFFICE"],
                             "ooffice",
                             "soffice",
                             "/opt/openoffice.org3/program/soffice"]
        open_office_paths.compact.collect do |path|
          open_office(path)
        end
      end
    end

    unless convertor.nil?
      mime_types "application/msword"
      mime_types "application/vnd.openxmlformats-officedocument.wordprocessingml.document"

      mime_types "application/vnd.oasis.opendocument.text"
      mime_types "application/vnd.oasis.opendocument.text-flat-xml"

      mime_types "application/vnd.ms-excel"
      mime_types "application/vnd.openxmlformats-officedocument.spreadsheetml.sheet"

      mime_types "application/vnd.oasis.opendocument.spreadsheet"
      mime_types "application/vnd.oasis.opendocument.spreadsheet-flat-xml"

      mime_types "application/vnd.ms-powerpoint"
      mime_types "application/vnd.openxmlformats-officedocument.presentationml.presentation"

      mime_types "application/vnd.oasis.opendocument.presentation"
      mime_types "application/vnd.oasis.opendocument.presentation-flat-xml"
    end

    def decompose
      convertor = self.class.convertor
      convertor.prepare

      extension = Pathname(@input.metadata.filename).extname
      pdf = Tempfile.new(["chupadata-office", ".pdf"],
                         :encoding => "ASCII-8BIT")
      powerpoint = Tempfile.new(["chupadata-office", extension],
                                :encoding => "ASCII-8BIT")
      powerpoint.write(@input.read)
      powerpoint.close

      convertor.convert(powerpoint, pdf)
      metadata.meta_ignore_time = true
      data = Chupa::Data.new(pdf.path, metadata)
      delegate(data)
    end
  end
end
