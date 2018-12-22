/*
 *  FILE       : weapon.h
 *
 *  PROGRAMMER : Paul Kelly 
 *               paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: Data structures for weapon and ammo data.
 *               
 *
 *  NOTES      : 
 */

/*
 *
 *  MACROS
 *
 */

#define MAX_WEAPONS     50      // max amount of weapons types to be defined
#define MAX_AMMO_TYPES  50      // max amount of ammo types to be defined

/*
 *
 *  TYPES
 *
 */

// intermediate data structure.  Parsed data is place in it before
// writing the data out in binary format.
typedef struct
{
    short   ammo_clip;
    short   fire_rate;
    char    ammo_type[80];     
} WEAPON_DATA;

// the binary data formats - this is the data that will be loaded
// into the game.
typedef struct
{
    short   ammo_clip;
    short   fire_rate;
    short   ammo_type;     // index into the ammo data
    short   pad;           // pad to be on 32-bit boundry
} WEAPON_BIN_DATA;

typedef struct
{
    int   damage;
} AMMO_DATA;


/*
 *
 *  GLOBAL VARIABLES
 *
 */
// the count of weapon and ammo types
extern int weapon_cnt;
extern int ammo_cnt;

// names of each data entry.  Will be used to generate enumeration.
extern char weapon_tbl[ MAX_WEAPONS ][80];
extern char ammo_tbl[ MAX_AMMO_TYPES ][80];

// data to be converted to a binary file.
extern WEAPON_DATA weapon[ MAX_WEAPONS ];
extern AMMO_DATA ammo[ MAX_AMMO_TYPES ];
