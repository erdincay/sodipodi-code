<h2>Workarounds for known bugs</h2>

<p>Please <a href="http://sourceforge.net/tracker/?group_id=4054&atid=104054">report
new bugs</a> to the Sodipodi area of the SourceForge site, and check
there for bugs not already listed here.</p>

<h2>0.31 bugs and work arounds</h2>

<h3><i>colinear!</i></h3>

<p>This is a libart warning.  The original libart intersector has all
kinds of numerical problems, and it is believed that this warning is
emitted if it tries to find intersection of two almost parallel lines.</p>

<h3><i>warning: failed to load external entity "glade/icons.svg"</i></h3>

<p>Many people see this warning, however it does not appear to be a
fatal problem.  It's a normal message that should be suppressed.
Instead of glade/icons.svg, SVG icons probably will be loaded from
$PREFIX/share/sodipodi/icons.svg.</p>




