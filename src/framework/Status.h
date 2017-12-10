#pragma once
#include "Resources.h"
#include "Workers.h"
#include<string>


class Status
{

public:
	Status(Workers w, Resources r): workers(w), resources(r) {
	}


	Workers getWorkers() const {
		return (workers);
	}

	Resources getResources() const {
		return (resources);
	}

	string toString() const {
		string el = "\n";
		string comma = ",";
		string tab = "   ";
		string res = tab+tab+tab+"\"status\": {" + el
			+ workers.toString()
			+ resources.toString()
			+ tab + tab + tab + "},";
			//+"\n";

		return res;
	}


	ostream& operator<<(ostream& os)
	{
		os << toString();
		return (os);
	}

private:
	Workers workers;
	Resources resources;


};




