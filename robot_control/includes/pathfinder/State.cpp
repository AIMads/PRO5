#include "State.h"
#include <limits>
#include <iostream>

using namespace std;


State::State( ) {}

void State::initState(Environment* e)
{
	env = e;
	visited = new bool[env->numactions];
	visited[0] = true; //Starting at 0, thus always visited
	for (int i = 1; i < env->numactions; i++) {
		visited[i] = false;
	}

	timesvisited = new int[env->numactions];
	timesvisited[0] = 1;
	for (int i = 1; i < env->numactions; i++) {
		timesvisited[i] = 0;
	}
}

int State::getUnvisited() {
	int unvisited = 0;
	for (int i = 0; i < env->numactions; i++) {
		if (!visited[i])
			unvisited++;
	}
	return unvisited;
}


ActionCost State::doAction(int action) {
	ActionCost AC;
	AC.cost = env->distances[pos][action];
	//cout << "Cost  " << AC.cost << endl;
	if (AC.cost != INT_MAX)
	{
		if (!visited[action])
		{
			AC.newNode = true;
			visited[action] = true;
		}
		pos = action;
		//cout << "New position!!" << endl;
	}
	else
		cout << "This should never happen!!!";

	timesvisited[action]++;

	if (AC.newNode)
		AC.reward = env->newNodeReward;
	else if (AC.cost == 0 || AC.cost == INT_MAX)
	{
		AC.reward = -env->illegal;
		cout << "nonononono";
	}
	else
		AC.reward = -AC.cost / env->maxlength;


	return AC;
}


void State::reset()
{
	pos = 0;
	for (int i = 1; i < env->numactions; i++)
	{
		visited[i] = false;
	}

	timesvisited[0] = 1;
	for (int i = 1; i < env->numactions; i++)
	{
		timesvisited[i] = 0;
	}
}



State::~State() {}