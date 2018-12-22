
Two sample applications are provided. Both use the same scheduler.

Files:

  Scheduler.h     scheduler public interface
  Scheduler.cpp   scheduler implementation
  Clock.h         clock interface (used by the scheduler)
  Clock.cpp       clock implementation
  ITask.h         task interface

  Sample.cpp -- sample console application, described below

  Win.cpp -- sample windows application, described below

For simplicity, both applications are bare-bones and use no MFC or other
boilerplate code. Both were written on Windows 2000 using MSVC 6.0, but
the scheduler files should be easy to port to other platforms and
compilers.

-----------------------------------------------------------------------
Sample

This is a console application which demonstrates event sequencing, with
both time and frame events. It uses a polling loop to call the scheduler
once every 33 ms, to simulate a real-time rendering cycle.

-----------------------------------------------------------------------
Win

This is a simple windows program which animates a set of colored balls.
It uses an offscreen buffer to achieve smoother rendering, and a
WM_TIMER message to activate the scheduler.

Each ball is animated independently by its own scheduler task, and the
animation updates are performed independently of render. When render
occurs, it renders a "snapshot" of the current positions of all the
balls, then copies the offscreen buffer to the window.

Balls can be added and removed dynamically. Clicking on a ball deletes
it, and pressing either '-' or 'd' deletes the last ball in the list.
Pressing 'a' or '+' adds a new ball. As balls are added and removed, an
animation task is also added or removed from the scheduler.

The windows application also demonstrates "pausing" the scheduler.
Pausing is simply telling it to stop and then restart later -- the
scheduler ignores elapsed time between Stop and Play commands. To stop
and reset the scheduler, use the Reset method. Pressing the spacebar
toggles the paused state.

 
