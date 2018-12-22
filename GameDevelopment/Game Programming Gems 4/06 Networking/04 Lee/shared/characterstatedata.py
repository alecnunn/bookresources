"""
characterstatedata.py - constants and lookup storage
"""

# LAYER IDs
POSTURE       = 1
MOVEMENT      = 2
ACTION        = 3

# STATE IDs BY LAYER

# POSTURE LAYER
STANDING      = 1   
SITTING       = 2
LAYINGDOWN    = 3
DEAD          = 4

# MOVEMENT LAYER
STOPPED       = 101
MOVEFORWARD   = 102
MOVEBACKWARD  = 103
MOVELEFT      = 104
MOVERIGHT     = 105

# ACTION LAYER
IDLE          = 201
CASTING       = 202
FIGHTING      = 203
GESTURING     = 204
CRAFTING      = 205

# STATE LOOKUP DICTIONARY - built by the RegisterState utility function
stateLookup = {}

