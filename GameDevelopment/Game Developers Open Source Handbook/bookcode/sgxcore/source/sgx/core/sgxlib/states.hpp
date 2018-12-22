#ifndef SGX_CORE_SGXLIB_STATES_HPP
#define SGX_CORE_SGXLIB_STATES_HPP

//
// A generic template to handle state changes of an arbitrary type
// Note: type must support operator!= and assignment. Must also be
// able to construct a default state.
//
template<typename STATE_TYPE>
class sgxStateTemplate {
public:
	tBOOL		bDirty;
	tBOOL		bUndefined;
	STATE_TYPE	state;

	sgxStateTemplate<STATE_TYPE>()
	{ 
		bDirty = FALSE; 
		bUndefined = TRUE; 
	}
	sgxStateTemplate<STATE_TYPE>(const STATE_TYPE &intial)  
	{ 
		bDirty = TRUE; 
		bUndefined = FALSE; 
		state = intial; 
	}

	SGX_INLINE tBOOL	  IsDirty()	const				{ return bDirty; }
	SGX_INLINE tBOOL	  IsUndefined()	const			{ return bUndefined; }
	SGX_INLINE void		  SetState(STATE_TYPE new_state)
	{ 
		if (bUndefined) 
		{ 
			state = new_state; 
			bDirty = TRUE; 
			bUndefined = FALSE; 
		}
		else if (state != new_state) 
		{ 
			state = new_state; 
			bDirty = TRUE; 
		} 
	}
	SGX_INLINE STATE_TYPE GetState() const
	{ 
		return state; 
	}
	SGX_INLINE tBOOL	Apply()		
	{ 
		if (bDirty) 
		{ 
			DoApply(); 
			bDirty = FALSE; 
			return TRUE; 
		} 
		return FALSE; 
	}
	SGX_INLINE tBOOL	Apply(STATE_TYPE new_state)		
	{ 
		SetState(new_state); 
		return Apply(); 
	}
	SGX_INLINE void		Reapply()	{ Force(); Apply(); }
	SGX_INLINE void		ForceClean(){ bDirty = FALSE; }
	SGX_INLINE void		ForceDirty(){ bDirty = TRUE; }

	virtual    tBOOL	DoApply()	{ return bDirty; }
};

//
// Basic types
//
typedef sgxStateTemplate<tUINT32>		sgxState;
typedef sgxStateTemplate<tBOOL>			sgxStateBool;
typedef sgxStateTemplate<void *>		sgxStatePtr;
typedef sgxStateTemplate<tREAL32>		sgxStateReal32;
typedef sgxStateTemplate<tREAL64>		sgxStateReal64;

#endif	/* SGX_STATES_HPP */

