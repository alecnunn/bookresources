Team AI Demo
by John Reynolds

The demo accompanies the article "Team Behavior in an Action Game"
from the book "AI Game Programming Wisdom".  Please refer to the book 
for a full explanation of the implementation.

To run, copy the files into a folder on your hard-drive and change the 
level.map file attributes from 'read only' to writable, if you wish to 
save your maps.  Otherwise, the demo may be run from the CD.

When the program is first run there is a sample map which may be used.
This level shows a simple opportunity for the enemy to outflank the
player.

The map may be modified by selecting the buttons available to the 
right of the map.  The position of the player, the target (the players
goal), and the enemy can also be modified.  Up to eight enemy can be
placed.  To remove an enemy simply place a floor tile over their 
position.

Doors must be placed so that the rooms can be identified.  This is
done by pressing the Door button and clicking on the grid square where
the door exists.  Just leaving spaces between rooms will cause the
program to assume the two rooms are actually one.

The dialogue between the different levels of command is given in the
window at the bottom of the map.  The commands and information has
been made readable by translating them into english but the orders and
parameters are all represented.  The commanders internal decisions are
also shown using the purely uppercase word 'COMMANDER' preceeding the 
text.

The program may be paused at any time using the Play/Pause toggle
button.  This can be useful if you wish to watch the information and 
orders carefully.

The player is not controllable in this demo.  It merely serves as a
target for the bots and to show team-work being used.

Thanks
John

John Reynolds
Creative Asylum Ltd
www.creative-asylum.com