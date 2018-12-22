
Program:        A* Explorer
Author:         James Matthews
Version:        1.0 (1st September, 2001)

- This program was written as an example program for my chapter in
  AI Wisdom published by Charles River Media, "Basic A* Pathfinding Made Simple".
- Check Generation5 for latest executable and help files:
		http://www.generation5.org/

=================================
         F E A T U R E S
=================================

- Draw, load and save maps.
- View the A* tree and open/closed lists.
- Run or Step the A*.
- Breakpoints and 5 break conditions. Set a breakpoint by double-right clicking 
  on a point on the map. If that point is queried, the A* will break allowing 
  you to examine the tree as well as the open and closed lists.


=================================
  B U G S   A N D   Q U I R K S
=================================

- Quirk: If a node changes parent, the A* tree doesn't update
  to reflect this.
- Quirk: Do not place the goal node on a impassable (black)
  piece, since it will render the goal unreachable.
- Quirk: The A* tree doesn't update when "Continous Update"
  is selected. This is for efficiency reasons.


=================================
 F U T U R E   F E A T U R E S ?
=================================

- Hexagonal map support.
- CAStar should really support distance function overloading.
- Improve the nasty saving/loading code.

Any comments would be welcome at jmatthews@generation5.org

Enjoy!

- James