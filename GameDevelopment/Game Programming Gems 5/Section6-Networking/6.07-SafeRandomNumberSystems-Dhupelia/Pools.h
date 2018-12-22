// Macros to override standard C/C++ calls
//  If this header is included at a high enough level in the application,
//  every call to standard C rand calls will go to this class instead
#define rand() Pools::getRandomNumber(POOL_DEFAULT)
#define srand(x) Pools::seedPools(x)

// Easy macro to return a random number >=0 and <1
#define rand_float ((double)Pools::getRandomNumber(POOL_TYPE_DEFAULT) / 42949.0)


// POOL_TYPE enumeration
//  This defines the list of pools, and should be tailored to the
//  application-specific needs
enum POOL_TYPE
{
	POOL_DEFAULT		= 0,
	POOL_ENVIRONMENT	= 1,
	POOL_AI			= 2,
	POOL_COMMENTARY	= 3,
	POOL_MUSIC			= 4,
	POOL_MAX_TYPES // SHOULD NOT BE USED AS A POOL
};


// Pools class
//  This class defines the actual random pools, number generation and more
class Pools
{
public:
	// Get singleton instance
	static Pools* getInstance();

	// Constructor and Destructor
	Pools();
	~Pools();

	// Seed the random pools
	//  MUST BE DONE ONCE, AND ONLY ONCE
	static void seedPools(unsigned int seedValue);

	// GET the seed value for a given pool
	//  This is used to synchronize the pool values over the network
	static unsigned int getPoolSeed(POOL_TYPE whichPool);

	// SET the seed value for a given pool
	//  This is used to synchronize the pool values over the network
	static void setPoolSeed(POOL_TYPE whichPool, unsigned int newSeed);

	// Function to get a random number from a specific pool
	//  This is the workhorse, that is used for every random call
	static unsigned int getRandomNumber(POOL_TYPE whichPool);

private:
	// Singleton instance pointer
	static Pools *instance;

	// The random pools themselves
	static unsigned int randPools[POOL_MAX_TYPES];

	// Track whether the random pools are seeded more than once
	static bool seeded;
};
