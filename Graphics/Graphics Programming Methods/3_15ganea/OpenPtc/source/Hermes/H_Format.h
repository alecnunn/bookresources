#ifndef __HERMES_FORMAT__
#define __HERMES_FORMAT__


#include "H_Types.h"
#include "H_Config.h"


typedef struct {
  hermes_int32 r,g,b,a;            /* Bitmasks for red, green, blue, alpha */
  int bits;                        /* Bits per pixel */
  char indexed;                    /* 1 if indexed colour format */
} HermesFormat;


/* Create a new empty format structure */
HERMES_C HermesFormat* HERMES_API Hermes_FormatNewEmpty();


/* Create a new format structure, returns 0 if failed.
*/
HERMES_C HermesFormat* HERMES_API Hermes_FormatNew(int bits,hermes_int32 r,hermes_int32 g,hermes_int32 b,
			 		  hermes_int32 a,char indexed);


/* Compare two formats. Return 1 if they are equal, 0 otherwise */
HERMES_C char HERMES_API Hermes_FormatEquals(HermesFormat *op1,HermesFormat *op2);

/* Copy the contents of format 'source' to format 'destination' */
HERMES_C void HERMES_API Hermes_FormatCopy(HermesFormat *source,HermesFormat *dest);


#endif

