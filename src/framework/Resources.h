#pragma once
#include<string>

using namespace std;

class Resources
{

public:
	Resources(FixedPoint m, FixedPoint v, unsigned int su, unsigned int s): minerals(m), vespene(v), supply_used(su), supply(s){
	}


	string getMinerals() const {
		return to_string(minerals.getWholeUnits());
	}

	string getVespene() const {
		return to_string(vespene.getWholeUnits());
	}

	string getSupply_used() const {
		return to_string(supply_used);
	}

	string getSupply() const {
		return to_string(supply);
	}

	string toString() const
	{
		string res = "";
		string nl = "\n";
		string tab = "   ";
		res = tab + tab + tab + tab + "\"resources\": {" + nl
			+ tab + tab + tab + tab + tab + "\"minerals\": " + getMinerals() + "," + "\n"
			+ tab + tab + tab + tab + tab + "\"vespene\": " + getVespene() + "," + "\n"
			+ tab + tab + tab + tab + tab + "\"supply-used\": " + getSupply_used() + "," + "\n"
			+ tab + tab + tab + tab + tab + "\"supply\": " + getSupply() + "\n"
			+ tab + tab + tab + tab + "}" + "\n";

        return res;
    }

    ostream &operator<<(ostream &os) {
        os << toString();
        return os;
    }


private:
	FixedPoint minerals;
	FixedPoint vespene;
	unsigned int supply_used;
	unsigned int supply;


};


