<?

function get_gallery ($dirname)
{
	$dir = opendir ($dirname);
	while ($entry = readdir ($dir)) {
		if (($entry != ".") && ($entry != "..") && (strstr ($entry, ".svg") == ".svg")) {
			$images[] = substr ($entry, 0, -4);
		}
	}
	if ($images) {
		sort ($images);
	}
	return $images;
}

function show_clipart_cell ($dirname, $image)
{
        if ("${dirname}" == "gallery/flags/") {
                print "<td align=\"right\" valign=\"bottom\" bgcolor=\"black\">\n";
        } else {
        	print "<td align=\"right\" valign=\"bottom\">\n";
        }
	print "<a href=\"showsvg.php3?file=${dirname}${image}.svg\">";
	if (file_exists ("${dirname}${image}.png")) {
		print "<img src=\"${dirname}${image}.png\">";
	} else {
		print "${image}.png";
	}
	print "</a>";
	print "</td>\n";
}

function show_clipart_row ($dirname, $image)
{
	show_clipart_cell ($dirname, $image);

	print "<td align=\"left\" valign=\"bottom\">\n";
	if (file_exists ("${dirname}${image}.html")) {
		include ("${dirname}${image}.html");
	} else 	if (file_exists ("${dirname}default.html")) {
		include ("${dirname}default.html");
	}
	print "</td>\n";
}

function show_svg ($filename)
{
	print ("Showing SVG $filename");
}

if ($pagedata['datadir']) {
	if ($directory) {
		$directory = $directory.$pagedata['datadir']."/";
	} else {
		$directory = $pagedata['datadir']."/";
	}
}


if (0 && $argv[0] && file_exists ("$directory".$argv[0].".svg")) {
	show_svg ("$directory".$argv[0].".svg");
} else {
	$images = get_gallery ($directory);
	$len = sizeof ($images);
	$start = (int) $argv[0];
	/* if ($start > ($len - 16)) {$start = $len - 16;} */
	if ($start < 0) {$start = 0;}
	$end = $start + 16;
	if ($end > $len) {$end = $len;}

	if ($len > 16) {
		print "<p>\n";
		for ($i = 0; $i < $len; $i += 16) {
			$first = $i + 1;
			$last = $first + 15;
			if ($last > $len) {$last = $len;}
			if ($i != $start) {
				print "<a href=\"index.php3?section=gallery/$page/$i\">$first - $last</a>\n";
			} else {
				print "$first - $last\n";
			}
		}
		print "<p>\n";
	}

	if (file_exists ("${directory}description.html")) {
		include ("${directory}description.html");
	}

        if ("${directory}" == "gallery/flags") {
        	print "<table cellspacing=\"16\" cellpadding=\"1\" border=\"0\">\n";
        } else {
               	print "<table cellspacing=\"16\" cellpadding=\"0\" border=\"0\">\n";
        }
	if ($pagedata['mode'] == "matrix") {
		for ($i = $start; $i < $end; $i += 4) {
			print "<tr>\n";
			for ($j = 0; $j < 4; $j++) {
				if (($i + $j) < $len) {
					show_clipart_cell ($directory, $images[$i + $j]);
				}
			}
			print "</tr>\n";
		}
	} else {
		for ($i = $start; $i < $end; $i++) {
			print "<tr>\n";
			show_clipart_row ($directory, $images[$i]);
			print "</tr>\n";
		}
	}
	print "</table>\n";

	if ($len > 16) {
		print "<p>\n";
		for ($i = 0; $i < $len; $i += 16) {
			$first = $i + 1;
			$last = $first + 15;
			if ($last > $len) {$last = $len;}
			if ($i != $start) {
				print "<a href=\"index.php3?section=gallery/$page/$i\">$first - $last</a>\n";
			} else {
				print "$first - $last\n";
			}
		}
		print "<p>\n";
	}

}

?>
