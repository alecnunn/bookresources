New Notes:

   - Two sighting methods for boids because obstacles need more work; they're
     not point masses.

   - Don't forget that the world is positive and negative, with center in
     the exact middle of the world.  Obstacles are at the base, which is
     -25 (y) in the demo.  Obstacle calcs depend on this.

Final Notes on the FlockingWithTeeth App

What You Should See When You Run the Demo
=========================================

   As shipped the demo sets up 3 flocks with a total of 37 boids spread
across them.  They'll of three types in three different colors:

      Flock      Color           # Boids    Type        Hunted By
      =====      =====           =======    ====        =========
      Flock #0   yellow/orange   30 boids   Flies       Sparrows
      Flock #1   red              5 boids   Sparrows    Hawks
      Flock #2   blue             2 boids   Hawks       N/A    

    The boids start out randomized around the world, so the initial
conditions will be somewhat chaotic.  As they move around and encounter
each other, however, order will arise out of chaos and you'll see groupings
form.  Left running long enough all boids of the same color usually find
each other and cluster together, though interactions with enemy boids and
the "teleporter" effect of the world box boundaries can wreak havoc with
their cohesion for any long period of time.

    As boids get hungry their predator/prey functionality will kick in.  You
will see red lines extend from a given boid to its intended prey, and it will
put on a burst of speed to try to overcome and eat it.  Don't be surprised if
the red line shifts around a lot; there's no state info saved in flocking
between frames, and so each hunter is continually looking for closer and 
easier prey to eat.

   Any boid which goes long enough without eating will *die* and be removed
from the simulation.  Flies are the only exception to this, since they don't
hunt anything in the demo (though that is a possible enhancement, as 
discussed in the chapter).

   Flies can *reproduce* if two conditions are met:

      - There are enough flies together in one group to make a new Fly, and
      - There's at least one dead Fly in the database (previously eaten by
        a Sparrow) which can be reactivated.

   Reproduction isn't handled in a terribly complex manner, since all we're 
really doing is "bringing back" a Fly which had previously been killed.

Function Keys
=============

   There are some keys that let you control things while the demo is
running, mostly to allow you to change your viewpoint.  Your default
Camera position is located at (0, 0, 100) with your LookAt point being
(0, 0, 0); so, you're looking "sideways" into the world towards the 
origin.  You can change that somewhat as follows:

      Up Arrow     -- Moves the camera -2 along the Z axis (i.e, "into"
                      the screen).
      Down Arrow   -- Moves the camera +2 along the Z axis (i.e., "out of"
                      the screen).
      Left hArrow  -- Moves the camera -2 along the X axis (i.e., to the
                      left).
      Right  Arrow -- Moves the camera +2 along the X axis (i.e., to the
                      right).
      F1 Key       -- Moves the camera -2 along the Y axis (i.e., "down"
                      the screen).
      F2 Key       -- Moves the camera +2 along the Y axis (i.e., "up"
                      the screen).

      F3 Key       -- Moves the camera AIMPOINT +2 along the X axis.
      F4 Key       -- Moves the camera AIMPOINT -2 along the X axis.
      F5 Key       -- Moves the camera AIMPOINT +2 along the Y axis.
      F6 Key       -- Moves the camera AIMPOINT -2 along the Y axis.
      F7 Key       -- Moves the camera AIMPOINT +2 along the Z axis.
      F8 Key       -- Moves the camera AIMPOINT -2 along the Z axis.

      F9 Key        -- Turns on/off orientation axes for all boids and 
                       the world.  Note that the axes for the boids track
                       THEIR orientation in local (boid) space.
      F10 Key       -- Turns on/off a white wireframe sphere around each
                       boid that shows its perception distance (range
                       of sight).  Boids in this demo can only react to
                       what they see; this option lets you see how that
                       works.
      F11 Key       -- Turns on/off a red wireframe sphere around each
                       boid that shows its preferred keepaway distance.
                       This is how far away a boid tries to keep enemy
                       boids (members of another flock).
      F12 Key       -- Turns on/off a flock-colored wireframe sphere
                       around each boid that shows its preferred
                       separation distance.  This is how close a boid
                       tries to be from its flockmates.

      Escape Key   -- Quits the demo.


   Note that turning on the spheres puts a pretty heavy burden on the
rendering engine, so your updates could get a bit chunky depending on
your hardware.  I did NOTHING to try to optimize these things, since 
they're primarily intended for debug purposes.

Engineering Notes
=================

   Anybody who getes interested enough in this application to go look at
others out on the web will quickly discover that pretty much every flocking 
app is slightly different from every other.  For example, this app only 
considers the one closest enemy when trying to steer away from an enemy boid, 
while others consider all enemies and generate a combined vector away from 
the mass.  Still other apps (such as the Microsoft DirectX flocking demo) 
give weight to ALL flockmates and then scale their influence by distance, 
whereas this app uses a simple line of sight algorithm to only consider 
things the boid can see.  This app is different from many others in that
I do all my work in the world coordinate system; many others do local 
(boid-space) calculations and convert from one frame to the other.

   They're all right.  Whatever works for you is what's right; there are 
lots of ways to do build good flocking apps around the basic flocking 
algorithms.

   Note too that I wrote this for ease of understanding, not any particular
efficiency.  There are several methods that could be templatized with a bit
of thought, and of course one could easily create CFly, CSparrow, and CHawk
classes derived from the original CBoid class rathan than using types.  This
demo illustrates some of the nuances of flocking; it's NOT game ready code.

The Visibility List
===================

   Astute browsers of the code will note the VisibleFriendsList, a
fixed-size array that is sized differently than the maximum number of
boids (MaxBoids).  I did this primarily for the sake of speed and to
save memory.  Simplifying the algorithm to only look at the first 5 
visible flockmates (the current default for Max_Friends_Visible) saves
on memory, limits CPU usage on visibility and influence calculations, and
really doesn't make any difference to the flocking behavior.  

Debug Output
============

   It may not be apparent but the FlockingWithTeeth demo generates an output
file that will be placed on the Windows desktop (if you're running Windows,
of course).  The file is named "FWT.txt".  What's in it depends
on what debug options you're running.  The default (no debug) merely has
a final summary of the status of all flocks and boid members at the end
of a given run.  When you turn on other debug options (detailed below) the 
file can get QUITE large.

Debug Options
=============

  The following debug options are built into the code.  Compiling with
it will turn on the described debugging comments, which will be dumped
into the afore-mentioned "FWT.txt" file on the Windows desktop.
None of these are very fancy, but they can help illustrate some of the
inner workings of the flocking algorithm and my implementation of it.

   Here are the various options you can turn on and what they do:

      REPRO_DEBUG      -- Generates debug when a given boid tries to
                          reproduce and make a new boid.  Not much detail
                          and usually shows failure (due to there not being
                          enough Flies visible at that moment).

      FLOCK_DEBUG      -- Generates flock-level debug (how many members are
                          in a given flock, addition/deletion of members,
                          flock status).  Good high-level debuggin info.

      BOID_DEBUG       -- Very detailed debugging information regarding
                          each individual boid's decision making process is
                          generated with this option.  Changes to state vectors,
                          which flockmates and enemy boids are being considered,
                          etc.--it all gets dumped into the logging file.
                          DON'T use this option with a lot of boids as you
                          will quickly generate megabyte-size log files.

      VISIBILITY_DEBUG -- Potentially worse than BOID_DEBUG, this one dumps 
                          all of the visibility calculations each boid goes
                          through.  Note for the faint of heart.

   
Miscellaneous
=============

   As with any project of this nature the list of things I wanted to add
is longer than the list of things that actually made it into the code.  I
console myself with the knowledge that this app gets the concept across,
which was the main goal.  

   Feel free to email me at any time with questions, suggestions, etc.  If
you find the code useful please let me know; I like to know where my babies
end up.



Steven Woodcock
From the High Mountains of Colorado
ferretman@gameai.com

4/16/01
