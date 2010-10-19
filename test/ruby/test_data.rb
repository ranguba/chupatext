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

class DataTest < Test::Unit::TestCase
  def setup
    text_file = Tempfile.new(["data-test", ".txt"])
    text_file.puts("Hello!")
    @data = Chupa::Data.decompose(text_file.path)
  end

  def test_text?
    assert_true(@data.text?)
    @data.metadata["mime-type"] = "text/html"
    assert_false(@data.text?)
  end
end
