#include <framework/Framework.h>
#include "GameObjectManagerTerran.h"
#include <framework/Optimizer.h>
#include <string>
#include <memory>
#include <cstdio>
#include <iostream>
#include <cstdio>
#include <chrono>



using std::string;
using std::cerr;
using std::endl;
using std::make_shared;


int main(int argc, char *argv[]) {
    if (argc != 3) {
        cerr << "Wrong number of arguments: %sourcefile %path2buildList %path2outputFile" << endl;
        return -1;
    }

    string path = argv[1];
	string output_path = argv[2];

	freopen(output_path.c_str(), "w", stdout);

    Simulator s(true);
	
	try{
		s.create(std::make_shared<GameObjectManagerTerran>(), path);
	}
	catch (std::exception& e) {
		return 0;
    }

    
	tuple<bool, unsigned int, string> result(s.run());
	
	cout << std::get<2>(result) << endl;


	
	std::ofstream out_file(output_path);
	if (out_file.is_open()) {
		out_file << std::get<2>(result);
		out_file.close();
	}
	else {
		std::cout << "Output file cannot be opened." << std::endl;
	}

    return 0;
}
