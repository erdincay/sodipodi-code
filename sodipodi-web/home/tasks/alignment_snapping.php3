<h3>Alignment Snapping</h3>

<p>Currently, all snapping goes to infinite grid lines.  The grid
snapping is done independantly in each dimension.</p>

<h4>Algorithms for alternate grid options</h4>

<p>Currently canvas-grid.[ch] handles rectangular grids.  We need a more
abstract mechanism that permits, e.g., iso and hex type grids.

<h4>UI for alternate grid options</h4>

<p>Add a dropdown to allow choice of different kinds of grids, including
iso and hex.</p>


<h4>Snap to Horizontal or Vertical</h4>

<p>Currently Sodipodi snaps to both horizontal and vertical.  Update the
code to make it to one or the other, depending on the value set in the
horizgrid and vertgrid attributes.


<h4>Horizontal/Vertical Snap UI</h4>

<p>Add a grid snapping checkboxes for causing Sodipodi to only snap to
horizontal lines (i.e., turn off the vertical snap.)  This must update
the horizgrid and vertgrid attributes in sp-namedview.[ch].</p>

<p><i>Owner:</i>  Bryce Harrington (2003/30/18) [bryce/osdl_org] </p>
<p><i>Status:</i> Done - patch sent to njh</p>


<h4>Radial Snap UI</h4>

<p>Implement snap along radial spokes.  The user is able to specify the
origin(s) of the radials.  They can specify one or multiple of these and
can select how long the spokes should be.</p>

<h4>Vector Snap UI</h4>

<p>Implement snapping to a vector.  Vector snap is already there, it is
just waiting for someone to add non-axis aligned guide lines.
</p>

<h4>Snap to Object Edges / Segments UI</h4>

<p>Objects assigned the 'snap' attribute with the value 'edge', are to
have segments along their boundaries be attractive.  Implementation can
be done by creating a snap-to-segment capability (i.e., finite line
instead of infinite line).</p>

<h4>Snap to Points UI</h4>

<p>Enable the user to add special 'snap-points' to objects in the
drawing.  The user can enter a positive or negative number, which is
summed with the normal snap value for that area of the object.  This
could be used, for example, to create symbols for flowcharts and to be
able to manually specify where lines/arrows/etc. snap on.</p>

<p>Editing of snap points works analogously to editing of node
points.</p> 

<p>In addition, provide support for assigning objects the 'snap'
attribute with the value 'center', which causes the object to have a
snap point at its center.</p>

<h4>Horizontal Snap UI</h4>

<p>Add a grid snapping setting that causes Sodipodi to only snap to
horizontal lines (i.e., turn off the vertical snap.)</p>

<h4>Vertical Snap UI</h4>

<p>Add a grid snapping setting that causes Sodipodi to only snap to
vertical lines (i.e., turn off the horizontal snap.)</p>

<h4>Radial Snap UI</h4>

<p>Implement snap along radial spokes.  The user is able to specify the
origin(s) of the radials.  They can specify one or multiple of these and
can select how long the spokes should be.</p>

<h4>Vector Snap UI</h4>

<p>Implement snapping to a vector.  Vector snap is already there, it is
just waiting for someone to add non-axis aligned guide lines.
</p>

<h4>Snap to Object Edges / Segments UI</h4>

<p>Objects assigned the 'snap' attribute with the value 'edge', are to
have segments along their boundaries be attractive.  Implementation can
be done by creating a snap-to-segment capability (i.e., finite line
instead of infinite line).</p>

<h4>Snap to Points UI</h4>

<p>Enable the user to add special 'snap-points' to objects in the
drawing.  The user can enter a positive or negative number, which is
summed with the normal snap value for that area of the object.  This
could be used, for example, to create symbols for flowcharts and to be
able to manually specify where lines/arrows/etc. snap on.</p>

<p>Editing of snap points works analogously to editing of node
points.</p> 

<p>In addition, provide support for assigning objects the 'snap'
attribute with the value 'center', which causes the object to have a
snap point at its center.</p>

<h4>Precise Positioning of Guidelines</h4>

<p>For things like business cards or CD covers it's necessary to be able to
exactly specify where the guidelines lay.  This can be done with the XML
editor but there should be an easier way to do this.</p>

<p>Requested by Gilles Schintgen</p>

<p><i>There was once such a guideline positioning dialog, written by
frank Felfe - but it got lost sometimes during Gtk+2 porting :-( Anybody
interested - the code should be either there, or if not, available from
old releases.</i> -- lauris</p>
