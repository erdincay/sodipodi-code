<h3>Keyboard Shortcuts</h3>

<p>Documentation of 
<a href="http://sodipodi.sourceforge.net/index.php3?section=documentation/usage/keybindings">Currently
implemented keyboard shortcuts</a> is available from the website.</p>

<p>The following is a detailed proposal from bulia byak [bulia/dr_com]
regarding keyboard shortcuts.  This has been discussed in the sodipodi
mailing list (in late March 2003).  Feedback from that series of
discussions has been folded into this proposal.</p>

<p>This only covers general contexts and two of the tools,
selector and node editor; other tools will be explored later.
The new keys are not only more usable but also more familiar for
users of other vector editors, mostly Windows ones.  Some of
the shortcuts require new functions, but mostly they remap what
is already available in the menu.</p>

<p>It is very important that all unhandled events are forwarded
from dialogs to the canvas, as described on the
<a
href="http://sodipodi.sourceforge.net/index.php3?section=development/tasks/usability">usability
task page</a> so that if a shortcut such as open a document (ctrl-o) is
struck when the toolbox is in focus, that's very bad usability indeed.</p>

<p>ALL shortcuts must be displayed next to the corresponding
commands the menu.</p>


<h4>Selector</h4>

<table border=0 width=100%>
 <tr>
  <td>  ctrl a   </td>
  <td>    select all objects                  </td>
 </tr>

 <tr>
  <td>  esc      </td>
  <td>    deselect (badly needed!)            </td>
 </tr>

 <tr>
  <td>  ctrl u   </td>
  <td>    ungroup, in addition to ctrl-G; ctrl-u is common in other
          vector apps  </td>
 </td>

 <tr>
  <td>  ctrl Z    </td>
  <td>    redo; ctrl-r is consistent with GIMP but not familiar to
          windows users (I would rather expect ctrl-r to be "redraw")
  </td>
 </tr>

 <tr>
  <td>  ctrl y    </td>
  <td>    another common Windows shortcut for redo </td>
 </tr>

 <tr>
  <td>  keypad +/-   </td>
  <td>    zoom, same as +/- on the main keyboard (also with ctrl)
          (currently these don't work, at least for me under
          Linux; it was reported that they work under Windows)
  </td>
 </tr>

 <tr>
  <td>  space     </td>
  <td>    it calls the menu in all other tools but not in
          selector - why? Space should always display the menu.
  </td>
 </tr>

 <tr>
  <td>  arrows:  </td>

  <td>    <p>Now both arrows and shift-arrows move selection in pixel
          units. This means that the actual movement depends on the zoom
          level. This is sometimes useful and sometimes not. Xara has
          movements by one visible pixel (alt-arrows) and two modes of
          movement by a fixed pixel-independed amount (arrows and
          ctrl-arrows), which is more convenient. So I propose a similar
          scheme:</p>

          <p><i>arrows</i> -- move by some fixed amount X (settable in preferences,
             e.g. 5mm by default), independent of zoom</p>
          <p><i>shift-arrows</i> -- move by amount 10*X</p>
          <p><i>alt-arrows</i> --  move by one visible pixel (same as shift-arrows now);
             this is consistent with "alt + transform = slow movement"

          <p>This should include keypad arrows which currently don't work.</p>
  </td>
 </tr>
</table>

<h4>Node editor</h4>

<table border=0 width=100%>
 <tr>
  <td>  arrows, shift-arrows, alt-arrows:</td>
  <td>    The same as in selector, but applying to the currently
          selected node(s)
  </td>
 </tr>

 <tr>
  <td>  ctrl-alt-arrows:</td>

  <td>    Move both control points of the currently selected node
          horizontally (right/left) or vertically (up/down) in opposite
          directions, rotating the control handles around the selected
          node 
  </td>
 </tr>

 <tr>
  <td>  del:  </td>

  <td>    Now it works only while you're actually dragging the points, and
          affects only one node even if more are selected. I think it
          should work on all selected nodes when they are not being
          dragged. The same applies to most other keys (c, s, etc.)
  </td>
 </tr>
</table>

<br> 
<p>Control point drag modifiers:</p>

<table border=0 width=100%>
 <tr>
  <td>  ctrl   </td>
  <td>    angle constrained movement (0, 15, ... degrees etc),
          relative to its node
  </td>
 </tr>

 <tr>
  <td>  alt   </td>
  <td>    move slower</td>
 </tr>

 <tr>
  <td>  ctrl-shift </td>
  <td>    keep the direction unchanged, change only distance
          from node</td>
 </tr>

 <tr>
  <td>  alt-shift </td>
  <td>    keep the distance unchanged, change only direction
          (i.e. rotate control point around the node)</td>
 </tr>

 <tr>
  <td>  ctrl-alt  </td>
  <td>    preserve the angle at the node: the opposite control
          point moves so that the angle between the handles is
          constant</td>
 </tr>
</table>

<br>
<p>Node drag modifiers:</p>

<table border=0 width=100%>
 <tr>
  <td>  ctrl  </td>
  <td>    angle constraint movement (0, 15, ... degrees etc),
          relative to the original position</td>
 </tr>

 <tr>
  <td>  alt  </td>
  <td>    move slower</td>
 </tr>

 <tr>
  <td>  ctrl-shift</td>
  <td>    move the node but not its control points</td>
 </tr>
</table>

<p>Copying and pasting nodes:</p>

<p>When a node is selected and ctrl-c is pressed, what goes to cilpboard is
the node type (cusp/smooth) and the _relative_ positions of its control
points, but not the coordinates of the node itself. To paste it, you
must first select one or more nodes; then you press ctrl-v and the
stored parameters are applied to the selected nodes (only their control
points are moved but not the nodes themselves). If you selected and
copied e.g. 4 nodes, and then select 6 nodes and press ctrl-v, the first
4 nodes get the parameters of the 4 copied nodes, and the remaining 2
nodes become the same as the first two (i.e. if the number of copied
nodes is more than the number of nodes selected during paste, the
sequence is truncated, otherwise it is repeated). The cut operation
(ctrl-x) is equivalent to the copy plus delete node.</p>



<h4>XML Editor</h4>

<table border=0 width=100%>
 <tr>
  <td>  Del  </td>
  <td>    delete element or attribute </td>
 </tr>

 <tr>
  <td>  Ins  </td>
  <td>    new element or attribute (depending on which pane
          is in focus)</td>
 </tr>

 <tr>
  <td>  ctr z, ctrl r, etc </td>
  <td>    undo and redo must work in the editor, undoing
          the last XML editor action</td>
 </tr>

 <tr>
  <td>  shift-arrows </td>
  <td>    selecting more than one element or attribute</td>
 </tr>

 <tr>
  <td>  ctrl-c, ctrl-x, ctrl-v  </td>
  <td>    copying and pasting must work on selected
          lines in the lists, so you can rearrange the tree by cutting
          and pasting stuff</td>
 </tr>
</table>

<h4>Everywhere</h4>

<p>Calling dialogs (these shortcult must toggle corresponding dialogs
on or off):</p>

<table border=0 width=100%>
 <tr>
  <td>  ctrl T </td>
  <td>    transform selection</td>
 </tr>

 <tr>
  <td>  ctrl F </td>
  <td>    fill and stroke (by the way the dialog is named
          differently than the command - needs to be unified)</td>
 </tr>

 <tr>
  <td>  ctrl O </td>
  <td>    tool options </td>
 </tr>

 <tr>
  <td>  ctrl E </td> 
  <td>    text properties (again name of the command is different)</td>
 </tr>

 <tr>
  <td>  ctrl A  </td>
  <td>    align objects</td>
 </tr>

 <tr>
  <td>  ctrl D </td>
  <td>    desktop settings (again name of the command is different)</td>
 </tr>

 <tr>
  <td>  ctrl X </td>
  <td>    XML editor</td>
 </tr>
</table>

<p>Changing tools:</p>

<p>Gimp uses letter keys to change tools (z for zoom, etc.) It's very
convenient and we should enable this too (in addition to function
keys). It's also useful since there are more tools than function keys,
so we can use letters to select arc, star, etc which are not accessible
via F keys.</p>

<p>One problem: in node editor c, s, b, and y are used for various
functions specific to this tool. Perhaps we'll need to remap them to
some ctrl- or alt- combinations. Also, the letter keys will be blocked
by the text tool and when you are editing values in dialog text fields,
but that's normal.</p>

<h4>Missing pieces</h4>

<p>These shortcuts are listed on the
<a
href="http://sodipodi.sourceforge.net/index.php3?section=documentation/usage/keybindings">keybindings
page</a> on the website but don't appear to work (0.31.1 on linux):

<table border=0 width=100%>
 <tr>
  <td>  Ctrl e </td>
  <td>    export file </td>
 </tr>

 <tr>
  <td>  Ctrl P  </td>
  <td>    print preview</td>
 </tr>
</table>

<p>Already work but not in the list:</p>

<table border=0 width=100%>
 <tr>
  <td>  F8  </td>
  <td>    dropper</td>
 </tr>

 <tr>
  <td>  F10  </td>
  <td>    "Sodipodi" menu on toolbox</td>
 </tr>
</table>






