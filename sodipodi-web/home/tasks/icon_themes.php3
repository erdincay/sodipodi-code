<h3>Icon Themes</h3>

<h4>Implement Icon Theme Support</h4>

<p>Currently Sodipodi includes numerous custom icons, which look nice
but are inconsistent with the standards of desktop environments (such as
GNOME).  Abstract out Sodipodi's icon loading scheme to allow us to
provide alternate sets of icons, or to pull from the current desktop
theme for appropriate things (such as file save/load).

<p><i>Btw - got interesting idea - how about distributing 'image
components' of themes? For example: Magnifying glass image with
semitransparent inside. So sodipodi can construct 'zoom page', 'zoom
drawing' and 'zoom selection' from stock + custom layers?  Thus it could
achieve better look under icon themes even for certain customized icons?
-- lauris</i></p>


<p><i>Owner:</i>  Pat Suwalski (2003/03/18) [pat/suwalski_net]</p>
<p><i>Status:</i>  DONE.  0.32 now uses the stock icons from the current
theme, from icons.svg.  So while not themable in the perfect sense of
the word, it's at least easy for someone to replace the icons.svg file
with whatever they wish, without recompiling.</p> 
