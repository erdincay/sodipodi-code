<h3>Object Style Gradients</h3>

<p>One of the things that makes sodipodi cool is the ability to
interactively define and edit gradients for objects in the drawing.
This has drawn a number of wishes and good ideas for making it even more
powerful.</p>

<H4>Multi-color Gradients</H4>

<p>Currently it is possible to do two-color gradients.  A logical
enhancement is to extend this to 3 or more color gradients.  For linear
gradients, this means that one should be able to specify several points
along the line, associating a different color for each.</p>

<p>The gradient vector dialog needs to be changed into a list of colors
with buttons to pop up a color selection dialog for each.  The sample
vector at the top of the dialog needs to be updated to show the effect
of the multiple colors.  This dialog also needs to have a way to edit
the name of the gradient.</p>

<h4>Import/Export GIMP (& etc.) Gradients</h4>

<p>Currently, gradients are stored in the current document file.  A
capability to load or save gradients to external files is to be added.
The format for this file should be the same as used for GIMP, if
possible.  If Sodipodi's gradient feature comes to exceed what is
possible to do with the standard GIMP gradient files, then the mechanism
should be changed to allow the user to select which gradient file format
to use (i.e., gimp or sodipodi or ...), and should warn if some of the
selected gradient's properties will be lost if stored to the specified
format.

<h4>References</h4>

<p><A HREF="http://lists.xcf.berkeley.edu/lists/gimp-developer/2002-May/007049.html">"Format of GIMP gradient files"</A>

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=517402&group_id=4054&atid=354054">517402</A>]</p>
- ""

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=696242&group_id=4054&atid=354054">696242</A>]
- "Import GIMP Gradients"
