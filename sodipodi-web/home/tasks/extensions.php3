<h3>Extensions</h3>

<p>The following is a list of tasks that could be implemented as
extensions (e.g., perl scripts accessed via the extensions
mechanism).  Most were originally suggested by Daniel Goude
[goude/dtek_chalmers_se].</p>


<ul>
<li> Evaluate perl expression
<li> Search/Replace (uses the above)
<li> Drop shadow (again, but better)
<li> Rasterize (using sodipodi --export-png - probably path issues here.
Putting the file in sodipodi:docbas seems like the best solution right
now)
<li> Image Magick > Blur
<li> Drop shadow (raster version, using the two filters above to blur the
shadow)

</ul>

<h4>sp_extensions.pm</h4>

<p>This perl module provides the following capabilities:</p>

<ul>
<li> provides getopt parsing according to the specs (you can add your own
arguments if you want)

<li> provides a way to exit with a specified exit codes (and an appropriate
message), i.e. "sp_die('MEM_ERR', 'Helpful message);"

<li> lets you iterate over a number of SVG fragments identified by '--id',
using XML::XQL.
</ul>

<P><i>I will be looking over SVG.pm to see if it can replace XML::XQL
for getting specific nodes by id. If all else fails, I'll have to put
manual code for this in "sp-extension.pm", but I'd rather not.) --
Daniel</i></p>


