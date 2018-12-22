#include "dog.h"

IE_START(Dog)

    GOAL (Nap)

#if 0
        IF (HearsNoise)	    GOTO (Investigate)

    GOAL (Investigate)
        IF (SeesHuman) 	    GOTO  (ConsiderAction)
        IF (Done) 	        GOTO  (Nap)

    GOAL (ConsiderAction)
        IF (LikesHuman)	    GOTO  (Follow)
        IF (WaryOfHuman)	GOTO  (RunAway)
        IF (HateHuman)	    GOTO  (Attack)

    GOAL (Follow)
        IF (Close) 	        GOSUB (Beg)
        IF (Tired)      	GOSUB (Rest
        IF (HumanGone) 	    GOTO  (GoHome)

    GOAL (Rest)
        IF (Done) 	        RETURN

    GOAL (Beg)
        IF (GetFood) 	    GOSUB (Eat)

    GOAL (Eat)

    GOAL (GoHome)
#endif

IE_END
