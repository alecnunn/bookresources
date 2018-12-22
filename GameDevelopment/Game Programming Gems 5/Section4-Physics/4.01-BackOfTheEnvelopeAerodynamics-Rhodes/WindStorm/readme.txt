/*****************************************************************************
|
| WindStorm
|
| by Graham S. Rhodes
| September 2004
|
| A simple demonstration of back of the envelope aerodynamics. This demo
| simulates a simple tornado, with low mass, spherical particles moved by
| an aerodynamic drag force.
|
| FOR FUN:
|
|  - Try placing multiple tornadoes in the world, and see how particles
|    behave
|
|  - Try making the core of the storm something other than a vertical
|    line, e.g., make a tornado whose centerline is curved, more like
|    real life storms.
|
|  - Change the variation of storm strength with altitude to get a shape
|    that is more realistic.
|
|  - Pretend the particles are spinning at different rates, and add in
|    a lift force calculation based on the particle spin. (This can
|    cause particles to gain altitude, perhaps rising above the storm,
|    then shooting out like a normal projectile only to reenter the
|    storm again once their altitudes decrease. Could look pretty cool.
|
|  - Try the above, but animate the shape of the vortex core line.
|
|  - Try a weaker storm and scatter particles much further away from the
|    core. Make a hurricane instead of a tornado!
|
|  - Kill particles once they reach some maximum distance from the core,
|    based on altitude, then respawn new particles in the center of the
|    storm so you always have particles in the center. (Depends on what
|    you want the thing to look like, and whether or not you'll be able
|    to see the core within your game.)
|
| FOR EFFICIENCY AND STABILITY:
|
|  Use fewer particles, fancier rendering, a true particle system that
|  deals with the life/evolution/death of particles, etc.
|
|  (A BIG DEAL) Upgrade to use Verlet or Runge-Kutta integration. This will
|  help stability. The system, as is, is conditionally stable meaning its
|  stable for certain combinations of storm strength, particle mass, particle
|  radius, and physics time step. The stability bounds can be violated and if
|  this happens particles will behave strangely, overflow and other bad things
|  may happen, etc.
|
|  Render particles as a batch?
|
| This demo is a companion to the chapter titled "Back of the
| Envelope Aerodynamics for Game Physics" in the book "Game
| Programming Gems 5" published by Charles River Media