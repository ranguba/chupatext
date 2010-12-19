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

require 'English'
require 'pathname'

module Chupa
  class Loader
    def initialize
      @decomposers = []
    end

    def load
      $LOAD_PATH.each do |path|
        path = Pathname(path)
        chupa_decomposer_directory = path + "chupatext" + "decomposers"
        next unless chupa_decomposer_directory.exist?
        Pathname.glob((chupa_decomposer_directory + "*.rb").to_s).each do |file|
          decomposer_file = file.relative_path_from(path).to_s.gsub(/\.rb\Z/, '')
          begin
            require(decomposer_file)
          rescue LoadError
            Logger.error($!)
          end
        end
      end
    end

    def decomposer(mime_type)
      BaseDecomposer.decomposers[mime_type]
    end

    def mime_types
      BaseDecomposer.decomposers.keys
    end
  end
end
