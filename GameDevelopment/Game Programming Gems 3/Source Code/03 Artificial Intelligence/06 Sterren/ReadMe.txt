/* Copyright (C) William van der Sterren, 2002. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) William van der Sterren, 2002"
 */

/* Copyright (C) James Matthews, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) James Matthews, 2001"
 */

Program:        A* Explorer
Author:         James Matthews / William van der Sterren
Version:          1.1 (2nd October, 2001)
                + tactical extensions (27th January 2002)

- The original program was written as an example program for James Matthews chapter in
  AI Wisdom published by Charles River Media, "Basic A* Pathfinding Made Simple".
- The program was extended for William van der Sterren's chapter in the Game Programming 
  Gems III book by Charles River Media, "Tactical Path-finding with A*"

- Check Generation5 or CGF-AI for latest executable and help files:
		http://www.generation5.org/
    http://www.cgf-ai.com/

=================================
         F E A T U R E S
=================================

- Draw, load and save maps.
- View the A* tree and open/closed lists.
- Run or Step the A*.
- Breakpoints and 5 break conditions. Set a breakpoint by double-right clicking 
  on a point on the map. If that point is queried, the A* will break allowing 
  you to examine the tree as well as the open and closed lists.
- Dynamically view the A* search space.
- Place threats and see their lines-of-fire
- Experiment with tactical paths via a large number of cost-functions that
  take threats and lines-of-fire into account

=================================
  B U G S   A N D   Q U I R K S
=================================

- Quirk: If a node changes parent, the A* tree doesn't update
  to reflect this.
- Quirk: Do not place the goal node on a impassable (black)
  piece, since it will render the goal unreachable.
- Quirk: The A* tree doesn't update when "Continous Update"
  is selected. This is for efficiency reasons.

- Bug  : line-of-fire/line-of-sight isn't 100% symmetric,
         the approximated LOF lookup table does not always
         see all positions being covered by a threat as
         under fire

=================================
 F U T U R E   F E A T U R E S ?
=================================

- Hexagonal map support.
- CAStar should really support distance function overloading.
- Improve the nasty saving/loading code.
- Improve the UI (single dialog to dial A* search settings, rather
  than numerous menu clinks)

Any comments would be welcome at jmatthews@generation5.org
and william@cgf-ai.com

Enjoy!

- James / William