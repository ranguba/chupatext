# -*- rd -*-

= chupatext / ChupaText / ChupaText's manual

== NAME

chupatext - Text extraction command

== SYNOPSHIS

(({chupatext})) [((*option ...*))] ((*file ...*))

== DESCRIPTION

ChupaText is a text extraction tool and useful for full text
search system.

It reads a file and extracts text and metadata and outputs
them as MIME format. It means that you can use a mail parser
for parsing ChupaText output.

It supports various file formats. See the following list
(encrypted file isn't supported):

  * Adobe PDF
  * Microsoft Word
  * Microsoft Excel
  * Microsoft PowerPoint
  * HTML
  * text

It also supports the following archive and compression formats:

  * zip
  * tar
  * gz

== OPTIONS

: -h, --help
   chupatext shows help message and exits.

: -j, --json
   chupatext outputs text and metadata extraction result as
   JSON format.

   chupatext outputs a hash object for a file but bracket
   that surrounds all hashs isn't included.

: -i, --ignore-errors
   chupatext ignores all errors occurred in extraction.

: --prefix=PATH
   This specifies a prefix that is added before file name in output.

   This is useful for outputing file name specified by
   relative path as absolute path or URI.

: -v, --version
   chupatext shows its version number and exits.

== EXIT STATUS

The exit status is 0 for success in extraction, non-0
otherwise. If ((*--ignore-errors*)) is specified, the exit
status is 0 for failure in extraction.

== FILES

: /usr/local/etc/chupatext/descriptions/*.desc

   Those files describe text extraction module information.

: /usr/local/lib/chupatext/modules/decomposers/*.so

   Those files are text extraction modules.

: /usr/local/lib/chupatext/ruby/chupa/decomposers/*.rb

   Those files are tet extraction modules implemented by Ruby.

== EXAMPLE

In the following example, chupatext extracts text and
metadata from PDF file and outputs as MIME format. Output
uses UTF-8 encoding.

  % chupatext sample.pdf

== SEE ALSO

unzip(1), gunzip(1), tar(1), pdftotext(1), wvText(1),
ssconvert(1), ooffice(1)
