# gameevents.py
#
# Contains a set of constants that identify game events.
#
# Author: Matthew Walker
#         mwalker@softhome.net
#

# Event Name           ID    Signature
ACTION_ATTACK_SUCCESS  = 1   # src: attacker, subj: defender, args: (damage,)
ACTION_ATTACK_FAILURE  = 2   # src: attacker, subj: defender, args: ()
ACTION_PLACE_OBJECT    = 3   # src: actor, subj: object, args: (destination,)
ACTION_TAKE_OBJECT     = 4   # src: actor, subj: object, args: (sourceloc,)
ACTION_WAVE            = 5   # src: actor, subj: target, args: ()
PLAYER_ENTERS_ROOM     = 6   # src: room, subj: player, args: ()
PLAYER_EXITS_ROOM      = 7   # src: room, subj: player, args: ()
PLAYER_DIES            = 8   # src: player, subj: None, args: ()


# lookup table by integer id
byvalue = {}

# init byvalue from the members of this module
for attr, val in locals().items():
  if attr[0] != '_' and attr not in ['byvalue',]:
    # avoid reserved attributes and the byvalue dict
    byvalue[val] = attr


