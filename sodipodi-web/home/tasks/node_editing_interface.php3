<h3>Node Editing Interface</h3>

<P>Sodipodi's node editor allows intuitive editing of curves and
polygonal shapes.  This is one of Sodipodi's key features, but there's
still much work to do on it to make it more perfect.

<h4>Knife</h4>

<P>A new path operator should be added to enable splitting one path into
two, at a particular point.  This differs from the 'Break line at
selected nodes' function in that it results in two separate objects,
instead of one object containing two unconnected paths.

<p>For a connected path (like a polygon), the knife operator would merely
change the connected path into a non-connected path.

<h4>Punch</h4>

<p>Like Knife, this permits splitting one path entity into two, however
this does it using another entity, called the 'punch entity'.

<ul>
<li>If the punch entity is a non-connected path with both endpoints
laying outside the selected object(s), then the area enclosed by the
punch entity's line and the selected object(s) borders is deleted from
those objects.

<li>If the punch entity is a connected path and partially overlaps, then
the overlapped region is removed. 

<li>If the punch entity is a connected path and lays entirely inside the
selected entity, then the result is a 'donut'.

<li>If the punch entity is a connected path and completely intersects the
selected entity, such that the result will be two or more connected
paths, then each resultant connected path becomes a separate entity.
</ul>

<p>Applying a punch to a selection of objects results in the deletion of
the punch entity.

<h4>Crop</h4>

<p>This is similar to Punch but does the opposite.  Instead of deleting
anything contained <u>within</u> its boundaries, it clips off anything
that lays <u>outside</u> its boundaries.  The cropping object must be a
connected path (or convertable to a connected path).

<h4>Join</h4>

<p>When two or more objects are selected, this path operator does the
following:

<ol>
 <li>Convert the selected objects to paths
 <li>Merge the selected objects into a single object
 <li>Any place that the paths intersect or overlap, the paths are
     joined. 
</ol>

<h4>Intersection</h4>

<p>When two or more connected paths are selected, this command produces
a new connected path is generated that corresponds to the intersection
of all of them.

<h4>Adding Nodes at Points</h4>

<p>Currently, the way to add new points to a path is to select two or
more node points and click the 'Insert new nodes into selected segments'
button, which causes a new node to be added at the midpoint between each
pair of nodes.

<p>A mechanism should be added to permit "dropping" new nodes onto the
path at arbitrary locations.  If snap is turned on, the points should
appear at the appropriate snapped locations, and the curve revised
accordingly (i.e., pulled to go through the snap line.)



<h4>References</h4>

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=470751&group_id=4054&atid=354054">470751</A>]
- "Multiple niceties"</p>

<p>[<A HREF="http://sourceforge.net/tracker/index.php?func=detail&aid=469121&group_id=4054&atid=354054">469121</A>]
- "curve editing and right click menu"</p>



