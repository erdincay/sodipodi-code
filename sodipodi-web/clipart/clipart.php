<?php
global $PHP_SELF;
global $n;
$cols = 4;
$perpage = 24;

// Set page variables
if ($n == 0) $n = 1;
$area = $pagedata['area'];

$count = 0;
$dir = opendir("./clipart/$area/");
while ($file = readdir($dir)) {
	if (ereg(".txt$", $file)) {
		$filelist[] = $file;
	}
}
closedir($dir);

sort($filelist);
$total = count($filelist);

// Generate 'Previous' and 'Next' navigation links
$left = $n - $perpage;
if ($left > 0) $previous = "<A HREF='$PHP_SELF?section=$section/$area&area=$area&n=$left'><<< Previous</A>\n";

$right = $n + $perpage;
if ($right < $total) $next = "<A HREF='$PHP_SELF?section=$section/$area&area=$area&n=$right'>Next >>>></A>\n";

// Generate drop-down with page numbers
print "<CENTER><FORM ACTION='$PHP_SELF' METHOD='POST'>\n";
print "<TABLE WIDTH='90%'><TR><TD WIDTH='33%' ALIGN='left'>$previous</TD>\n";
print "<TD WIDTH='33%' ALIGN='center'>\n";
print "<INPUT TYPE='hidden' NAME='section' VALUE='$section/$area'>\n";
print "<INPUT TYPE='hidden' NAME='area' VALUE='$area'>\n";
print "<SELECT NAME='n'>\n";
for ($i=0; $i<($total / $perpage); $i++) {
	$def = '';
	$first = ($i * $perpage) + 1;
	$last = ($i + 1) * $perpage;
	if ($last > $total) $last = $total;

	if ($first == $n) $def = ' selected';

	print "<OPTION VALUE='$first'$def>$first - $last</OPTION>\n";
}
print "</SELECT><INPUT TYPE='SUBMIT' VALUE='Go'></TD>\n";
print "<TD WIDTH='33%' ALIGN='right'>$next</TD></TR></TABLE></FORM></CENTER>\n";

print "<BR><BR><TABLE WIDTH='100%' BORDER='0'>\n";

for ($i=($n-1); $i<($n + $perpage - 1); $i++) {
	$file = $filelist[$i];
	if ($i >= $total) break;

	if ($count % $cols == 0) print "<TR ALIGN='center' VALIGN='bottom'>\n";
	
	$eval = file("./clipart/$area/$file");
	eval(join('', $eval));

	$basefile = basename($file, '.txt');

	print "<TD>\n";
	print "<A HREF='clipart/$area/$basefile.svg' CLASS='clipart'>\n";
	print "<IMG SRC='clipart/$area/$basefile.png' CLASS='clipart'><BR>\n";
	print "<B>$title</B></A><BR>By: $author<BR>\n";
	print "<BR>&nbsp;</TD>\n";

	$count++;
	if ($count % $cols == 0) print "</TR>\n";
}

// Make sure table closed off properly
if ($count % $cols != 0) print "</TR>\n";
?>

</TABLE>

<BR><BR>
<CENTER>

<TABLE WIDTH='90%'><TR>
<TD WIDTH='33%' ALIGN='left'><?php echo $previous?></TD>
<TD WIDTH='33%' ALIGN='center'>Total: <B><?php echo $total?></B></TD>
<TD WIDTH='33%' ALIGN='right'><?php echo $next?></TD>
</TR></TABLE>

</CENTER>
