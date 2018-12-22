/*
 * WinsockError.h
 *
 * Convert Winsock error codes to readable strings.
 *
 * Created 7/6/99 Andrew Kirmse (ARK)
 */

#ifndef _WINSOCKERROR_H
#define _WINSOCKERROR_H

// Return a string describing the last Winsock error code.
int   GetLastWinsockError();
char *GetLastWinsockErrorString();

#endif
