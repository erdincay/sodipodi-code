<?

$d = getdate ();
$b = ($d['yday'] + $d['hour']) % 19;
$c = $b % 5;
$r = rand (0, 5);

if ($r > $c) {
    print "<html>\n";
    print "<head><title>Darkness</title></title>\n";
    print "<body bgcolor=\"black\">\n";
    print "<p align=\"center\">\n";
    print "<img width=\"200\" height=\"400\" src=\"kurayami.png\">\n";
    print "</p>";
    print "</body>\n";
    print "</html>\n";
} else {

include ("style.php3");
include ("library/data.lib");


/* Set default section */
if (!$section) {$section = "home";}

header ("Content-Type: text/html; charset=UTF-8");

data_show_page ($section);
}

?>
