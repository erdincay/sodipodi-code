<h3>Animation</h3>

<p>The SVG spec defines how animation sequences can be specified in an
SVG document.  This programming task is to add support for animation
editing and display to Sodipodi.

<p>To date (Version 0.32), this has been outside the scope of Sodipodi
development, though if someone is interested in adding it, it would be a
valuable extension to the software.</p>


<h4>Buttons</h4>

<p>Create a patch that adds the following buttons into a toolbox area:
Record, Play, Stop, Pause, Rewind, Fast-forward.</p>

<h4>Timeline</h4>

<p>Add a progress bar that can show the relative play position of a
given animation sequence.</p>

<h4>Interactive Animation Recording</h4>

<p>Implement the record button.  When toggled on, this enables the user
to record animation actions for drawing elements.  This works as
follows:</p> 

<ol>
 <li>Press record button (timeline is set to 0 sec)
 <li>Move, scale, add, delete, etc. drawing element(s)
 <li>Press either the Stop button or the Record toggle 
 <li>This defines one "Animation Sequence"
</ol>

<h4>Animation Editing</h4>

<p>Conceptualize, design an editing GUI dialog that allows precise
setting and editing of animation behaviors.  The idea is to provide
something analogous to the other Sodipodi editing tools for precise
translation, rotation, color specification, etc.

<h4>Animation Path Design</h4>

<p>Here is another approach for how to handle animation sequence
creation within Sodipodi, suggested by Thomas Ingham.</p>

<p>For travel paths, the user can draw a "movement path" as a regular
interpolated path.  The user selects this and the item(s) to follow that
path and clicks an appropriate button to pop up the animation properties
dialog.  This dialog has the following options:</p>

<ul>
  <li> Use open path to dictate motion (use_path)
  <li> Rotate object along heading of path / shape (fixed_heading)
  <li> Time to execute animation
</ul>

<p>Once the dialog has been applied, "Make Animation" would then use the
movement path and calculate heading, distance, and position as functions
of time.  If fixed_heading is specified, the entity(ies) would be
rotated such that the top of the shape (top by its original position)
would always face the average of its current point and the next point to
reach.</p>

<p>If the user did not select Use open path... for use_path, Sodipodi
would then examine the closed shape to determine movement.</p>

<p>Similar mechanisms could be established for designating and
controlling animation behavior of properties.</p>

<h4>Animation Viewing</h4>

<p>Implement the Play, Stop, Pause, Rewind, and Fast-forward buttons to
enable viewing of the animation sequences.</p>

