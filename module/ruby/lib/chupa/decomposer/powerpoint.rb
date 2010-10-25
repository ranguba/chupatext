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

class Chupa::PowerPoint < Chupa::BaseDecomposer
  mime_types "application/vnd.ms-powerpoint"

  def decompose
    pdf = Tempfile.new(["chupadata-pdf", ".pdf"])
    FileUtils.rm(pdf.path)
    ppt = Tempfile.new(["chupadata-ppt", ".ppt"])
    ppt.write(@source.read)
    ppt.close

    ooffice_pid = spawn("ooffice", "-headless", ppt.path,
                        "macro:///Standard.Export.WritePDF(\"file://#{pdf.path}\")")
    Process.waitpid(ooffice_pid)
    while `ps aux`.include?(pdf.path)
      sleep(0.5)
    end
    data = Chupa::Data.decompose(pdf.path)
    accepted(data.read)
  end
end
