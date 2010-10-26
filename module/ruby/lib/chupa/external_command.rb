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

module Chupa
  class ExternalCommand
    def initialize(command)
      @command = command
    end

    def run(*args)
      if args.last.is_a?(Hash)
        option = args.pop
      else
        option = {}
      end
      pid = spawn((option[:env] || {}), @command, *args, (option[:spawn_option] || {}))
      pid, status = Process.waitpid2(pid)
      status.success?
    end

    def exist?
      (ENV['PATH'] || "").split(File::PATH_SEPARATOR).any? do |path|
        (Pathname.new(path) + @command).expand_path.exist?
      end
    end
  end
end
