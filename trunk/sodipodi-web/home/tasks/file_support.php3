<h3>File Support</h3>

<h4>Extend file support to various compressed file formats</h4>

<p>Implement modules for loading files of different compressed formats,
such as BZ2, GZip, ZIP, OLE2, remote files, in-memory files, iochannels,
bonobo streams, etc.</p>

<p>libgsf might help here, although piping through gzip might be much
easier and eliminate the need for the extra dependency.  For
componentized storage we need composite file format decided first.
<i>Zip
might help solve some linking problems.  Maybe I'll investigate this
further.  -- lauris</i></p>

<h4>Import/export of EPS</h4>

<p>Create import/export of eps via gnome-print.  This has to be done by
Sodipodi itself, rather than pass the work off to a lib.</p>

<h4>Propose approach for a container format</h4>

<p>Things to look at:  libefs, libgsf, ZIP/Jar used by OpenOffice.  

<p>Why bother with a container format?  It's just useful to be able to
combine all the elements of a compound document in one file -- for
example, an SVG file that embeds a large number of JPEGs, and then be
able to work with that compound document directly.</p>

<p>It's not required for anything, but one-(conceptual-)document-one-file
is certainly handy.</p>

<p>It wouldn't necessarily have anything to do with multiple pages or
desktop publishing, per se.  The discussion of various office suites
came in just because they've had to solve the same problem.</p>

<PRE>

[ n.b. this assumes you have zip(1) and unzip(1) installed ... the JRE
does provide a jar(1) command, but for this purpose I think they're more
convenient anyway.  FWIW, jar(1) has an interface almost identical to
classic tar(1) ]

listing file(s) inside a jar archive (to stdout):

  unzip -Z -1 JAR

  e.g. unzip -Z -1 foo.jar

a more ls-like format:

  unzip -Z -l JAR

  e.g. unzip -Z -l foo.jar

catting file(s) inside a jar archive (to stdout):

  unzip -p JAR FILE(s)

  e.g.: unzip -p foo.jar small/orav.svg | less

removing file(s) from the jar archive:

  zip -d JAR FILE(s)

  e.g.: zip -d foo.jar small/orav.svg

writing a file into the jar archive (the weak part):

  zip -r JAR FILE(s)


  e.g.: zip -g -r foo.jar small/orav.svg

  The disadvantage here is that the directory/filename used in the zip
  file is the one given on the commandline.  You can specify - to read
  the file from stdin ... but then you're stuck with a file named '-'
  in the archive :/

  I don't think InfoZIP supports renaming files inside of ZIP archives.
</PRE>

<p>There are other tools that can be used as well that may not have these
deficiencies; zip(1) and unzip(1) are just the most common on Unix
systems (I think).</p>

<p>It would be especially nifty if this concept included provision for
autogeneration of a thumbnail for the document to serve as a "preview
icon" in file managers, document systems, etc.  There would need to be a
mechanism of identifying which element of the compound document is the
"main page", and if the main page is an SVG file, a routine to generate
it in PNG form.</p>

<p>The GnomeOffice/OpenOffice folks may already have some convention in
mind for that sort of thing.</p>



