<?
	if (file_exists ("lc")) {
		$fp = fopen ("lc", "r");
		if ($fp) {
			$count = fgets ($fp, 32);
			$count = $count + 1;
			fclose ($fp);
		}
		$fp = fopen ("lc", "w");
		if ($fp) {
			fputs ($fp, "$count\n");
			fclose ($fp);
		}
	}
?>

<h3>Lauris Kaplinski</h3>

<p>
I am 31 years old, living in Tartu, Estonia.
</p>

<img src="home/lauris.jpg" width="253" height="400" alt="Portrait of Lauris">

<p>
This is me :)
</p>

<h3>My activities</h3>
<ul>
<li>Sodipodi (you are here ;)
<li><a href="http://floyd.sourceforge.net">Floyd 3D library</A>
<li>There used to be internet ordering system written by me up,
    but given company has gone out of business (not my fault).
<li>I was the original Estonian translator of
    <a href="http://www.gnome.org">Gnome</a>
<li>I have very big park style garden at home, so in springs and summers I
    spend most time there
</ul>

<h3>Contacts</h3>

<p>
You can reach me sending email to
<a href="mailto:lauris@kaplinski.com">lauris@kaplinski.com</a>
</p>


