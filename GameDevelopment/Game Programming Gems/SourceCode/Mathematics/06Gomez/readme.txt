Recommended System:

PIII 600Mhz with NVidia GeForce 256 or faster, with latest OpenGL drivers


Controls:

Rotate Camera - left mouse click and drag
Move Forward/Backward - arrow keys


Purpose:

The "Floating Boat" demo combines material presented in the "Wave Equation" 
and "Rigid Body" gems to simulate a buoyant object on a dynamic water surface.


Things to notice:

1)  Initially, the water is displaced from equilibrium about a third of the 
way into the grid.  Waves emanate radially from this initial "splash."

2)  When the waves hit the boat, it rocks back and forth.  This is because 
the Newton-Euler equations are used to simulate rigid-body motion.

3)  When the waves hit the boat, it is pushed in the direction of the 
wave.  This is because buoyant force is applied in the direction normal 
to the water surface (as opposed to vertically).

4)  There is a circular "island" about x=1, y=1 (right behind the boat) 
where the damping coefficients are 0.  Notice how the waves diffract 
around this dead region and create a wave "shadow" behind it.


Apologies for the code:

Yes, the code is a little messy, it was written in haste, but hopefully 
there is enough commenting so that it can be followed after or while 
reading through the gems.

Sincerely,
Miguel Gomez
