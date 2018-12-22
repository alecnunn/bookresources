/*****************************************/
// Source code for Game Programming Gems 4, article "NPC Decision Making: Dealing with Randomness"
// Author: Kare'n Pivazyan
// File: DP.cpp
/*****************************************/



#include"DP.h"
#include<stdlib.h>
#include<iostream>
#include<iomanip>

using std::cout;


float _min(float a, float b, float c) {
	return __min(__min(a, b), c);
}

/******************************************/
//	The heart of DP algorithm.
void CDynamicProgrammingAlg::Iterate() {
	float cost, cost_N, cost_S, cost_W, cost_E;

	for (int row = 0; row < WIDTH; row++) {
		for (int col = 0; col < HEIGHT; col++) {
			//	Temporary variables that hold costs for each direction.
			cost_N = LARGEST_FLOAT;
			cost_S = LARGEST_FLOAT;
			cost_W = LARGEST_FLOAT;
			cost_E = LARGEST_FLOAT;

			//	Compute the costs for all four directions.
			if (!m_map.isWallN(row, col))
				cost_N = ComputeCostN(row, col);
			if (!m_map.isWallS(row, col))
				cost_S = ComputeCostS(row, col);
			if (!m_map.isWallW(row, col))
				cost_W = ComputeCostW(row, col);
			if (!m_map.isWallE(row, col))
				cost_E = ComputeCostE(row, col);

			//	Choose best action and least cost.
			//	The cell is a goal
			if ((row == m_goalRow) && (col == m_goalCol)) {
				cost = 0;
				m_bestAction[row][col] = 'G';
			}
			//	The cell is a wall
			else if (m_map.isWall(row, col)) {
				cost = 0;
				m_bestAction[row][col] = char(219);
			}
			//	North action has smallest cost
			else if (cost_N <= _min(cost_S, cost_W, cost_E)) {
				cost = cost_N;
				m_bestAction[row][col] = char(24);
			}
			//	South action has smallest cost
			else if (cost_S <= _min(cost_N, cost_E, cost_W)) {
				cost = cost_S;
				m_bestAction[row][col] = char(25);
			}
			//	West action has smallest cost
			else if (cost_W <= _min(cost_N, cost_S, cost_E)) {
				cost = cost_W;
				m_bestAction[row][col] = char(27);
			}
			//	East action has smallest cost
			else {
				cost = cost_E;
				m_bestAction[row][col] = char(26);
			}

			//	Update value function
			m_value.setValue(row, col, cost + m_map.getCost(row, col));

		}
	}
}

//	Compute cost of moving north.
float CDynamicProgrammingAlg::ComputeCostN(int row, int col) {
	float prob_N = m_map.getProbStraight(row, col);
	float prob_E = m_map.getProbRight(row, col);
	float prob_W = m_map.getProbLeft(row, col);

	if (m_map.isWallE(row, col))
		prob_E = 0;

	if (m_map.isWallW(row, col))
		prob_W = 0;

	return prob_N * m_value.getValueN(row, col) +	prob_W * m_value.getValueW(row, col) + prob_E * m_value.getValueE(row, col) + (1 - prob_N - prob_W - prob_E) * m_value.getValue(row, col);
};

//	Compute cost of moving south.
float CDynamicProgrammingAlg::ComputeCostS(int row, int col) {
	float prob_S = m_map.getProbStraight(row, col);
	float prob_E = m_map.getProbLeft(row, col);
	float prob_W = m_map.getProbRight(row, col);

	if (m_map.isWallE(row, col))
		prob_E = 0;

	if (m_map.isWallW(row, col))
		prob_W = 0;

	return prob_S * m_value.getValueS(row, col) +	prob_W * m_value.getValueW(row, col) + prob_E * m_value.getValueE(row, col) + (1 - prob_S - prob_E - prob_W) * m_value.getValue(row, col);
};

//	Compute cost of moving west.
float CDynamicProgrammingAlg::ComputeCostW(int row, int col) {
	float prob_W = m_map.getProbStraight(row, col);
	float prob_S = m_map.getProbLeft(row, col);
	float prob_N = m_map.getProbRight(row, col);

	if (m_map.isWallS(row, col))
		prob_S = 0;

	if (m_map.isWallN(row, col))
		prob_N = 0;

	return prob_W * m_value.getValueW(row, col) + prob_N * m_value.getValueN(row, col) + prob_S * m_value.getValueS(row, col) + (1 - prob_W - prob_S - prob_N) * m_value.getValue(row, col);
};

//	Compute cost of moving east.
float CDynamicProgrammingAlg::ComputeCostE(int row, int col) {
	float prob_E = m_map.getProbStraight(row, col);
	float prob_S = m_map.getProbRight(row, col);
	float prob_N = m_map.getProbLeft(row, col);

	if (m_map.isWallS(row, col))
		prob_S = 0;

	if (m_map.isWallN(row, col))
		prob_N = 0;

	return prob_E * m_value.getValueE(row, col) + prob_N * m_value.getValueN(row, col) + prob_S * m_value.getValueS(row, col) + (1 - prob_E - prob_S - prob_N) * m_value.getValue(row, col);
};

//	Print value function.
void CDynamicProgrammingAlg::PrintValues() {
	cout << "------------------------------------\n";
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			cout << "|" << std::setw(4) << (int)  m_value.getValue(col, row) << " ";
		}
		cout << "|\n";
		cout << "------------------------------------";
		cout << "\n";
	}
};

//	Print best actions.
void CDynamicProgrammingAlg::PrintBestActions() {
	for (int row = 0; row < HEIGHT; row++) {
		for (int col = 0; col < WIDTH; col++) {
			cout << m_bestAction[col][row];
		}
		cout << "\n";
	}
};

//	Initializer.
CDynamicProgrammingAlg::CDynamicProgrammingAlg(CMap &rMap, int goalRow, int goalCol) : m_map(rMap) {
	m_goalRow = goalRow;
	m_goalCol = goalCol;
	for (int row = 0; row < WIDTH; row++) {
		for (int col = 0; col < HEIGHT; col++) {
			m_bestAction[row][col] = '_';
			m_value.setValue(row, col, m_map.getCost(row, col));
		}
	}
}

/******************************************/
//	Initializer
CMap::CMap(const bool rMap[WIDTH][HEIGHT], const float rCosts[WIDTH][HEIGHT], CAction &rActionProbs) {
	//	Init walls and costs
	for (int row = 0; row < WIDTH; row++) {
		for (int col = 0; col < HEIGHT; col++) {
			m_walls[row][col] = rMap[row][col];
			m_costs[row][col] = rCosts[row][col];
		}
	}
	//	Init action outcome probabilities
	m_actionProbs.m_probStraight = rActionProbs.m_probStraight;
	m_actionProbs.m_probLeft = rActionProbs.m_probLeft;
	m_actionProbs.m_probRight = rActionProbs.m_probRight;
	m_actionProbs.m_probBack = rActionProbs.m_probBack;
}

//	Boundary check.
bool CMap::isOutside(int row, int col) {
	return (row < 0) || (row >= WIDTH) || (col < 0) || (col >= HEIGHT);
}

/******************************************/
int main() {
	//	Setup the map
	bool walls[WIDTH][HEIGHT] = {	{false, false, false, false},
									{false, true, true, false},
									{false, true, true, false},
									{false, false, false, false},
									{false, false, false, false},
									{false, false, false, false} };

	//	Setup the cell costs. Walls have fixed cost of -1, goal has cost 0,
	//	regular cells have cost 1, the cell with the giant has cost 100.
	float costs[WIDTH][HEIGHT] = {	{1, 1, 1, 1},
									{1, -1 , -1, 1},
									{1, -1, -1, 1},
									{1, 1, 1, 1},
									{1, 1, 100, 1},
									{0, 1, 1, 1} };

	//	Probabilities of action outcomes. Here they are the same for all the cells.
	CAction actionProbs;
	actionProbs.m_probStraight = 0.5;
	actionProbs.m_probLeft = 0.25;
	actionProbs.m_probRight = 0.25;
	actionProbs.m_probBack = 0;

	//	Create the map.
	CMap map = CMap(walls, costs, actionProbs);

	//	Initialize the algorithm. (5, 0) is the location of the goal cell.
	CDynamicProgrammingAlg dp = CDynamicProgrammingAlg(map, 5, 0);

	//	Iterate 15 times. In more sophisticated domains, the number of iterations would
	//	depend on CPU time available and on the estimate of how close is the algorithm
	//	to a good solution.
	for (int i = 0; i < 15; i++) {
		cout << "\nIteration " << i << "\n";
		cout << "Value function:\n";
		dp.PrintValues();
		cout << "\nBest path found so far:\n";
		dp.PrintBestActions();
		cout << "\n";
		dp.Iterate();
	};

	cout << "After you have examined the program output,\ninput any letter followed by carrigae return to quit.\n";
	char wait_for_key_press;
	std::cin >> wait_for_key_press;
	return 0;
}