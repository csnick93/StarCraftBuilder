#pragma once
#include<string>

using namespace std;

class Workers
{

public:
	Workers(unsigned int m, unsigned int v): minerals(m), vespene(v) {
	}

	string getMinerals() const {
		return to_string(minerals);
	}

	string getVespene() const {
		return to_string(vespene);
	}

	string toString() const {
		string nl = "\n";
		string tab = "   ";
		string res = tab+tab+tab+tab+"\"workers\": {" + nl
			+ tab+tab+tab+tab+tab + "\"minerals\": " + getMinerals() + "," + "\n"
			+ tab + tab + tab + tab + tab + "\"vespene\": " + getVespene() + "\n"
			+ tab + tab + tab + tab + "}," + "\n";

		return res;
	}

	ostream& operator<<(ostream& os)
	{
		os << toString();

		return (os);
	}



private:
	unsigned int minerals;
	unsigned int vespene;



};


