<h2>Workarounds for known bugs</h2>

<p>Please <a href="http://sourceforge.net/tracker/?group_id=4054&atid=104054">report
new bugs</a> to the Sodipodi area of the SourceForge site, and check
there for bugs not already listed here. More information about
how to submit report can be found at
<a href="index.php3?section=home/bugs">bug reporting section</a>.
</p>

<h3><i>Windows version fails to load</i></h3>
<p>
If you get error message, that read something like:
<i>
The procedure entry point libiconv_set_relocation_prefix could not be
located in the dynamic link library iconv.dll
</i>
you may try copying iconv.dll to sodipodi program location
(usually 'Program Files\Sodipodi'). If there are more errors,
copy all <b>DLL files</b> from Gtk+ lib subdirectory to sodipodi
program location, AND also <b>bin</b>, <b>lib</b> and <b>share</b>
subdirectories from
Gtk+ installation.
</p>

<h3><i>colinear!</i></h3>

<p>This is a libart warning.  The original libart intersector has all
kinds of numerical problems, and it is believed that this warning is
emitted if it tries to find intersection of two almost parallel lines.</p>

<h3><i>warning: failed to load external entity "glade/icons.svg"</i></h3>

<p>Many people see this warning, however it does not appear to be a
fatal problem.  It's a normal message that should be suppressed.
Instead of glade/icons.svg, SVG icons probably will be loaded from
$PREFIX/share/sodipodi/icons.svg.</p>




