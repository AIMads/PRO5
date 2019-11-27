#pragma once

#ifndef STATE_H
#define STATE_H


#include "Environment.h"



class State {
public:
	State();

	int pos = 0; //Initially at checkpoint 0.

	void initState(Environment* environment);
	int getUnvisited();
	ActionCost doAction(int action);
	void reset();
	bool* visited;
	int* timesvisited;

	~State();

private:
	Environment* env;
};


#endif // !STATE_H
