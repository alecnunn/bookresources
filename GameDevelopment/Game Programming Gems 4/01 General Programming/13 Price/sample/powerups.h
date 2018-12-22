struct PowerUp_t {
	char szName[10];		// display name
	char szImage[16];		// image file name

	// health increase/decrease (-128 to 127)
	signed char   iHealth;

	// temporary abilities/penalties
	// (value is duration in seconds)
	unsigned char iInvulnerability;
	unsigned char iFastMove;
	unsigned char iHighJump;
	unsigned char iStunPlayer;

	// extra life (count)
	unsigned char iLifeUp;
};

// global power-up definition cache
extern struct PowerUp_t *g_PowerUps;
