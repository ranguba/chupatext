# Copyright (C) 2010  Nobuyoshi Nakada <nakada@clear-code.com>
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

require 'nokogiri'

class Chupa::HTML < Chupa::BaseDecomposer
  mime_types "text/html"

  def decompose
    data = @input.read
    doc = Nokogiri::HTML.parse(data, nil, guess_encoding(data))
    if title = (doc % "head/title")
      metadata.title = title.text
    end
    if encoding = doc.encoding
      metadata.original_encoding = encoding.downcase
    end
    metadata.encoding = "utf-8"
    if body = (doc % "body")
      body = body.text.gsub(/^\s+|\s+$/, '')
    else
      body = ""
    end
    metadata.content_length = body.bytesize
    text(body)
  end

  private
  def guess_encoding(data)
    case data
    when /\A<\?xml.+?encoding=(['"])([a-zA-Z0-9_-]+)\1/
      $2
    else
      nil
    end
  end
end
