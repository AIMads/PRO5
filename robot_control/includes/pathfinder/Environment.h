#pragma once
#include <limits>

struct ActionCost {
	int cost = INT_MAX;
	bool newNode = false;
	int reward = 0;
};

struct Environment {
	int** distances;
	int numactions;
	int* actions;
	int newNodeReward = 10000;

	int illegal = 100;
	int maxlength = 120 * 80;


	void initEnv(int numa) { 
		numactions = numa; 
		actions = new int[numactions];
		for (int i = 0; i < numactions; i++) { actions[i] = i; } }
};