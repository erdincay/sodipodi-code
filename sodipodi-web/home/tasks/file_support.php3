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

