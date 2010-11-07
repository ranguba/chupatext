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

== OUTPUT FORMAT

chupatext command outputs extracted text data and metadata
with MIME format. Header fields include metadata and body
includes text data. Here are always included header fields:

: Content-Type
   It is always "text/plain; charset=UTF-8".

: Content-Length
   It is number of bytes of text data. chupatext outputs a
   newline at the last. So chupatext actually outputs number
   of bytes of text data + 1 bytes (for a newline) as a body.

   For example, chupatext outputs 7 bytes data for 6 bytes
   text "Sample". It's "Sample" (6 bytes) + "newline (\n)"
   (1 byte).

: Original-Filename
   It is the input filename.

: Original-Content-Type
   It is the MIME type of the input file. It may includes the
   following optional parameters:
     * charset: The encoding of the input file.
     * name: The input filename. (The same value of the Original-Filename)

: Original-Content-Disposition
   It is presentation information for input file. The type
   is always "inline".
     * filename: The input filename. (The same value of the Original-Filename)
     * creation-date: The creation time of the input file.
       (The same value of the Creation-Time)
     * modification-date: The last modified time of the
       input file. (The same value of the Modification-Time)
     * size: The number of bytes of the input file.
     * read-date: The time when the input file is read.

Here are optional fileds:

: Title
   The title of the input file.

: Author
   The author of the input file.

: Modification-Time
   The last modified time of the input file.

: Creation-Time
   The creation time of the input file.

== OPTIONS

: -h, --help
   chupatext shows help message and exits.

# : -j, --json
#    chupatext outputs text and metadata extraction result as
#    JSON format.

#    chupatext outputs a hash object for a file but bracket
#    that surrounds all hashs isn't included.

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
ssconvert(1), ooffice(1), unoconv(1)
