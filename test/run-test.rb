#!/usr/bin/env ruby

$KCODE = 'utf-8' unless defined?(Encoding)

require 'rbconfig'
require 'fileutils'
require 'pathname'
require "open-uri"

top_build_dir = ENV["TOP_BUILD_DIR"] || File.join(File.dirname(__FILE__), "..")
top_build_dir = Pathname(top_build_dir).expand_path
top_src_dir = ENV["TOP_SRC_DIR"] || File.join(File.dirname(__FILE__), "..")
top_src_dir = Pathname(top_src_dir).expand_path

build_dir = top_build_dir + "test"
src_dir = top_src_dir + "test"

test_lib_dir = build_dir + "lib"
FileUtils.mkdir_p(test_lib_dir.to_s)

rubyforge_download_base_url = "http://rubyforge.org/frs/download.php"
tgz_uris = ["#{rubyforge_download_base_url}/71835/test-unit-2.1.1.tgz",
            "#{rubyforge_download_base_url}/71705/test-unit-notify-0.0.1.tgz"]
tgz_uris.each do |tgz_uri|
  package = File.basename(tgz_uri, ".tgz")
  package_dir = test_lib_dir + package
  unless package_dir.exist?
    tgz = test_lib_dir + File.basename(tgz_uri)
    tgz.open("wb") do |output|
      output.print(open(tgz_uri).read)
    end
    system("tar", "xfz", tgz.to_s, "-C", tgz.dirname.to_s) or exit(false)
  end
  $LOAD_PATH.unshift((package_dir + "lib").to_s)
end

chuparuby_dir = test_lib_dir + "chuparuby"
chuparuby_ext_dir = chuparuby_dir + "ext" + "chupatext"
chuparuby_lib_dir = chuparuby_dir + "lib"
chuparuby_force_make = false
unless chuparuby_dir.exist?
  chuparuby_force_make = true
  Dir.chdir(test_lib_dir.to_s) do
    chuparuby_git_url = "git://github.com/ranguba/chuparuby.git"
    system("git", "clone", chuparuby_git_url) or exit(false)
  end
  Dir.chdir(chuparuby_ext_dir.to_s) do
    system(ENV["RUBY"], "extconf.rb") or exit(false)
  end
end
if chuparuby_force_make or ENV["NO_MAKE"] != "yes"
  Dir.chdir(chuparuby_dir.to_s) do
    system("git", "pull", "--rebase") or exit(false)
  end
  Dir.chdir(chuparuby_ext_dir.to_s) do
    make_args = ["CPPFLAGS=-I#{top_src_dir}",
                 "LIBPATH=-L#{top_build_dir}/chupatext/.libs"]
    system(ENV["MAKE"], *make_args) or exit(false)
  end
end
$LOAD_PATH.unshift(chuparuby_ext_dir.to_s)
$LOAD_PATH.unshift(chuparuby_lib_dir.to_s)

require 'test/unit'
require 'test/unit/version'
require 'test/unit/notify'

ruby_test_dir = src_dir + "ruby"
$LOAD_PATH.unshift(ruby_test_dir.to_s)

require 'chupa_test_utils'

ARGV.unshift("--exclude", "run-test.rb")
ARGV.unshift("--notify")
exit Test::Unit::AutoRunner.run(true, ruby_test_dir.to_s)
