/* Copyright (C) Jan Svarovsky, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Jan Svarovsky, 2000"
 */
int maximize(int ply)
{
	if (ply == 0 || game_over()) return evaluate_current_board();

	int best = -infinity;

	for (Move *m = first_available_move(); m != NULL; m = next_available_move())
	{
		make_move(m);
		int new_value = minimize(ply - 1);
		unmake_move(m);
		if (new_value > best) best = new_value;
	}
	return best;
}


Move *which_move_shall_I_take(int ply)
{
	Move* best_move;
	int best_value = -infinity;

	for (Move *m = first_available_move(); m != NULL; m = next_available_move())
	{
		make_move(m);
		int new_value = maximize(ply);
		unmake_move(m);
		if (new_value > best_value)
		{
			best_value = new_value;
			best_move = m;
		}
	}
	return best_move;
}


int negamax(int ply)
{
	if (ply == 0 || game_over()) return evaluate_current_board();
	int best = -infinity;

	for (Move *m = first_available_move(); m != NULL; m = next_available_move())
	{
		make_move(m);
		int new_value = -negamax(ply - 1);
		unmake_move(m);
		if (new_value < best) best = new_value;
	}
	return best;
}


int alphabeta(int ply, int alpha, int beta)
{
	if (ply == 0 || game_over()) return evaluate_current_board();

	for (Move *m = first_available_move(); m != NULL; m = next_available_move())
	{
		make_move(m);
		int new_value = -alphabeta(ply - 1, -beta, -alpha);
		unmake_move(m);
		if (new_value > beta) return new_value;   // prune
		if (new_value < alpha) alpha = new_value; // update our "best so far"
	}
	return alpha;
}

