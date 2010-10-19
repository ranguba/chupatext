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

class Chupa::BaseDecomposer
  class << self
    def decomposers
      @@decomposers
    end

    @@decomposers = {}
    def mime_types(*types)
      types.each do |type|
        @@decomposers[type] = self
      end
    end
  end

  def feed
    begin
      success = decompose
      @feeder.feed(@target) if success
      success
    rescue Exception
      puts "#{$!.class}:#{$!.message}"
      puts $@
      raise
      false
    end
  end

  private
  def accepted(data)
    @sink.write(data)
  end

  def metadata
    @target.metadata
  end
end
