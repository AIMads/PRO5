#include "Agent.h"
#include <random>
#include <iostream>

using namespace std;



Agent::Agent( Environment *environment, double e, double a, double g) {
	epsilon = e;
	alpha = a;
	gamma = g;

	env = environment;


	state.initState(env);
	state_.initState(env);

	srand(2313);
}


int* Agent::run(int maxepisodes) {

	initQTable();

	while (epoch < maxepochs) {
		step(false);
		if (noImprovement >= 30)
			break;
	}

	return bestRoute;
}


void Agent::initQTable() {
	Qtable = new double**[env->numactions];
	for (int pos = 0; pos < env->numactions; pos++) {
		Qtable[pos] = new double*[env->numactions];
		for (int times = 0; times < env->numactions; times++) {
			Qtable[pos][times] = new double[env->numactions];
			for (int action = 0; action < env->numactions; action++)
			{
				//If the dist is INFINITE (ILLEGAL), then the corresponding qvalue must be -INFINITE

				if (env->distances[pos][action] == 0 || pos == action) {
					Qtable[pos][times][action] = INT_MIN;
				}
				else {
					Qtable[pos][times][action] = 0;
				}
			}
		}
	}
};


int Agent::bestAction(State *state) {		//No state needed??
	double value = INT_MIN;
	int bestmove = 0;

	for (int action = 0; action < env->numactions; action++) {

		if (Qtable[state->pos][state->timesvisited[state->pos]][action] > value) {
			value = Qtable[state->pos][state->timesvisited[state->pos]][action];
			bestmove = action;
		}
	}
	return bestmove;
}


Action Agent::chooseAction() {
	Action A;
	int randval = rand() % 100;
	if (randval > epsilon * 100)
	{
		A.action = bestAction(&state);
		A.random = false;
	}
	else
	{
		A.action = rand() % env->numactions;
		A.random = true;
		while (Qtable[state.pos][state.timesvisited[state.pos]][A.action] == INT_MIN)
		{
			A.action = rand() % env->numactions;
		}
	}
	return A;
}


void Agent::printStuff(Action action_) {
	cout << "Move " << moves << "  Position: " << state.pos << "  Random Action: " << action_.random <<
		"  Action: " << action_.action << "  Qvalue: " << Qtable[state.pos][state.timesvisited[state.pos]][action_.action]
		<< endl;
	system("pause");
}

void Agent::step(bool printdebug) {
	Action action_ = chooseAction();
	route[moves] = action_.action;

	ActionCost AC = state_.doAction(action_.action);

	if (printdebug)
		printStuff(action_);
		

	updateQTable(AC.reward , action_.action, printdebug);

	state.pos = state_.pos;
	state.timesvisited[action_.action] = state_.timesvisited[action_.action];
	if (AC.newNode)
	{
		state.visited[action_.action] = state_.visited[action_.action];
	}

	moves++;
	episodecost += AC.cost;
	
	if (state.getUnvisited() == 0 || moves >= maxmoves)
	{
		endEpisode();
	}
}

void Agent::endEpisode() {
	episodes++;

	int nodesVisited = env->numactions - state.getUnvisited();
	if (nodesVisited >= epochBest && moves <= epochBestSteps) {
		epochBest = nodesVisited;
		epochBestSteps = moves;
	}

	
	//Update best route
	if (episodecost < bestCost && state.getUnvisited() == 0) {
		for (int i = 0; i < 90; i++) {
			if (route[i] == -1)
				break;
			bestRoute[i] = route[i];
		}
		bestCost = episodecost;
		noImprovement = 0;
	}


	//Do stats on current epoch
	if (episodes % 10000 == 0) {
		epoch++;
		cout << "This epoch: " << epochBest << " nodes visited, in only " << epochBestSteps << " moves" << "    epsilon " << epsilon << endl;
		cout << "Shortest path so far:  " << bestCost << endl << endl;
		epochBest = 0;
		epochBestSteps = maxmoves;	
		noImprovement++;
	}


	//Prepare for next episode
	moves = 0;
	episodecost = 0;
	state.reset();
	state_.reset();
	resetArray(route, 90);
	if (epsilon > mineps)
		epsilon -= epsdec;
}

void Agent::resetArray(int a[], int max) {
	for (int i = 0; i < max; i++) {
		a[i] = -1;
	}
}

void Agent::updateQTable(int reward, int action, bool printdebug) {
	int bestaction = bestAction(&state_);
	//int maxQ = Qtable[moves+1][state_.pos][bestaction];		//Correct state???? moves +1???
	double maxQ = Qtable[state_.pos][state_.timesvisited[state_.pos]][bestaction];
	//double qval = Qtable[moves][state.pos][action];
	double qval = Qtable[state.pos][state.timesvisited[state.pos]][action];
	double qval_ = qval + alpha * (reward + gamma * maxQ - qval);
	if (printdebug)	
		cout << "Updating Qtable at position " << state.pos << "," << state.timesvisited[state.pos] << "," << action
		<< "   Reward: " << reward << "       new Q value: " << qval_ << endl << endl;

	//cout << "New qval_ " << qval_ << endl;
	//Qtable[moves][state.pos][action] = qval_;
	Qtable[state.pos][state.timesvisited[state.pos]][action] = qval_;
}



Agent::~Agent() {}