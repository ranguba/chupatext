# -*- rd -*-

= Install to Debian GNU/Linux --- How to install ChupaText to Debian GNU/Linux

== Introduction

This document explains how to install ChupaText to Debian
GNU/Linux.

== Install

We can use aptitude because ChupaText provides Debian packages.

There are packages for Debian GNU/Linux squeeze/sid
i386/amd64. If you need a package for another environment,
you can request on ((<mailing
list|URL:http://lists.sourceforge.jp/mailman/listinfo/groonga-dev>)).

Here are apt lines for for squeeze. We put them into
/etc/apt/sources.list.d/chupatext.list.

/etc/apt/sources.list.d/chupatext.list:
  deb http://groonga.rubyforge.org/debian/ squeeze main
  deb-src http://groonga.rubyforge.org/debian/ squeeze main

Here are apt lines for for sid. We put them into
/etc/apt/sources.list.d/chupatext.list.

/etc/apt/sources.list.d/chupatext.list:
  deb http://groonga.rubyforge.org/debian/ unstable main
  deb-src http://groonga.rubyforge.org/debian/ unstable main

ChupaText packages are signed by key of
kou@cozmixng.org/kou@clear-code.com. If we trust the key, we
can register the key:

  % gpg --keyserver hkp://subkeys.pgp.net --recv-keys 1C837F31
  % gpg --export 1C837F31 | sudo apt-key add -

If we register the key, we can install ChupaText by aptitude:

  % sudo aptitude update
  % sudo aptitude -V -D install chupatext

== The next step

Installation is completed.

We should confirm chupatext command usage in
((<chupatext.rd>)). We will see also reference manual (TODO)
to use ChupaText as a library.
