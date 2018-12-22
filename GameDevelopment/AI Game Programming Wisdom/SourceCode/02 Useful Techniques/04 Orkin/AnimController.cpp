#include "AnimController.h"

// --------------------------------------------------------------------------

bool CAnimController::PlayAnim(AnimFileStruct* pRequestedAnim)
{
	bool bPlayAnim = true;

	ANIM_INSTANCE_MAP::iterator it;
	for(it = m_mapAnimInstances.begin(); it != m_mapAnimInstances.end(); ++it) 
	{
		EnumAnimCategory ePlayingCategory	= (EnumAnimCategory)it->first;
		AnimFileStruct*	 pPlayingAnim		= it->second.pAnimFileStruct;

		// Check if animation is already playing.
		if(pRequestedAnim == pPlayingAnim) 
		{
			bPlayAnim = false;
			break;
		}

		// Check if categories clash.
		else if(ePlayingCategory & pRequestedAnim->eCategory) 
		{	
			unsigned long nPlayingPriority = pPlayingAnim->nPriority;
			
			// If an animation of the same or lower priority, and the 
			// exact same category is playing, stop the currently playing
			// animation.  The requested animation will replace it.
			if(ePlayingCategory == pRequestedAnim->eCategory) 
			{
				if(nPlayingPriority > pRequestedAnim->nPriority)
				{
					bPlayAnim = false;
					break;
				}
				else {
					m_mapAnimInstances.erase(it);
				}
				break;
			}

			// If requested animation has a higher priority, and encompasses the currently
			// playing animation, stop the currently playing animation.
			// For example, UpperBody encompasses LeftArm.
			if( ((unsigned long)ePlayingCategory < (unsigned long)pRequestedAnim->eCategory)
				&& (nPlayingPriority < pRequestedAnim->nPriority) ) 
			{
				m_mapAnimInstances.erase(it);
				break;
			}
		}
	}

	// No conflicts were found, so play the requested animation.
	if(bPlayAnim)
	{
		m_mapAnimInstances.insert( ANIM_INSTANCE_MAP::value_type( (unsigned long)(pRequestedAnim->eCategory), AnimInstanceStruct(pRequestedAnim) ) );
	}

	return bPlayAnim;
}

// --------------------------------------------------------------------------

void CAnimController::Print()
{
	printf("\nCurrently Playing:\n");

	ANIM_INSTANCE_MAP::iterator it;
	for(it = m_mapAnimInstances.begin(); it != m_mapAnimInstances.end(); ++it)
	{
		printf("  %s\n", it->second.pAnimFileStruct->szFileName);
	}
}
