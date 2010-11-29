# -*- rd -*-

= Install to Ubuntu Linux --- How to install ChupaText to Ubuntu Linux

== Introduction

This document explains how to install ChupaText to Ubuntu
Linux.

== Install

We can use aptitude because ChupaText provides Debian packages.

There are packages for Ubuntu Linux Lucid
Lynx/Maverick Meerkat i386/amd64. If you need a package for another
environment, you can request on ((<mailing
list|URL:https://lists.sourceforge.net/lists/listinfo/chupatext-users-en>)).

Here are apt lines for for Lucid Lynx. We put them into
/etc/apt/sources.list.d/chupatext.list.

/etc/apt/sources.list.d/chupatext.list:
  deb http://groonga.rubyforge.org/ubuntu/ lucid main
  deb-src http://groonga.rubyforge.org/ubuntu/ lucid main

Here are apt lines for for Maverick Reerkat. We put them into
/etc/apt/sources.list.d/chupatext.list.

/etc/apt/sources.list.d/chupatext.list:
  deb http://groonga.rubyforge.org/ubuntu/ maverick main
  deb-src http://groonga.rubyforge.org/ubuntu/ maverick main

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
