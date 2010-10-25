# Copyright (C) 2010  Yuto HAYAMIZU <y.hayamizu@gmail.com>
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

module Chupa
  class PowerPoint < BaseDecomposer
    mime_types "application/vnd.ms-powerpoint"

    def decompose
      ooffice = OpenOffice.new(ENV['CHUPA_HOME'])
      ooffice.prepare()

      pdf = Tempfile.new(["chupadata-pdf", ".pdf"])
      FileUtils.rm(pdf.path)
      ppt = Tempfile.new(["chupadata-ppt", ".ppt"])
      ppt.write(@source.read)
      ppt.close

      ooffice.convert(ppt.path, pdf.path)
      data = Chupa::Data.decompose(pdf.path)
      accepted(data.read)
    end
  end

  class OpenOffice
    SCRIPT_DIR = ".openoffice.org/3/user/basic/Standard"
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

    def initialize(home_dir = nil)
      @home_dir = Pathname.new(File.expand_path(home_dir || "~/.chupa"))
      @script_dir = @home_dir + SCRIPT_DIR
    end

    def prepare
      unless File.directory?(@home_dir)
        FileUtils.mkdir_p(@home_dir)
      end
      unless File.directory?(@script_dir)
        unique_key = Digest::MD5.hexdigest(Time.now.to_s + Object.new.object_id.to_s)
        ooffice_pid = spawn({"HOME" => @home_dir.to_s},
                            "ooffice", "-headless", "macro:///Stop", unique_key)
        Process.waitpid(ooffice_pid)
        while (ps_str = `ps aux`).include?(unique_key)
          sleep(0.5)
        end
      end

      SCRIPTS.each do |script|
        File.open(@script_dir + script[:name], "w") do |file|
          file.puts(script[:content])
        end
      end
    end

    def convert(from_path, to_path)
      ooffice_pid = spawn({"HOME" => @home_dir.to_s},
                          "ooffice", "-headless", from_path,
                          "macro:///Standard.Export.WritePDF(\"file://#{to_path}\")")
      Process.waitpid(ooffice_pid)
      ooffice_start_time = Time.now
      while `ps aux`.include?(to_path)
        if Time.now - ooffice_start_time > 30
          system("killall soffice.bin")
          raise DecomposeError.new("Timeout: PowerPoint file conversion")
        end
        sleep(0.5)
      end

      to_path
    end

    def home_dir
      @home_dir.to_s
    end
  end
end
