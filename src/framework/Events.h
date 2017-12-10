#pragma once

#include<string>
#include"TypeID.h"
#include <sstream>
#include "ID.h"
#include <vector>
#include "Types.h"

using std::string;
using std::stringstream;
using std::vector;



class Event
{
	

public:

	virtual string getType() const = 0;

    virtual string toString(bool activateSpecial) const = 0;

	string getDataRow(string key, string value, bool lastDataRow) const
	{
		string res = enquote + key + enquote + ": " + enquote + value + enquote + (lastDataRow ? "" : comma) + "\n";
		return res;
	}

	string getDataRow(string key, vector<ID> values) const
	{

		string res = enquote + key + enquote + ": " + beginValues + el;
		for (int i = 0; i < values.size(); i++)
		{
			res += (tab + tab + tab + tab + tab + tab+enquote+values[i].toString()+enquote+el);			//TODO: is this correct?
		}

		res += (tab + tab + tab + tab + tab+endValues+el);

		return res;
	}

protected:
	string begin = "{";
	string end = "}";
	string enquote = "\"";
	string comma = ",";
	string el = "\n";
	string tab = "   ";
	string beginValues = "[";
	string endValues = "]";

};


class BuildStartEvent : public Event
{

public:

	string getType() const override{
		return "build-start";
	}

	BuildStartEvent(TypeID *name, ID producerID) : name(name), producerID(producerID) {}

	string toString(bool activateSpecial) const override
	{
		string res;


		res = tab + tab + tab + tab + begin + el
              + tab + tab + tab + tab + tab + getDataRow("type", getType(), false)
              + tab + tab + tab + tab + tab +
              getDataRow("name", name->toString(), !activateSpecial || !producerID.valid())
              + ((producerID.valid() && activateSpecial)? tab + tab + tab + tab + tab + getDataRow("producerID", producerID.toString(), 1) : "")
              + tab + tab + tab + tab + end;

		return res;

	}
		

protected:
	TypeID *name;
	ID producerID;
};


class BuildEndEvent : public Event
{


public:
	string getType() const override{
		return "build-end";
	}

	BuildEndEvent(TypeID *name, ID producerID, ID producedID) : name(name), producerID(producerID),
		producedIDs(vector<ID>{producedID}) {
	}

	BuildEndEvent(TypeID *name, ID producerID, vector<ID> producedIDs) : name(name), producerID(producerID),
		producedIDs(producedIDs) {
	}


	string toString(bool activateSpecial) const override
	{
		string res;

		res = tab + tab + tab + tab + begin + el
			  + tab + tab + tab + tab + tab + getDataRow("type", getType(), 0)
			  + tab + tab + tab + tab + tab + getDataRow("name", name->toString(), !activateSpecial)
			  + ((producerID.valid()&& activateSpecial) ? tab + tab + tab + tab + tab + getDataRow("producerID", producerID.toString(), 0) : "")
			  + (activateSpecial?tab + tab + tab + tab + tab + getDataRow("producedIDs", producedIDs) : "")
			  + tab + tab + tab + tab + end;

		return res;
	}

	pair<TypeID*, ID> getAttributes() const{
		return make_pair(name, producerID);
	}

	ID getProducedID() const{
		// this method shall only be called during accumulation -> only one element per vector 
		assert(producedIDs.size() == 1);
		return producedIDs.front();
	}

protected:
	TypeID *name;
	ID producerID;
	vector<ID> producedIDs;
};

class SpecialAbilityEvent : public Event
{


public:
    SpecialAbilityEvent(Race race, ID triggeredBy, ID targetBuilding) : race(race), triggeredBy(triggeredBy),
                                                                          targetBuilding(targetBuilding)
	{
	}

    string getType() const override{
        return string("special");
    }


	string toString(bool activateSpecial) const override
	{
		string res;
		if (race == Terran)
		{
			res = tab + tab + tab + tab + begin + el
                  + tab + tab + tab + tab + tab + getDataRow("type", "special", 0)
                  + tab + tab + tab + tab + tab + getDataRow("name", "mule", 0)
                  + tab + tab + tab + tab + tab + getDataRow("triggeredBy", triggeredBy.toString(), 1)
                  + tab + tab + tab + tab + end;
		}
		else if (race == Zerg)
		{
			
			res = tab + tab + tab + tab + begin + el
                  + tab + tab + tab + tab + tab + getDataRow("type", "special", 0)
                  + tab + tab + tab + tab + tab + getDataRow("name", "injectlarvae", 0)
                  + tab + tab + tab + tab + tab + getDataRow("triggeredBy", triggeredBy.toString(), 0)
                  + tab + tab + tab + tab + tab + getDataRow("targetBuilding", targetBuilding.toString(), 1)
                  + tab + tab + tab + tab + end;
		}
		else {
			
			res = tab + tab + tab + tab + begin + el
                  + tab + tab + tab + tab + tab + getDataRow("type", "special", 0)
                  + tab + tab + tab + tab + tab + getDataRow("name", "chronoboost", 0)
                  + tab + tab + tab + tab + tab + getDataRow("triggeredBy", triggeredBy.toString(), 0)
                  + tab + tab + tab + tab + tab + getDataRow("targetBuilding", targetBuilding.toString(), 1)
                  + tab + tab + tab + tab + end;
		}

		return res;
	}

protected:
	Race race;
    ID triggeredBy;
    ID targetBuilding;

};
