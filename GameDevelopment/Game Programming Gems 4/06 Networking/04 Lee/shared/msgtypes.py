"""
msgtypes.py - shared msg ids between client
              and server
"""

# client to server message types
MSG_TYPE_LOGIN                = 1
MSG_TYPE_STATE_CHANGE_REQUEST = 2

# server -> client message types
MSG_TYPE_LOGIN_ACK          = 100
MSG_TYPE_NEW_ACTOR          = 101
MSG_TYPE_ACTOR_LEFT         = 102
MSG_TYPE_STATE_CORRECTION   = 103
MSG_TYPE_STATE_CHANGE       = 104
MSG_TYPE_ACTOR_FROZEN       = 105
MSG_TYPE_ACTOR_DEAD         = 106
MSG_TYPE_SHUTDOWN           = 255

# msgtype size in bytes
MSG_HDR_SIZE = 1