<?
    $meta = data_read_meta ($directory."meta");

    $data = $meta[$argv[0]];
    if ($data['file'] && file_exists ($directory.$data['file'])) {
	include ($directory.$data['file']);
    } else {

	include ($directory."index.html");

        $keys = array_keys ($meta);
	$len = sizeof ($keys);
	print "<p><ul>\n";
	for ($i = 0; $i < $len; $i++) {
	    $data = $meta[$keys[$i]];
	    print "<li>";
	    print "<a href=\"index.php3?section=development/tasks/${data['name']}\">${data['title']}</a></li>\n";
	}
	print "</ul></p>\n";
    }
?>
