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

require 'nkf'
require 'nokogiri'

class Chupa::HTMLDecomposer < Chupa::BaseDecomposer
  mime_types "text/html"

  def decompose
    data = @input.read
    doc = Nokogiri::HTML.parse(data, nil, guess_encoding(data))
    if title = (doc % "head/title")
      metadata.title = title.text
    end
    encoding = doc.encoding
    metadata.original_encoding = encoding.downcase if encoding
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
    when /<meta\s.*?
           http-equiv=(['"])content-type\1\s+
           content=(['"])(.+?)\2/imx # "
      content_type = $3
      mime_type, parameters = content_type.split(/;\s*/, 2)
      encoding = nil
      if parameters and /\bcharset=([a-zA-Z0-9_-]+)/i =~ parameters
        encoding = normalize_charset($1)
      end
      encoding
    else
      guess_encoding_nkf(data)
    end
  end

  def normalize_charset(charset)
    case charset
    when /\Ax-sjis\z/i
      "Shift_JIS"
    else
      charset
    end
  end

  def guess_encoding_nkf(data)
    case NKF.guess(data)
    when NKF::EUC
      "EUC-JP"
    when NKF::JIS
      "ISO-2022-JP"
    when NKF::SJIS
      "Shift_JIS"
    when NKF::UTF8
      "UTF-8"
    when NKF::UTF16
      "UTF-16"
    when NKF::UTF32
      "UTF-32"
    else
      nil
    end
  end
end
