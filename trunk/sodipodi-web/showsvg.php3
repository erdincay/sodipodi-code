<?

if (!$file) {
	echo "No file specified!";
}
$file = str_replace ('..', '', $file);

while (substr ($file, 0, 1) == '/') {
	$file = substr ($file, 1);
}

header ("Content-Type: image/svg+xml");

$fp = fopen ($file, "r");
fpassthru ($fp);
/* fclose ($fp); */

?>
