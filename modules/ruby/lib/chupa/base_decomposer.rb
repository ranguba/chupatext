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

module Chupa
  class DecomposeError < StandardError
  end

  class BaseDecomposer
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

    attr_reader :input
    def initialize(feeder, input)
      @feeder = feeder
      @input = input
      @output_stream = nil
      @output = nil
      @metadata = nil
      @accepted = false
      @finisehd = false
    end

    def feed
      decompose
      finished
    end

    private
    def text(data)
      output_stream.add_data(data)
      unless @accepted
        metadata["mime-type"] = "text/plain"
        @feeder.accepted(output)
        @accepted = true
      end
    end

    def need_finished?
      @accepted and !@finished
    end

    def finished
      return unless need_finished?
      output.finished
      @finished = true
    end

    def delegate(delegate_data)
      @feeder.feed(delegate_data)
    end

    def output_stream
      @output_stream ||= GLib::MemoryInputStream.new
    end

    def output
      @output ||= Chupa::Data.new(output_stream, metadata)
    end

    def metadata
      @metadata ||= create_metadata
    end

    def create_metadata
      metadata = Chupa::Metadata.new
      metadata.merge_original_metadata(@input.metadata)
      metadata
    end
  end
end
