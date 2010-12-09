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
  class Logger
    @@domain = "chupatext"
    class << self
      def domain
        @@domain
      end

      def domain=(domain)
        @@domain = domain
      end

      def error(message)
        default.log(:error, message, 1)
      end

      def critical(message)
        default.log(:critical, message, 1)
      end

      def message(message)
        default.log(:message, message, 1)
      end

      def warning(message)
        default.log(:warning, message, 1)
      end

      def debug(message)
        default.log(:debug, message, 1)
      end

      def info(message)
        default.log(:info, message, 1)
      end

      def statistics(message)
        default.log(:statistics, message, 1)
      end
    end

    def log(level, message, n_call_depth=nil)
      unless level.is_a?(Chupa::LogLevelFlags)
        level = Chupa::LogLevelFlags.const_get(level.to_s.upcase)
      end
      n_call_depth ||= 0
      file, line, info = caller[n_call_depth].split(/:(\d+):/, 3)
      ensure_message(message).each_line do |one_line_message|
        log_full(self.class.domain, level, file, line.to_i, info.to_s,
                 one_line_message.chomp)
      end
    end

    private
    def ensure_message(message)
      case message
      when nil
        ''
      when String
        message
      when Exception
        "#{message.class}: #{message.message}:\n#{message.backtrace.join("\n")}"
      else
        message.inspect
      end
    end
  end
end

require 'chupatext/base_decomposer'
require 'chupatext/loader'
require 'chupatext/data'
