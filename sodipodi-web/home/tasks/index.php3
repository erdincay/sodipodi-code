<?
    $meta = data_read_meta ($directory."meta");

    $data = $meta[$argv[0]];
    if ($data['file'] && file_exists ($directory.$data['file'])) {
	include ($directory.$data['file']);
    } else {

	include ($directory."index.html");

        $keys = array_keys ($meta);
	$len = sizeof ($keys);
	print "<table border=0 width='90%' cellspacing='0'>\n";
	for ($i = 0; $i < $len; $i++) {
	    $data = $meta[$keys[$i]];
            if ($i % 2 == 0) {
               print " <tr bgcolor=#EEEEEE>\n";
            } else {
               print " <tr bgcolor=#FFFFFF>\n";
            }
	    print "  <td> * </td>\n";
	    print "  <td><a href=\"index.php3?section=development/tasks/${data['name']}\">${data['title']}</a></td>\n";
            print "  <td>${data['status']}</td>\n";
            print " </tr>\n";
	}
	print "</table>\n";
    }
?>
