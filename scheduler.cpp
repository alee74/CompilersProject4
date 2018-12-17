/* * * * * * * * * * * * * * * * *
 *                               *
 * scheduler.cpp                 *
 *                               *
 * Compilers Project 4           *
 *                               *
 * Written by: Austin James Lee  *
 *                               *
 * * * * * * * * * * * * * * * * */

#include "scheduler.h"



// Node constructor.
Scheduler::Node::Node(Instruction in) :i{in}, weight{0} {}


// Scheduler constructor.
//
// Sets Instruction labels, creates Nodes,
// assigns virtual registers, then calls member
// functions to create dependency graph and
// calculate latency-weighted distances to roots.
Scheduler::Scheduler(string infile, bool sp)
			:intRep{Parser{infile, sp}.intRep} {
	int n = 0;
	int highReg = -1;

	for (Instruction in : intRep) {
		// set Instruction labels and create to Nodes
		if (in.op != nop) {
			in.label = n++;
			nodes.push_back(new Node{in});
		}
		// get highest number of sr so
		// sr2vr is of adequate size
		if (in.src1.isReg && in.src1.sr > highReg)
			highReg = in.src1.sr;
		if (in.src2.isReg && in.src2.sr > highReg)
			highReg = in.src2.sr;
		if (in.dest.isReg && in.dest.sr > highReg)
			highReg = in.dest.sr;
	}

	// assign unique VR to each value
	assignVRs(highReg + 1);

	// create edges between nodes
	buildDepGraph();

	// compute latency-weighted distances to roots
	computeWeights();

}


// Scheduler destructor.
//
// Deletes all dynamically allocated Nodes.
Scheduler::~Scheduler() {
	for (Node* n : nodes)
		delete n;
}


// builds dependency graph.
void Scheduler::buildDepGraph() {

	Node* snapper;
	Node* arctic;
	bool addEdge;
	bool recentStore;
	bool recentOutput;

	// 'turtle' for slower outer iterator
	for (auto turtle = nodes.begin();  turtle != nodes.end(); ++turtle) {

		snapper = *turtle;
		recentStore = false;
		recentOutput = false;

		// 'fox' for quicker inner iterator
		for (auto fox = turtle + 1; fox != nodes.end(); ++fox) {

			arctic = *fox;
			addEdge = false;

			// determine whether to add Serialization Edge
			switch (snapper->i.op) {

				case load:
					if (arctic->i.op == store)
						addEdge = true;
					break;

				case output:
					if (!recentOutput
					&& (arctic->i.op == output
					|| arctic->i.op == store))
						addEdge = true;
					break;

				case store:
					if (!recentStore
					&& (arctic->i.op == load
					|| arctic->i.op == store
					|| arctic->i.op == output))
						addEdge = true;
					break;

				default:
					break;
			}

			// track what's beyond the turtle;
			// only want most recent of these for edges.
			if (arctic->i.op == store)
				recentStore = true;
			if (arctic->i.op == output)
				recentOutput = true;

			// determine whether to add Register Edge
			if ((arctic->i.src1.isReg && arctic->i.src1.vr == snapper->i.dest.vr)
			|| (arctic->i.src2.isReg && arctic->i.src2.vr == snapper->i.dest.vr))
				addEdge = true;

			// add the edges!
			if (addEdge) {
				arctic->children.push_back(snapper);
				snapper->parents.push_back(arctic);
			}

		}
	}

}


// computes latencty-weighted distance to
// a root for every node using a worklist.
void Scheduler::computeWeights() {

	queue<Node*> worklist;

	// find roots - put them on worklist
	for (Node* n : nodes) {
		if (n->parents.empty())
			worklist.push(n);
	}

	// perform work until worklist is empty
	Node* curr;
	int latency;
	while (!worklist.empty()) {

		curr = worklist.front();
		worklist.pop();

		// properly set the latency
		switch (curr->i.op) {
			case load:
			case store:
				latency = 3;
				break;
			case mult:
				latency = 2;
				break;
			default:
				latency = 1;
				break;
		}

		bool putBack = false;	// flag
		int heavyParent = 0;	// max parent weight

		// if not a root
		if (!curr->parents.empty()) {
			for (Node* n : curr->parents) {
				if (n->weight == 0) {
					putBack = true;
					worklist.push(n);
				} else if (n->weight > heavyParent)
					heavyParent = n->weight;
			}
		}

		if (putBack)
			worklist.push(curr);
		else {
			curr->weight = heavyParent + latency;
			for (Node* n : curr->children)
				worklist.push(n);
		}

	}

}


// assigns a virtual register to each source register.
// Essentially computeLastUse without tracking nextUse.
void Scheduler::assignVRs(int n) {
	int vrName = 0;
	vector<int> sr2vr (n, INVALID);

	auto it = nodes.end();
	while (it != nodes.begin()) {
		--it;
		// update and kill dest
		if ((*it)->i.dest.isReg) {
			update((*it)->i.dest, sr2vr, vrName);
			sr2vr[(*it)->i.dest.sr] = INVALID;
		}
		// update src1
		if ((*it)->i.src1.isReg)
			update((*it)->i.src1, sr2vr, vrName);
		// update src2
		if ((*it)->i.src2.isReg)
			update((*it)->i.src2, sr2vr, vrName);
	}
}


// helper function for assignVRs.
// eliminates redundant code.
void Scheduler::update(Register& op, vector<int>& sr2vr, int& vrName) {
	if (sr2vr[op.sr] == INVALID)
		sr2vr[op.sr] = vrName++;
	op.vr = sr2vr[op.sr];
}


// overload of output operator for simple printing.
ostream& operator<<(ostream& os, const Scheduler& s) {
	// indent padding
	string pad = "       ";

	// print nodes
	os << "nodes:" << endl;
	for (auto n : s.nodes)
		os << pad << "n" << n->i.label << " : " << n->i;
	os << endl;

	// print edges
	vector<int> edges;
	os << "edges:" << endl;
	for (auto n : s.nodes) {
		os << pad << "n" << n->i.label << " : { ";
		for (auto it = n->children.begin(); it < n->children.end(); ++it) {
			edges.push_back((*it)->i.label);
			sort(edges.begin(), edges.end(), [](int x, int y) { return x < y; });
		}
		for (auto it = edges.begin(); it != edges.end(); ++it) {
			os << "n" << *it;
			if (it != edges.end() - 1)
				os << ", ";
		}
		os << " }" << endl;
		edges.clear();
	}
	os << endl;

	// print weights
	os << "weights:" << endl;
	for (auto n : s.nodes)
		os << pad << "n" << n->i.label << " : " << n->weight << endl;
	os << endl;

	return os;
}

