<h3>Object Style Palette</h3>

<h4>Custom Palette</h4>

<p> For fill colors and gradients there needs to be be a 'custom
palette', so that the user can "bookmark" particular colors or gradients
for future use.  Twelve small buttons should be added in the Color paint
area of the Object Style Dialog, and should show up to twelve custom
colors/gradients.  When a button is clicked, the color or gradient
settings in other widgets should be changed to match.  There should also
be a straightforward way for users to add/remove/change items in the
color palette.</p>

<h4>Palette Files</h4>

<p>Each document has its own custom palette, which is initially blank.
The user can load or save palettes to palette files, in standard formats
consistent with other graphics software.</p>

<p>A global palette.svg file should be installed with Sodipodi, that
provides the default palette settings for a blank document.  This can
include colors as well as any gradients listed in the global
gradients.svg file (see the Object Style Gradients task).</p>

<p>Optionally, the user can have a palette.svg file in their home
~/.sodipodi directory.  If present, this will be loaded instead of the
global default file, overriding it.  This file may refer to gradients in
both the global gradients.svg file, as well as a gradients.svg file in
the user's ~/.sodipodi directory.</p>

<h4>Color Dropper</h4>

<p><b>Status:</b>  Halfway implemented, needs more work.

<p>If the current palette has one or more undefined entries, then when
one selects colors with the dropper, then they are automatically added
to the palette, unless the user is holding the 'ctrl' key.</p>

<h4>Color Defaults Indicator</h4>

<p>In GIMP, there is an indicator showing the current foreground and
background colors.  If the user clicks on them, color selectors pop up
to allow making different selections.</p>

<p>Sodipodi needs an analogous mechanism, but it should include current
fill and stroke color settings.</p>

<h4>References</h4>

<p>
[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=468485&group_id=4054&atid=354054">468485</A>]
- "custom palette"
</p>

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=469121&group_id=4054&atid=354054">469121</A>
- "Curve editing and right click menu"
