<?

if ($argv[0] == "keybindings") {
	print "<pre>\n";
	include ("${directory}keybindings.txt");
	print "</pre>\n";
} else {
	print "<h3>Random documents</h3>\n";
	print "<ul>\n";
	print "<li><a href=\"index.php3?section=documentation/usage/keybindings\">Keyboard shortcuts</a>\n";
	print "</ul>\n";
}

?>
