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

class MetadataTest < Test::Unit::TestCase
  def setup
    @metadata = Chupa::Metadata.new
  end

  def test_accessor
    assert_nil(@metadata.mime_type)
    @metadata.mime_type = "text/plain"
    assert_equal("text/plain", @metadata.mime_type)
  end

  def test_accessor_time
    assert_nil(@metadata.creation_time)
    creation_time = Time.parse("2010-11-07T19:44:31+09:00")
    @metadata.creation_time = creation_time
    assert_equal(creation_time, @metadata.creation_time)
  end
end
