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

require 'English'

module Chupa
  class ExternalCommand
    def initialize(command)
      @command = Pathname.new(command)
    end

    def run(*args)
      if args.last.is_a?(Hash)
        option = args.pop
      else
        option = {}
      end
      pid = spawn((option[:env] || {}),
                  @command.to_s,
                  *args,
                  (option[:spawn_option] || {}).merge(default_spawn_options))
      pid, status = Process.waitpid2(pid)
      status.success?
    end

    def exist?
      if @command.absolute?
        @command.file? and @command.executable?
      else
        (ENV['PATH'] || "").split(File::PATH_SEPARATOR).any? do |path|
          (Pathname.new(path) + @command).expand_path.exist?
        end
      end
    end

    private
    def default_spawn_options
      SpawnLimitOptions.new.options
    end

    class SpawnLimitOptions
      attr_reader :options
      def initialize
        @options = {}
        set_default_options
      end

      private
      def set_default_options
        set_option(:cpu, :int)
        set_option(:rss, :size)
        set_option(:as, :size)
      end

      def set_option(key, type)
        value = ENV["CHUPA_EXTERNAL_COMMAND_LIMIT_#{key.to_s.upcase}"]
        return if value.nil?
        value = send("parse_#{type}", key, value)
        return if value.nil?
        rlimit_number = Process.const_get("RLIMIT_#{key.to_s.upcase}")
        soft_limit, hard_limit = Process.getrlimit(rlimit_number)
        if hard_limit < value
          log_hard_limit_over_value(key, value, hard_limit)
          return nil
        end
        limit_info = "soft-limit:#{soft_limit}, hard-limit:#{hard_limit}"
        log(:info, "[#{key}][set] <#{value}>(#{limit_info})")
        @options[:"rlimit_#{key}"] = value
      end

      def parse_int(key, value)
        begin
          Integer(value)
        rescue ArgumentError
          log_invalid_value(key, value, type, "int")
          nil
        end
      end

      def parse_size(key, value)
        return nil if value.nil?
        scale = 1
        case value
        when /GB?\z/i
          scale = 1024 ** 3
          number = $PREMATCH
        when /MB?\z/i
          scale = 1024 ** 2
          number = $PREMATCH
        when /KB?\z/i
          scale = 1024 ** 1
          number = $PREMATCH
        when /B?\z/i
          number = $PREMATCH
        else
          number = value
        end
        begin
          number = Integer(number)
        rescue ArgumentError
          log_invalid_value(key, value, "size")
          return nil
        end
        number * scale
      end

      def log_hard_limit_over_value(key, value, hard_limit)
        log(:warning, "[#{key}][large] <#{value}>(hard-limit:#{hard_limit})")
      end

      def log_invalid_value(key, value, type)
        log(:warning, "[#{key}][invalid] <#{value}>(#{type})")
      end

      def log(level, message)
        Logger.send(level,
                    "[decomposer][ruby][external-command][limit]#{message}")
      end
    end
  end
end
