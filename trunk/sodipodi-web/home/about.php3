<?
if (file_exists ("$directory"."latest.html")) {
    include ("$directory"."latest.html");
}
?>

<h3>What is Sodipodi?</h3>
<p>
Sodipodi is a vector-based drawing program, like CorelDraw&reg;
or Adobe Illustrator&reg; from the proprietary software world, and
<a href="http://sketch.sourceforge.net/">Sketch</a> or
<a href="http://www.koffice.org/karbon/">Karbon14</a>
from the free software world. It is free software, distributed under the
terms of the Gnu General Public License, Version 2.
</p>
<p>
Sodipodi uses
<a href="http://www.w3c.org/">W3C</a>
<a href="http://www.w3c.org/TR/SVG/">SVG</a>
as its native file format.  It is therefore a very
useful tool for web designers.
</p>
<p>
It has a relatively modern display engine, giving you finely antialiased
display, alpha transparencies, vector fonts and so on. Sodipodi is
written in C, using the Gtk+ toolkit and optionally some Gnome or
KDE libraries.
</p>
<p>
Sodipodi works under most versions of Unix and Windows.
</p>

<?
$directory="screenshots/new/";
include "${directory}latest.php3";
?>
