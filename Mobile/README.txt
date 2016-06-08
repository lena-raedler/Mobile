This Project was created by:
Simon Priller
Anna-Lena Rädler
Sabrina Schmitzer

It requires libGLEW.so.1.13.

For the fourth Programming assignment we tried to add textures and billboards to out Mobile implementation.
One texture worked nearly at first try, but it does get shown on the middle bar too (we don't know why).
The other texture we wanted to add to all objects, but it didn't work and resulted in Segmentation Faults.

Functionality for the BillBoard exists, but it doesn't get shown at all. But you can make the whole scene black with it.

Our keyboard functions:

the normal mode which is activated when pressing 1
	here you can:
	stop (p), change direction (s), go back to normal (o)
	speed up (+), slow down (-) and
	quit (q)

and the full user control mode which is activated when pressing 2
	here you can:
	zoom in (8), zoom out (5), translate camera left (4), translate camera right (6),
	reset to normal (o), quit (q), change Hue (h), change value (v),
	turn off ambient light (a), turn off diffuse light (d), turn off specular light (s)
	here you also can use the mouse:
	left button - speed up
	right button - slow down
	middle button- change direction

To run the project use make and ./Mobile_Shader
