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

<h4>External Gradient Libraries</h4>

<p>Currently, gradients are stored in the document file.  A capability
to load or save gradients to an external SVG file is to be added.  This
saved file will contain <u>only</u> gradients.</p>

<p>If the user chooses to import gradients, these will be appended to
the user's current list of gradients.  If the file imported from
contains more than gradients, only the gradients are imported.</p>

<h4>Global and User Gradient Library Files</h4>

<p>There are two gradient libraries recognized by Sodipodi at load
time:  One stored in Sodipodi's standard config directory at install
time, and another kept in the user's ~/.sodipodi directory.  Both are
named gradients.svg.</p>

<p>A palette.svg file (see Object Style Palette) specifies the subset of
gradients to load from these libraries.</p>

<p>If the user chooses to export the custom gradients, an option to
save to their ~/.sodipodi/gradients.svg should be given.</p>

<h4>Import/Export GIMP (& etc.) Gradients</h4>

<p>There are a number of very good gradients already available for GIMP,
however they are implemented using a GIMP-specific gradient file format
(see References).  Routines should be added to Sodipodi that can
translate between this format and the SVG gradient format, and
appropriate code added for loading/saving the GIMP-formatted files.</p>

<h4>New Gradient Editing Tool</h4>

<p>Implement a new tool for editing linear and elliptic gradients by
dragging control points right on the canvas, instead of in a
dialog. Switching to this tool should make control points of the
selected object visible, otherwise they are hidden. There are at least
two control points for a linear gradient (start and end of the gradient
vector) and four for radial gradient (center, ends of the two axes,
focus point). The tool must calculate the correct cx, cy, fx, fy, r,
gradientTransform attributes for each position of control points, and it
must parse these attributes and calculate the control points when the
document is loaded. In case of multi-stop gradients, one control point
for each stop is added to the vector (for linear gradients) or to the
segment connecting the focal point with the end of one of the axes (for
elliptic gradients). The axes of the elliptic gradient are always
perpendicular; if the object is skewed, the gradient is automatically
unskewed into a normal ellipse that is equal to the skewed one, by
changing the axes ratio and rotation angle (contact Nathan Hurst for the
unskewing formulas - 
Nathan Hurst [Nathan.Hurst/infotech_monash_edu_au]).</p>


<h4>References</h4>

<p><A HREF="http://lists.xcf.berkeley.edu/lists/gimp-developer/2002-May/007049.html">"Format of GIMP gradient files"</A>

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=517402&group_id=4054&atid=354054">517402</A>]
- "needed features"

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=696242&group_id=4054&atid=354054">696242</A>]
- "Import GIMP Gradients"
