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

require 'rmail'
require 'nkf'

class Chupa::Mail < Chupa::BaseDecomposer
  mime_types "message/rfc822"

  def decompose
    data = @input.read
    message = RMail::Parser.read(data)

    metadata.title = NKF.nkf("-w80", message.header.subject)
    from = message.header.from.collect(&:format).join(", ")
    metadata.author = NKF.nkf("-w80", from)
    if message.body.is_a? Array
      # FIXME: do not handle multipart for now
      body = message.body[0]
    else
      body = message.body
    end
    body = NKF.nkf("-w80", body)
    metadata.content_length = body.bytesize
    text(body)
  end
end
