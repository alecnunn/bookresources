/*
 *  FILE       : main.c
 *
 *  PROGRAMMER : Paul Kelly 
 *               paul_kelly2000@yahoo.com
 *
 *  DESCRIPTION: This tool will parse the weapon data file and generate the
 *               binary data and the necessary enumerations for indexing into 
 *               the data in code.
 *               
 *               If you use this code or have any comments, please email me.
 *               I would love to hear from you!
 *
 *  NOTES      : For compiling and linking, be sure to have the lexer's 
 *               header file in the include path and all the necessary 
 *               libraries for the lexer and parser.
 */

/*
 *
 *  INCLUDES
 *
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "weapon.h"


/*
 *
 *  MACROS
 *
 */

#define FALSE 0
#define TRUE  1


/*
 *
 *  GLOBAL VARIABLES
 *
 */

extern FILE *yyin;
#ifdef _DEBUG
extern int yydebug;  // turns on parser debug info. #define YYDEBUG must 
#endif               // also be defined in the .y file.

// count of the different types of data
int weapon_cnt = 0;
int ammo_cnt = 0;

// weapon names
char weapon_tbl[ MAX_WEAPONS ][80];
char ammo_tbl[ MAX_AMMO_TYPES ][80];

WEAPON_DATA weapon[ MAX_WEAPONS ];
AMMO_DATA ammo[ MAX_AMMO_TYPES ];


/*
 *
 *  PROTOTYPES
 *
 */

extern int yyparse (void);


/*
 *
 *  FUNCTIONS
 *
 */

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: GetAmmoIdx
 *    Get the index of an ammo type based on its string and its location
 *    in the string table.
 *
 *
 * RETURN:
 *    The index of the ammo type within the ammo data.   Error if -1 returned.
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    char *ammoStr - ptr to ammo string to be found in the table of ammo 
 *                    strings.
 *                    
 *-----------------------------------------------------------------------------
 */
int GetAmmoIdx (char *ammoStr)
{
    int idx = 0;
    int found = FALSE;

    while (!found && idx < ammo_cnt)
    {
        if (strcmp (ammoStr, ammo_tbl[ idx ]) == 0)
        {
            found = TRUE;
            return idx;
        }
        else
        {
            idx++;
        }
    }
    
    return -1;
}

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: GenerateBinaryFile
 *
 *    File structure is as follows:
 *    int - weapon count
 *    int - ammo count
 *    WEAPON_BIN_DATA * weapon count - weapon data
 *    AMMO_DATA * ammo count - ammo data
 *
 *
 * RETURN:
 *    1 on success, 0 otherwise.
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */
int GenerateBinaryFile (char *binFilename)
{
    FILE *outFile;
    int  idx;
    WEAPON_BIN_DATA  weap;


    outFile = fopen (binFilename, "wb");

    if (outFile == NULL)
        return 0;  // error!  Something went wrong.

    // write out header info
    fwrite (&weapon_cnt, 1, sizeof (int), outFile);
    fwrite (&ammo_cnt, 1, sizeof (int), outFile);

    for (idx = 0; idx < weapon_cnt; idx++)
    {
        weap.ammo_clip = weapon[ idx ].ammo_clip;
        weap.fire_rate = weapon[ idx ].fire_rate;
        weap.ammo_type = GetAmmoIdx (weapon[ idx ].ammo_type);
        weap.pad =       (short) 0xFFFF;

        fwrite (&weap, 1, sizeof (WEAPON_BIN_DATA), outFile);
    }

    fwrite (&ammo, ammo_cnt, sizeof (AMMO_DATA), outFile);

    fclose (outFile);

    return 1; // success!
}

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: GenerateEnumerationFile
 *    Make a .h file that contains enumerations that can be used to index
 *    data in the code.
 *
 *
 * RETURN:
 *    1 on success, 0 otherwise.
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    char *enumFilename - filename where the enumeration will be placed.
 *
 *-----------------------------------------------------------------------------
 */
int GenerateEnumerationFile (char *enumFilename)
{
    FILE   *outFile;
    int    idx;
    time_t    tp;
    struct tm *local_time;

    outFile = fopen (enumFilename, "w");

    if (outFile == NULL)
        return 0;

    // get the time so that we can put it in the output of the file to show
    // when the file was made.
    time (&tp);
    local_time = localtime (&tp);


    // file header
    fprintf (outFile, "/*\n");
    fprintf (outFile, " * FILENAME       : %s\n", enumFilename);
    fprintf (outFile, " *\n");
    fprintf (outFile, " * DESCRIPTION    : Weapon and Ammo enumerations\n");
    fprintf (outFile, " *\n");
    fprintf (outFile, " * NOTES          : This file is automatically ");
    fprintf (outFile,                     "generated\n");
    fprintf (outFile, " *\n");
    fprintf (outFile, " * DATE GENERATED : %s", asctime (local_time));
    fprintf (outFile, " */\n\n");

    // create weapon enumeration
    fprintf (outFile, "// weapon enumeration\n");
    fprintf (outFile, "enum\n");
    fprintf (outFile, "{\n");

    for (idx = 0; idx < weapon_cnt; idx++)
    {
        fprintf (outFile, "    WPN_%s,\n", weapon_tbl[ idx ]);
    }

    fprintf (outFile, "};\n\n\n");

    // create ammo enumeration
    fprintf (outFile, "// ammo enumeration\n");
    fprintf (outFile, "enum\n");
    fprintf (outFile, "{\n");

    for (idx = 0; idx < ammo_cnt; idx++)
    {
        fprintf (outFile, "    AMMO_%s,\n", ammo_tbl[ idx ]);
    }

    fprintf (outFile, "};\n");

    fclose (outFile);

    return 1;  // success!
}

/*
 *-----------------------------------------------------------------------------
 *
 * FUNCTION: main
 *
 *    
 *
 *
 * RETURN:
 *    
 *    Returns the status of the program:
 *      0 = success
 *
 * OUTPUTS:
 *    none
 *
 * INPUTS:
 *    none
 *
 *-----------------------------------------------------------------------------
 */
int main (int argc, char **argv)
{
    FILE *pFileIn;                           // pointer to the input data file.
    char binFilename[20] = "weapon.bin";     // name of binary output file.
    char enumFilename[20] = "weapon_enum.h"; // name of enumeration output file

    #ifdef _DEBUG
    // turn on parser debug info.  #define YYDEBUG must be defined in the .y 
    // file also.
    yydebug = 1;
    #endif

    pFileIn = fopen ("weapon.dat", "r");

    yyin = pFileIn;

    if (yyparse() == 1)
        return 1;  // error in parse!!!

    // Parse was successful.  Now generate data file and constants file.
    if (GenerateBinaryFile (binFilename) == 0)
        return 1;  // error in file creation!!!
    
    // Now generate the necessary enumeration to index into the data.
    if (GenerateEnumerationFile (enumFilename) == 0)
        return 1;  // error in file creation!!!

    return 0;
}
