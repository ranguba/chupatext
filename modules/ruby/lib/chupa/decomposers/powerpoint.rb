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
require 'chupa/external_command'

module Chupa
  class PowerPoint < BaseDecomposer
    class UnoConv < ExternalCommand
      def initialize
        super("unoconv")
      end

      def prepare
      end

      def convert(input, output)
        run("--stdout", input.path,
            {:spawn_option => {:out => output.fileno}})
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
          run(unique_key)
          while (ps_str = `ps aux`).include?(unique_key)
            sleep(0.5)
          end
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
        run(input.path,
            "macro:///Standard.Export.WritePDF(\"file://#{output.path}\")")
        ooffice_start_time = Time.now
        while `ps aux`.include?(output.path)
          if Time.now - ooffice_start_time > 30
            system("killall soffice.bin")
            raise DecomposeError.new("Timeout: PowerPoint file conversion")
          end
          sleep(0.5)
        end
      end

      def home_dir
        @home_dir.to_s
      end

      private
      def run(*arguments)
        super("-headless", *arguments, {:env => {"HOME" => @home_dir.to_s}})
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

      def unoconv
        UnoConv.new
      end

      def convertor
        [open_office("ooffice"),
         open_office("soffice"),
         open_office("/opt/libreoffice.org3/program/soffice"),
         open_office("/opt/openoffice.org3/program/soffice"),
         unoconv].find do |command|
          command.exist?
        end
      end
    end

    mime_types "application/vnd.ms-powerpoint" unless convertor.nil?

    def decompose
      convertor = self.class.convertor
      convertor.prepare

      pdf = Tempfile.new(["chupadata-pdf", ".pdf"])
      ppt = Tempfile.new(["chupadata-ppt", ".ppt"])
      ppt.write(@input.read)
      ppt.close

      convertor.convert(ppt, pdf)
      data = Chupa::Data.new(pdf.path, metadata)
      delegate(data.decompose(@feeder))
    end
  end
end
