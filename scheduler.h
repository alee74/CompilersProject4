/* * * * * * * * * * * * * * * * *
 *                               *
 * scheduler.h                   *
 *                               *
 * Compilers Project 4           *
 *                               *
 * Written by: Austin James Lee  *
 *                               *
 * * * * * * * * * * * * * * * * */

#pragma once

#include "parser.h"
#include <vector>
#include <queue>
#include <algorithm> // for sort, in printing

using std::vector;
using std::queue;
using std::sort;


/// Scheduler Class ///

class Scheduler {
	// Node struct for depency graph
	struct Node {
		Node(Instruction in);
		Instruction i;
		int weight;
		vector<Node*> parents;
		vector<Node*> children;
	};
	public:
		Scheduler(string infile, bool = false);
		~Scheduler();
		list<Instruction> intRep;
		vector<Node*> nodes;
	private:
		void buildDepGraph();
		void computeWeights();
		void assignVRs(int n);
		void update(Register& op, vector<int>& sr2vr, int& vrName);
		friend ostream& operator<<(ostream& os, const Scheduler& s);
};
