<h3>Alignment Snapping</h3>

<p>Currently, all snapping goes to infinite grid lines.  Snapping to
grid corners is the sum of the horizontal and vertical snaps, which is
what makes grid corners especially "attractive".</p>

<h4>Horizontal Snap</h4>

<p>Add a grid snapping setting that causes Sodipodi to only snap to
horizontal lines (i.e., turn off the vertical snap.)</p>

<h4>Vertical Snap</h4>

<p>Add a grid snapping setting that causes Sodipodi to only snap to
vertical lines (i.e., turn off the horizontal snap.)</p>

<h4>Radial Snap</h4>

<p>Implement snap along radial spokes.  The user is able to specify the
origin(s) of the radials.  They can specify one or multiple of these and
can select how long the spokes should be.</p>

<h4>Vector Snap</h4>

<p>Implement snapping to a vector</p>

<h4>Snap to Object Edges / Segments</h4>

<p>Objects assigned the 'snap' attribute with the value 'edge', are to
have segments along their boundaries be attractive.  Implementation can
be done by creating a snap-to-segment capability (i.e., finite line
instead of infinite line).</p>

<h4>Snap to Points</h4>

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

