<?

include ("style.php3");
include ("library/data.lib");


/* Set default section */
if (!$section) {$section = "home";}

header ("Content-Type: text/html; charset=UTF-8");

data_show_page ($section);

?>
