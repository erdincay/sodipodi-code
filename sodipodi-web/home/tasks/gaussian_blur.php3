
<h3>Gaussian Blur</h3>

<p><i>Status:</i>  Proposed (unreviewed)</p>

<p>Of all bitmap filters, Gaussian blur is by far the most often used
one, and there's likely not any serious obstacle to implementing it in a
<u>vector</u> fashion, simply by storing blur radius as a separate
attribute for each object and generating the blur "on the fly" for
display at the current size and magnification.</p>

<p>There will be some limitations of course. In bitmap blur, each pixel
is calculated taking into account all neighbouring pixels; in vector,
although the neighbourhood of a point may contain parts of more than one
object, you'll probably have to blur each object separately, as if it
was in its own transparent layer (although I may be wrong on this). But,
since blur is applied not only to colors but to the transparency level
(e.g. a point sitting on the outline of an opaque object will be 50%
transparent because it is blurred with the fully transparent outside),
overlaying blurred objects will be able to make for some amazingly
bitmap-looking effects.</p>

<p>Sodipodi is probably mature enough to implement this feature, as it
has transparency and even transparency gradients. Granted, this is not
in SVG, but Sodipodi already has some extensions added to the basic SVG,
so this would add one more. When exporting into "pure" SVG it would be
possible to render the vector-blurred objects as embedded bitmaps.</p>

<p>On the "ideas for extensions" page there's an idea for an ad hoc drop
shadow tool.  This proposal is similar but more of a general purpose
vector based blur of any given object, not merely drop shadows. A
blurred drop shadow is only a special case which is very easy to
implement using some sort of macro once you have vector blur
available.</p>

<p>With vector blur implemented, we'll be able to achieve one effect
which is difficult if possible at all in bitmap editors: blur gradients!
Imagine all Sodipodi's gradient types (linear, radial, etc.) with the
blur radius as the variable parameter. Just one use for it: it will
enable imitation of the photographic "depth of field," i.e. blur all
points of an object variably and smoothly depending on their distance
from the focus point.  E.g. take a closeup macro photo of a ruler
pointed away from the camera. In GIMP/Photoshop, imitating such a photo
is a clumsy process whose result is never quite satisfactory (basically
you have to overlay the blurred copy of the object over a crisp copy
with a transparency gradient). With a vector blur gradient, it's much
easier and, what's important, much more realistic.</p>

<p>Here's a further idea. Instead of storing just the blur radius, we
could store two radii and an angle, which together define an ellipse
that you can squeeze or rotate arbitrarily. Blurring each point within
this ellipse will be equivalent to photoshop's "motion blur", and you
can adjust both the direction of the motion (by rotating the large axis
of the ellipse) and the amount of "motion" by squeezing the
ellipse. When the ellipse becomes a circle, we get plain directionless
Gaussian blur.</p>

<p>Any of the parameters of such elliptic blur can be changed along a blur
gradient, so that, for example, you have the left side of an object
(e.g. an imported photo) blurred directionlessly by a circle, while the
right side is violently "smeared" vertically by a narrow ellipse - and
everything in between is smoothly transitioned from one blur mode to the
other.</p>

<p>And here's one more straightforward application for blur gradient
(besides depth of field): shadows. Imagine a vertical pole that casts
shadow on the ground. The further away from the pole foot, the more
blurred is the shadow.  You'll need to use blur gradients for
reproducing such shadows. Note that these are only the most basic
"photorealistic" applications; no doubt artists will find lots of other
unexpected uses for this tool.</p>

<p>SVG has built-in descriptions for filter primitives in its DTD [1].
Batik [2] can be used to render example SVGs written with
filters. However, even with the SVG specification supporting filters,
including filter rendering in Sodipodi is a whole different ballgame.
</p>

<p>A simple gaussian blur of sourceGraphic masked by an alpha gradient
and composited over the original graphic should do the trick.</p>

<p>With a little creativity, it's even possible to 'fake' e.g. linear
motion blur.  (Mental has some recipes for various things floating
around somewhere, if anyone's interested)</p>


<h4>Contributors</h4>

<p> bulia byak [bulia/dr_com]</p>
<p> M. Seymour [seymour/astro_queensu_ca]</p>
<p> MenTaLguY  [mental/rydia_net</p>


<h4>References</h4>

<p>[1] http://www.w3.org/TR/SVG/filters.html</p>
<p>[2] http://xml.apache.org/batik/</p>
