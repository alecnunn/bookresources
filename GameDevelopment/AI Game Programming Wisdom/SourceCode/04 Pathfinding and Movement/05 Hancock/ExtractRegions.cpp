

LArray2D<bool> NodeMap::RegionConnection; //region connections for all traversable links
LArray2D<bool> NodeMap::DoorRegionConnection; //region connections using non-door links
LArray2D<bool> NodeMap::JumpRegionConnection; //region connections using non-jump links
LArray2D<bool> NodeMap::ElevatorRegionConnection; //region connections using non-elevator links



void NodeMap::GenerateRegionConnectivity(const std::vector<const PathLink *> &oneWayLinks,
										LArray2D<bool>& RCMatrix, PathNode::RegionType rtype){
	RCMatrix.Fill(false);
	for (int i=0; i < RCMatrix.rows(); i++)
		RCMatrix(i, i) = true;

	for (int j =0; j < (int)oneWayLinks.size(); j++){ //for all one way links
		LList<int> regions;
		const PathLink *link = oneWayLinks[j];
		int start = link->Start()->GetRegion(rtype); //take the start region of the connected link
		int end = link->End()->GetRegion(rtype); //given a link that goes from start to end
		for (int k=0; k < RCMatrix.rows(); k++){ //for every region k
			if (RCMatrix(k, start))  //if k can reach start
				RCMatrix(k, end) = true; //then k can reach end
		}
	}
}

template <class OneWayPred, class ConnectedPred>
int GenerateRegions(std::vector<PathNode *>& nodeList, LVector<const PathLink *> &oneWayLinks,
						  OneWayPred& oneway, ConnectedPred& cpred, PathNode::RegionType rtype)
{

	std::vector<PathNode *>::iterator i;
	int connectedRegionCount = 0;
	
	for (i = nodeList.begin(); i != nodeList.end(); i++){
		PathNode *node = (*i);
		if (node->GetRegion(rtype) == 0){ //if we haven't given the node a region yet, put it on the list
			LList<PathNode *> connected;
			connected.push_back(node);
			int currentRegion = ++connectedRegionCount;
			node->SetRegion(rtype, currentRegion);
			while (!connected.empty()){
				PathNode *n = connected.front();
				LAssert(n->GetRegion(rtype) == currentRegion);
				connected.pop_front();
				for (int j = 0; j < (int)n->links.size(); j++){
					const PathLink &link = n->links[j];
					if (cpred(link)){ //if this link is a legal connection
						if (oneway(link)) //if it is a 1-way link 
							oneWayLinks.push_back(&link); //put it on the 1-way list
						else if(link.End()->GetRegion(rtype) == 0) { //else if this link end node hasn't been labelled
							link.EndNonConst()->SetRegion(rtype, currentRegion); //set the region of the link end node
							connected.push_back(link.EndNonConst()); //and add it to the connected list
						}
					}
				}
			}
		}
	}
	return connectedRegionCount;
}

struct AlwaysTrue{
	bool operator()(const PathLink&){return true;}
};

struct NoJump{
	bool operator()(const PathLink& link){ return !(link.flags & PathLink::flagLinkJump);}
};

struct NoDoor{
	bool operator()(const PathLink& link){return !(link.flags & PathLink::flagLinkDoor);}
};

struct NoElevator{
	bool operator()(const PathLink& link){return !(link.flags & PathLink::flagLinkElevator);}
};

struct NormalOneWay{
	bool operator()(const PathLink& link){return !link.GetReturnLink();}
};

struct JumpOneWay{
	bool operator()(const PathLink& link){
		const PathLink *retlink = link.GetReturnLink();
		return (!retlink || (retlink->flags & PathLink::flagLinkJump));
	}
};

struct DoorOneWay{
	bool operator()(const PathLink& link){
		const PathLink *retlink = link.GetReturnLink();
		return (!retlink || (retlink->flags & PathLink::flagLinkDoor));
	}
};

struct ElevatorOneWay{
	bool operator()(const PathLink& link){
		const PathLink *retlink = link.GetReturnLink();
		return (!retlink || (retlink->flags & PathLink::flagLinkElevator));
	}
};

void NodeMap::ExtractRegions(){

	std::vector<const PathLink *> oneWayLinks;

	//compute connected regions and connectivity matrix
	AlwaysTrue truefunc;
	NormalOneWay normal1way;
	int regions = GenerateRegions(nodeList, oneWayLinks, normal1way, truefunc, PathNode::Connected);
	RegionConnection = LArray2D<bool>(regions + 1, regions + 1);
	GenerateRegionConnectivity(oneWayLinks, RegionConnection, PathNode::Connected);


	//compute no-door regions and connectivity matrix
	oneWayLinks.clear();
	NoDoor nodoor;
	DoorOneWay door1way;
	int noDoorRegions = GenerateRegions(nodeList, oneWayLinks, door1way, nodoor, PathNode::NoDoor);
	DoorRegionConnection = LArray2D<bool>(noDoorRegions + 1, noDoorRegions + 1);
	GenerateRegionConnectivity(oneWayLinks, DoorRegionConnection, PathNode::NoDoor);
	

	//compute no-force-jump regions and connectivity matrix
	oneWayLinks.clear();
	NoJump nojump;
	JumpOneWay jump1way;
	int noJumpRegions = GenerateRegions(nodeList, oneWayLinks, jump1way, nojump,
		PathNode::NoJump);
	JumpRegionConnection = LArray2D<bool>(noJumpRegions + 1, noJumpRegions + 1);
	GenerateRegionConnectivity(oneWayLinks, JumpRegionConnection, PathNode::NoJump);

	//compute no-elevator regions and connectivity matrix
	oneWayLinks.clear();
	NoElevator noelevator;
	ElevatorOneWay elevator1way;
	int noElevatorRegions = GenerateRegions(nodeList, oneWayLinks, elevator1way, noelevator,
		PathNode::NoElevator);
	ElevatorRegionConnection = LArray2D<bool>(noElevatorRegions + 1, noElevatorRegions + 1);
	GenerateRegionConnectivity(oneWayLinks, ElevatorRegionConnection, PathNode::NoElevator);
}
