<?

if ($argv[0] == "lauris") {
	include ("${directory}lauris.php3");
} else {
print "
<p>
The main author of sodipodi is
<a href=\"index.php3?section=about/authors/lauris\">Lauris Kaplinski</a>.
But like all free software projects, there are many people, who
have worked on one or another feature of our beloved program. And
it contains code, borrowed from earlier projects too. So it will
be impossible, to list everybody, whose work has gone into this
program, but here are some most notable ones:
\n";
	print "<dl>\n";
	print "<dt><a href=\"http://lauris.kaplinski.com/\">Lauris Kaplinski</a>\n";
	print "<dd>Main author of Sodipodi\n";
	print "<dt>Frank Felfe</li>\n";
	print "<dd>Author of toolbox, many dialogs, icons, transformation and snapping code\n";
	print "<dt><a href=\"http://www.globe.to/~oka326/\">Mitsuru Oka</a></li>\n";
	print "<dd>Dynamic draw, star, spiral and arc tools, per-object node editing\n";
	print "<dt>Masatake Yamato</li>\n";
	print "<dd>Frontline/Autotrace support, drag and drop\n";
	print "</dl>\n";
}

?>
