/*****************************************/
// Source code for Game Programming Gems 4, article "NPC Decision Making: Dealing with Randomness"
// Author: Kare'n Pivazyan
// File: DP.h
/*****************************************/



//	Map width and height.
const int WIDTH = 6;
const int HEIGHT = 4;

//	A helper constant.
const float LARGEST_FLOAT = 1E10;

/******************************************/
//	Structure to hold action outcome probabilities.
struct CAction {
	float m_probStraight;
	float m_probLeft;
	float m_probRight;
	float m_probBack;
};

/******************************************/
//	The map data along with various helper access methods.
class CMap {
private:
	bool m_walls[WIDTH][HEIGHT];	//	true if wall
	CAction m_actionProbs;
	float m_costs[WIDTH][HEIGHT];

public:
	CMap(const bool rMap[WIDTH][HEIGHT], const float rCosts[WIDTH][HEIGHT], CAction &rActionProbs);

	//	Wall checks.
	bool isWall(int row, int col) { return isOutside(row, col) || m_walls[row][col]; };
	bool isWallN(int row, int col) { return isOutside(row, col - 1) || m_walls[row][col - 1]; };
	bool isWallS(int row, int col) { return isOutside(row, col + 1) || m_walls[row][col + 1]; };
	bool isWallE(int row, int col) { return isOutside(row + 1, col) || m_walls[row + 1][col]; };
	bool isWallW(int row, int col) { return isOutside(row - 1, col) || m_walls[row - 1][col]; };

	//	Boundary check.
	bool isOutside(int row, int col);

	//	Data access methods.
	float getCost(int row, int col) { return m_costs[row][col]; };
	float getProbStraight(int row, int col) { return m_actionProbs.m_probStraight; };
	float getProbLeft(int row, int col) { return m_actionProbs.m_probLeft; };
	float getProbRight(int row, int col) { return m_actionProbs.m_probRight; };
	float getProbBack(int row, int col) { return m_actionProbs.m_probBack; };
};

/******************************************/
//	Value function class. I have included it to simplify the presentation of the algorithm.
//	In production code this class should be changed to value array, a member of CDynamicProgrammingAlg
class CValue {
private:
	float m_value[WIDTH][HEIGHT];
public:
	void setValue(int row, int col, float val) { m_value[row][col] = val; };

	//	Get the value. N = north, S = south, etc.
	float getValue(int row, int col) { return m_value[row][col]; };
	float getValueN(int row, int col) { return m_value[row][col - 1]; };
	float getValueS(int row, int col) { return m_value[row][col + 1]; };
	float getValueE(int row, int col) { return m_value[row + 1][col]; };
	float getValueW(int row, int col) { return m_value[row - 1][col]; };
};

/******************************************/
//	The algorithm class. Holds all the data and computes the value function.
class CDynamicProgrammingAlg {
private:
	//	The map.
	CMap m_map;
	//	The value function.
	CValue m_value;
	//	The best action array derived from the current value function.
	char m_bestAction[WIDTH][HEIGHT];
	//	The goal.
	int m_goalRow, m_goalCol;
	//	Helper functions that compute costs for each direction.
	float ComputeCostW(int row, int col);
	float ComputeCostE(int row, int col);
	float ComputeCostS(int row, int col);
	float ComputeCostN(int row, int col);
public:
	CDynamicProgrammingAlg(CMap &rMap, int goalRow, int goalCol);
	//	The heart of the DP algorithm.
	void Iterate();
	//	Print value function.
	void PrintValues();
	//	Print best actions.
	void PrintBestActions();
};