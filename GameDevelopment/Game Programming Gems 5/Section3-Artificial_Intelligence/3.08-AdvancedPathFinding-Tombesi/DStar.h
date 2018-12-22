/**********************************************************************

	DEVELOPED BY:		MARCO TOMBESI
	CONTACT:		e-mail:		baggior@libero.it		
					web-page:	digilander.iol.it/baggior

 *>	Copyright (c) Marco Tombesi, 2003
 **********************************************************************/
#ifndef _DSTAR_
#define _DSTAR_

struct LGraph_TNode;
struct LHash;
struct LEdgeInfo;
struct LGraph;
struct LArray;
struct LNodeInfo;
struct LEdgeMap;
struct LGraph_TEdge;

typedef unsigned long int       ui4;

#define NEW			 0
#define OPEN		10
#define CLOSED		20
#define IS_LOWER	30
#define IS_RAISE	40



struct point
{	
	unsigned long x;
	unsigned long y;
};

struct node_info
{	
	point pos;				 // (row,col) in the map
	point bptr;				 // BackPointer of the node
	unsigned long h;		 // estimated distance from goal
	unsigned int  tag;       // new/open/closed 
	unsigned int  status;	 // raise/lower NOTE: it is valid only for open states
	unsigned long iterations;// number of iteration on the node
};

class DStar
{
public:
	DStar(unsigned long n, point start, point goal);
	//n is the edge of the map
	//start is the coord (row, col) of start point
	//goal is the coord (row, col) of goal point
	
	~DStar(void);
	//destructor
	
	void SetEdge(point p0, point p1, unsigned long value);
	//sets the weight of all adjacent edges of p to value
	
	node_info GetNodeInfo(point p);
	//return the the weight of all adjacent edges of p 
	
	point* Play(long n, long &len);
	//computes n steps of the path and returns them
	//if point == NULL, there's no path
	//after the method's call len is the length of the path
	//use n = -1 to compute the entire search

	inline unsigned long GetCost()
	{return PathCost;}
	//gives the cost of the path so far

	point CurrentLocation();
	//return the (row,col) in which the agent is

	point GetStart();
	//returns the start of the map

	point GetGoal();
	//returns the goal of the map

	unsigned long GetEdgeCost(point p0, point p1);
	//returns the cost of the edge between p0 and p1

	inline point* GetProcessedNodes()
	{return ProcessedNodes;}
	//returns the processed node since last path processing
	//NOTE: Only the NodesProcessedSinceLast()-1 indicies of the array are valid

	inline bool Finished()
	{return finished;}
	//when this method is true, the agent is on the goal
	//or the path does not exist

	inline unsigned long TotalProcessedNodes()
	{return total_processed_nodes;}
	//returns total processed nodes
	
	inline unsigned long NodesProcessedSinceLast()
	{return nodes_processed_since_last;}
	//returns the number of nodes processed since last replanning

	inline unsigned long GetBase()
	{return base;}
	//returns the edge of the map

private:
	struct Slot 
	{
		ui4			  value;
		LGraph_TNode* state;
	};

	LGraph_TNode* goal;
	LGraph_TNode* start;
	LGraph_TNode* current;
	LGraph_TNode* minState;
	LGraph*       Map;
	LArray*       NodesList;
	LEdgeInfo*    Cost;
	LNodeInfo*    Tag;
	LNodeInfo*    BackPtr;
	LNodeInfo*    H;
	LNodeInfo*    NumberIterations;
	LHash*		  KIndices;
	Slot*		  K;
	LEdgeMap*	  EdgeMap;
	point*        ProcessedNodes;
	ui4			  OpenListCount;
	ui4			  PathCost;
	ui4           base;
	bool		  finished;
	ui4			  total_processed_nodes;
	ui4           nodes_processed_since_last;
    
	ui4             K_Insert(ui4, LGraph_TNode*);
	LGraph_TNode*   K_GetState();
	ui4				K_GetMin();
	void			K_Init();
	LGraph_TNode*	MinState();
	void			Insert(LGraph_TNode*, ui4);
	void			Delete(LGraph_TNode*);
	ui4				GetKMin();
	ui4				ProcessState();
	ui4				ModifyCost(LGraph_TNode*, LGraph_TNode*, ui4);
};

#endif