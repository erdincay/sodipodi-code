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

<h4>Animation Viewing</h4>

<p>Implement the Play, Stop, Pause, Rewind, and Fast-forward buttons to
enable viewing of the animation sequences.</p>

