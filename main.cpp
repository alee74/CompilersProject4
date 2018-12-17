/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *                                                         *
 * main.cpp                                                *
 *                                                         *
 * Main for Scheduler implementation.                      *
 * Expects file as command line argument.                  *
 * Uses file to construct Scheduler, which constructs      *
 * a Parser (that constructs a Scanner) to parse and       *
 * scan file and build its intermediate representation,    *
 * which it utilizes (and modifies) in order to construct  *
 * a depenency graph and compute latency-weighted          *
 * distances from each Node to the root.                   *
 *                                                         *
 * Run with [-h --help] option for additional info.        *
 *                                                         *
 * Written by: Austin James Lee                            *
 *                                                         *
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

#define MIN_ARGS 2
#define MAX_ARGS 2

#include "scheduler.h"
#include <cstring>	// strcmp()

using std::strcmp;

// helper function prototypes
bool validFile(string filename);


/// main ///
int main(int argc, char* argv[]) {
	string infile = "";
	string usage = "usage: reader [-h --help] <filename>\n"
					"where: <filename> is the name of the file to be compiled\n"
					"       and brackets indicate program options.\n"
					"**invoke the help option for further details.";
	string help = "\n"
		"\'sched\' performs the first half of instruction scheduling by constructing\n"
		"a dependency graph from the ILOC code found in the input file and then\n"
		"calculating the latency-weighted distances between each node and a root node.\n\n"
		"usage: reader [-h --help] <filename>\n\n"
		"Program arguments:\n"
		"      -h   help option. prints this help summary and exits the simulation.\n"
		"           --help is the verbose form of this option.\n"
		"filename   the name of a file containing ILOC code to be compiled.\n"
		"           unless the help option is invoked, this will always be the\n"
		"           last option.\n";
		

	// ensure correct number of arguments
	if (argc < MIN_ARGS) {
		cerr << "error: not enough arguments"
			<< endl << usage << endl;
		return 1;
	} else if (argc > MAX_ARGS) {
		cerr << "error: too many arguments"
			<< endl << usage << endl;
		return 1;
	// parse arguments
	} else {
		// parse -h & --help
		if (strcmp(argv[1], "-h") == 0 ||
			strcmp(argv[1], "--help") == 0) {
				cout << help << endl;
				return 0;
		// parse filename for argc == 2
		} else if (validFile(argv[1]))
			infile = argv[1];
		// bad filename
		else if (argc == 2) {
			cerr << "error: invalid filename: " 
				<< argv[1] << endl << usage << endl;
			return 1;
		// bad argument
		} else {
			cerr << "error: invalid argument: "
				<< argv[1] << endl << usage << endl;
			return 1;
		}

	}


	// Create Scheduler object.
	// all functionality is actived by constructor.
	Scheduler scheduler {infile};

	// print output.
	cout << scheduler;

	return 0;
}


// tests for valid file
bool validFile(string filename) {
	bool valid = false;
	ifstream f(filename);
	if (f) {
		valid = true;
		f.close();
	}
	return valid;
}

