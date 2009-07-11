			    -----------------------
			    |	SOUSAPHONE HERO   |
			    |       v. 0.1        |
			    |     David Woods     |
			    -----------------------

Requirements:
-------------

SDL and SDL-mixer libraries (provided)
OpenGL 1.0

Controls:
---------

The keys 'a', 's' and 'd' or the left, middle and right mouse buttons should map to the left, middle and right keys of the sousaphone, respectively. (The middle mouse button may not work!)

Instructions telling the player what to play will scroll across the top of the screen. The note should be played when it reaches the centre of the screen, indicated by the yellowy gradient.

Points are awarded for notes correctly played. Points are deduced for notes played at the wrong time, extra notes, incorrect notes and notes not played.

Editable Files:
---------------

timbre.txt :

The file timbre.txt defines the sound of the sousaphone. I believe this is already perfect, but should you wish to change it then the format is:

<wavetype> <frequency multiplication> <relative volume>

Valid wavetypes are sine (or sin), square (or sq), saw, triangle (or tr).
Frequency multiplication determines the pitch of the wave, relative to the root pitch. Relative volume is what it says. For example, the default file is

sin 1.0 1.0 sq 1.0 0.7 saw 0.5 0.5 tr 1.5 0.7 sin 2.0 0.2

tune files :

The user can also specify a tune. To play sousaphone hero with a tune drag the tune file over the shero.exe program. If no tune is specified the advanced internal composer function is activated.

In a tune file the characters a,b,c,d,e,f and g specify a sequence of notes. White space is ignored. A full stop specifies a pause one note long.

Additionally, a number of commands can be used, which must be put in < > brackets.

<tempo number> specifies a tempo in beats per minute
<tempomult number> multiplies the current tempo by the number. This can be used to increase the speed with each playing of the tune.
<key a b# db e 200> (for example) specifies the key. The note 'a' is set to the default pitch of a-natural. The note 'b' is set to the default pitch for b-sharp. The note 'd' is set the the default pitch for d-flat. The note e is set to 200Hz. Remember that key changes will be preserved when the tune loops back to the start, so temporary changes should be reversed at the start of the file.