#include <stdio.h>
#include <string.h>
#include "XDSliteAPI.h"

#include "powerups.h"
#define DEFINE_DSD
#include "powerups_dsd.h"

int g_iPowerUpCount = 0;
//unsigned short iElemType;
struct PowerUp_t *g_PowerUps;


void XDS_PROCESSNODE(unsigned short  nodeType,
					 void           *nodeData,
					 unsigned long   nodeSize)
{
	printf("PROCESSNODE: 0x%04x=", nodeType);
	switch(nodeType)
	{
	case XDS_Powerups_Powerup:
		printf("RECORD XDS_Powerups_Powerup\n");
		// record start -- do nothing
		break;

	case XDS_Powerups_g_PowerUps:
		printf("ELEMENT XDS_Powerups_g_Powerups\n");
		// powerup data -- save it
		g_PowerUps = (struct PowerUp_t *)nodeData;
		g_iPowerUpCount = nodeSize / sizeof(struct PowerUp_t);
		break;

	case XDS_RECORD_COMMENT:
		printf("COMMENT '%s'\n", nodeData);
		break;

	case XDS_RECORD_ENDOFDATA:
		printf("END-OF-DATA\n");
		break;

	default:
		printf("<unknown>\n");
		break;
	}
}

#ifdef XDS_SUPPORT_DEFTYPE
void regDsd(struct xdsHandle *hXds)
{
	// Register my types (test only)

	xdsDefRecord(hXds, "Powerup", 4);

	unsigned short iStructType = xdsDefStructType(hXds, "PowerUp_t");
	xdsDefStructField(hXds, iStructType, "szName", XDS_TYPE_CHAR, 10);
	xdsDefStructField(hXds, iStructType, "szImage", XDS_TYPE_CHAR, 16);
	xdsDefStructField(hXds, iStructType, "iHealth", XDS_TYPE_CHAR, 0);
	xdsDefStructField(hXds, iStructType, "iInvulnerability", XDS_TYPE_BYTE, 0);
	xdsDefStructField(hXds, iStructType, "iFastMove", XDS_TYPE_BYTE, 0);
	xdsDefStructField(hXds, iStructType, "iHighJump", XDS_TYPE_BYTE, 0);
	xdsDefStructField(hXds, iStructType, "iStunPlayer", XDS_TYPE_BYTE, 0);
	xdsDefStructField(hXds, iStructType, "iLifeUp", XDS_TYPE_BYTE, 0);

	xdsDefStructDone(hXds, iStructType);
	unsigned short iArrayType = xdsDefArrayType(hXds, "_xdsType1", iStructType, 0, 2);

	xdsDefElement(hXds, "g_PowerUps", iArrayType, 0);
}
#endif

void printIf(int iValue, char *szFormat)
{
	if(iValue)
		printf(szFormat, iValue);
}
void printPowerups(void)
{
	printf("%d Power-ups:\n", g_iPowerUpCount);
	for(int ii = 0; ii < g_iPowerUpCount; ++ii)
	{
		printf("%10s  ", g_PowerUps[ii].szName);
		printf("%16s  ", g_PowerUps[ii].szImage);
//		if(g_PowerUps[ii].fHealth != 0.0)
//            printf("Health[%f] ", g_PowerUps[ii].fHealth);
		printIf(g_PowerUps[ii].iHealth, "Health[%d] ");
		printIf(g_PowerUps[ii].iInvulnerability, "Invulnerability[%d] ");
		printIf(g_PowerUps[ii].iFastMove, "FastMove[%d] ");
		printIf(g_PowerUps[ii].iHighJump, "HighJump[%d] ");
		printIf(g_PowerUps[ii].iStunPlayer, "Stun[%d] ");
		printIf(g_PowerUps[ii].iLifeUp, "LifeUp[%d] ");
		printf("\n");
	}
}

void readPowerups(char *szFile)
{
	FILE *fp = fopen(szFile, "rb");
	if(!fp)
	{
		fprintf(stderr, "Cannot open '%s' - %s\n", szFile, strerror(errno));
		exit(1);
	}
	printf("Reading powerups from '%s'\n", szFile);

#ifdef XDS_SUPPORT_DEFTYPE
	struct xdsHandle *h = xdsInit(fp, "Powerups", NULL);
	regDsd(h);
#else
	struct xdsHandle *h = xdsInit(fp, "Powerups", &XDSDSD_Powerups[0]);
#endif

	while(xdsReadRecord(h))
		;
	xdsFini(h);
	fclose(fp);
}

void writePowerups(char *szFile)
{

	FILE *fp = fopen(szFile, "wb");
	if(!fp)
	{
		fprintf(stderr, "Cannot open '%s' - %s\n", szFile, strerror(errno));
		exit(1);
	}
	printf("Writing powerups to '%s'\n", szFile);
	struct xdsHandle *h = xdsInit(fp, "Powerups", &XDSDSD_Powerups[0], XDS_IOMODE_WRITE);
	xdsStartRecord(h, XDS_Powerups_Powerup);
		xdsWriteNode(h, XDS_Powerups_g_PowerUps, g_PowerUps, g_iPowerUpCount);
	xdsEndRecord(h);
	xdsFini(h);
	fclose(fp);

}

int main(int argc, char **argv)
{
	readPowerups("powerups.xml");
	printPowerups();

#ifdef XDS_SUPPORT_WRITING
	writePowerups("powerups.xds");

	XDS_FREE(g_PowerUps);
	g_iPowerUpCount = 0;

	readPowerups("powerups.xds");
	printPowerups();
#endif
}