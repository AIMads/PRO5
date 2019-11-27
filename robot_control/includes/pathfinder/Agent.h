#pragma once

#ifndef AGENT_H
#define AGENT_H

#include "State.h"
#include "Environment.h"


struct Action { int action; bool random; };


class Agent {
public:
	Agent(Environment *e, double epsilon=0.5, double alpha=0.04, double gamma=0.3);

	int *run(int maxepisodes = 1000000);

	
	float episodecost = 0;
	int moves = 0;
	int episodes = 0;
	int maxepochs = 1000;
	int epoch = 0;
	int epochBest = 0;
	int epochBestSteps = maxmoves;
	int bestCost = INT_MAX;

	~Agent();


private:
	State state;
	State state_;
	Environment *env;


	//Q-learning
	void initQTable();
	int bestAction(State *state);
	Action chooseAction();
	void step(bool printdebug=false);
	void updateQTable(int cost, int action, bool printdebug=false);

	double*** Qtable;	//Movenr x Pos x Action			or			Pos x timesvisited[pos] x action
	double epsilon;
	double alpha;
	double gamma;
	double epsdec = 0.000001;
	double mineps = 0.0005;


	//Helpers
	void endEpisode();
	void printStuff(Action action);
	void resetArray(int a[], int max);	

	int maxmoves = 90;
	int noImprovement = 0;
	int bestRoute[90] = { -1 };
	int route[90] = { -1 };
};







#endif AGENT_H