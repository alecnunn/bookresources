#ifndef _TYPES_H
#define _TYPES_H

// basic type definitions
typedef unsigned int	u32;
typedef unsigned short	u16;
typedef unsigned char	u8;
typedef signed int		s32;
typedef signed short	s16;
typedef signed char		s8;

// useful macros
#define countof(A)		(sizeof(A)/sizeof((A)[0]))

#define	FREE(A)			if((A))	{ free((A));		(A)=0;	}
#define RELEASE(A)		if((A))	{ (A)->Release();	(A)=0;	}


// useful inline functions
inline float Lerp( float a, float b, float f )
{
	return a + (b-a)*f;
}

inline float Bilinear( const float aValues[4], const float aFrac[2] )
{
	float	aTemp[2];
	aTemp[0] = Lerp(aValues[0],aValues[1],aFrac[0]);
	aTemp[1] = Lerp(aValues[2],aValues[3],aFrac[0]);
	return Lerp(aTemp[0],aTemp[1],aFrac[1]);
}

template <class X>
X *IncPointer( X *ptr, u32 amount )
{
	return (X*)(((u8*)ptr)+amount);
}

#endif
