<h3>Add atoi/atof/itos/itof functions</h3>

<p>There was a bug in Sodipodi related to parsing and writing of
numbers to strings for some locales (see References, below).  The
workaround implemented to correct for this was to force LC_NUMERIC "C".
This task involves writing a replacement for atoi, atof, itoa, and ftoa
for doing number->string and string->number conversions.</p>

<p>The SVG spec itself accepts exponential number variants, but CCS2
does not.  Having atoi and cousins available will provide a more
reliable solution than forcing the LC_NUMERIC workaround.</p>

<h4>atoi</h4>

<p>This routine takes an unsigned char * as input and returns the
corresponding integer.</p>

<h4>atof</h4>

<p>This routine converts strings to floating point representations.  It
must be able to deal with both normal floating point and exponential
formatted numbers.  E.g., 1.2345e9 and 1234500.  This will allow either
form to be used both in SVG attributes (like rect coords, etc.) and in
CSS.</p>

<p>This routine takes an unsigned char * as input and returns the
corresponding floating point number.</p>

<h4>itoa</h4>

<p>This routine takes an integer and returns the string formatted
version of it as an unsigned char *.</p>

<h4>ftoa</h4>

<p>This routine takes a floating point (double) as input and returns the
string formatted version of it as an unsigned char *.  It needs to be
able to provide controls to force representation of numbers in either
exponential or non-exponential formats.</p>

<p>This would provide an alternate to strnprintf(s, "%g", num).</p>


<h4>References</h4>

