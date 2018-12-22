
class PathNode {
public:
	friend class NodeMap;
	
	
	enum NodeFlags{
		flagNodeGround    = 0x0001, //node is on the ground
		flagNodeHide     = 0x0002, //good hiding place

		flagNodeSnipe      = 0x0008, //good sniping place
		
		flagNodeCrouch    = 0x00010, //need to be in crouch state
		flagNodeLedgeHang = 0x00020, //need to hang on a ledge
		
		flagNodeStairs    = 0x00100, //node is on stairs
		flagNodeElevator	= 0x00200, //node is on an elevator
		flagNodeSwitch		= 0x00400, //node is next to a switch	
	};
	
	enum RegionType { Connected = 0, NoDoor, NoJump, NoElevator, MaxTypes };
	
	CPathNode() : flags(0), radius(1.5f), elevatorFloor(0), theta(0.0f), pathSession(0){
		for (int i=0; i < MaxTypes; i++) regions[i] = 0;
	}

	explicit PathNode(const Vector3 &p, int flag = 0) : pos(p), flags(flag), radius(1.5f), elevatorFloor(0), pathSession(0),
		prevLink(NULL), theta(0.0f), {
		for (int i=0; i < MaxTypes; i++) regions[i] = 0;
	}
	
	Vector3 pos;
	int flags;
	std::string name; //an optional name
	std::vector<PathLink> links;
	int EntityID; //the ID for an elevator or switch
	int	elevatorFloor; //if the node is on an elevator, which floor it is on
	float radius; //radius within which we can be considered "at this node"
	
	//These functions are not commutative, i.e. n1->PathExists(n2) does not necessarily imply n2->PathExists(n1)
	bool PathExists(const CPathNode *destination, const Character *pChar = NULL) const; //returns true if a path exists from this node to destination that can be travelled by pChar
	bool NoDoorsBetween(const CPathNode *destination) const; 
	bool NoElevatorsBetween(const CPathNode *destination) const; //returns true if destination can be reached from this node without using an elevator
	bool ReachableWithoutJumping(const CPathNode *destination) const; //returns true if destination can be reached from this node without jumping
	
	~CPathNode();
	const Elevator *GetElevator() const;
	
	//Get and Set the regions used by the region extraction algorithm
	unsigned char GetRegion(RegionType t) const {return regions[t];}
	void SetRegion(RegionType t, unsigned char r) {regions[t] = r;}
	
	float	TotalCost() const {return searchCost + futureCost;}
	void	ProcessForPath(PathNodePriorityQueue &pq, const CPathLink *parent) const;
protected:
	unsigned char regions[MaxTypes];
	
	//reserved for path search purposes
	mutable bool mOpen; //whether we're currently on the open list
	mutable int pathSession;
	mutable float searchCost; //the cost so far to reach this node
	mutable float futureCost;	//the estimated cost to the goal from here
	mutable const PathLink *prevLink;
};

class PathLink {
public:
	friend class NodeMap;
	
	enum {
		flagLinkReliable		= 0x000001, //this link has been verified as passable
			flagLinkCrouch			= 0x000002,	//we must actively crouch to follow this link
			flagLinkDoor			= 0x000004,	//there's a door on the link
			flagLinkJump			= 0x000008, //must jump to cross this link
			
			flagLinkNearCliff		= 0x000020, //danger of falling off a cliff if going outside fat link
			flagLinkRun				= 0x000040, //force the character to run on this link
			flagLinkSlow			= 0x000080, //force the character to sneak along this link
			
			flagLinkClimb			= 0x000200, //the AI needs to do an auto-climb somewhere on this link
			
			flagLinkSaveMask		= 0x00FFFF, //only save flags that mask with this
			flagLinkNoShortcut		= 0x010000, //the AI can not shortcut (rubberband) this link
			flagLinkElevator		= 0x020000,	//the link goes between elevator floors
	};
	
	int flags;
	
	
	PathLink(int flag = 0) : flags(flag), origin(NULL), next(NULL), dist(0), alwaysAvailable(true), reliability(1.0f){}
	PathLink(PathNode *start, PathNode *end, int flag = 0);
	
	bool CanBeShortcut() const;
	const PathNode *Start() const {return origin;}
	const PathNode *End() const {return next;}
	PathNode *EndNonConst() const {return next;}
	PathNode *StartNonConst() const {return origin;}
	const PathLink *GetReturnLink() const; //returns the link going in the opp. direction, NULL if no return link exists
	
	const Door* GetDoor() const;
	bool CanITravelLink(const CEntityCharacter *me);
	~PathLink(){}
	float Distance() const {return dist;}
	
protected:
	float dist;
	int	doorID;			//the ID of the door if one exists on this link
	CPathNode *origin; //the origin node of the link (do we need to keep this?)
	CPathNode *next; //the node where the link is headed
};
