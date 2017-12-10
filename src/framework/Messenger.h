#pragma once
#include "Message.h"
#include<iostream>

using namespace std;

class Messenger {

	string begin = "{";
	string end = "}";
	string enquote = "\"";
	string comma = ",";
	string el = "\n";
	string tab = "   ";
	string beginValues = "[";
	string endValues = "]";


public:
	Messenger(bool activateSpecial) :messageList_(vector<Message>{}), activateSpecial_(activateSpecial){}

	void addMessage(Message m) {
		messageList_.push_back(m);
	}

	string streamSimulatorOutput(Race race, bool val)
	{
		string res;

		string raceAsString;
		if (race == Terran) {
			raceAsString = "sc2-hots-terran";
		}
		else if (race == Protoss) {
			raceAsString = "sc2-hots-protoss";
		}
		else {
			raceAsString = "sc2-hots-zerg";
		}


		if (!val)
		{
			res = begin + el + tab
				+enquote + "game" + enquote + ": " + enquote+ raceAsString +enquote + comma + el + tab
				+enquote + "buildlistValid" + enquote + ": " + "0" + el
				+ end
				+ "\n";
		}
		else {
			res = begin + el + tab
				+ enquote + "buildlistValid" + enquote + ": " + "1" + comma + el + tab
				+ enquote + "game" + enquote + ": " + enquote + raceAsString + enquote + comma + el + tab
				+ ((activateSpecial_) ? getInitialUnitsMessage(): "")
				+ getDataRow("messages", messageList_)
				+ el + end;
		}

		//clear messageList_
		messageList_.clear();

		return res;
	}

	void setInitialUnits(map<TypeID*, vector<ID>> initialUnits) {
		initialUnits_ = initialUnits;
	}

	string getInitialUnitsMessage()
	{
		string res = enquote+"initialUnits" +enquote+": "+ begin +el;
		unsigned int elementCounter = 0;
		for (auto it = initialUnits_.begin(); it != initialUnits_.end(); ++it) {
			res += (tab + tab + enquote + (it->first->toString()) + enquote + ": " + beginValues + el);
			for (auto unitIt = it->second.begin(); unitIt != it->second.end(); ++unitIt) {
				res += (tab + tab + tab + enquote + unitIt->toString() + enquote + (unitIt + 1 == it->second.end() ? "" : comma) + el);
			}
			res += (tab + tab + endValues + (++elementCounter < initialUnits_.size() ? comma : "")+el);
		}
		res += (tab + end + comma+el+tab);

		return res;
	}
		

	string getDataRow(string key, vector<Message> values) const
	{

		string res = enquote + key + enquote + ": " + beginValues + el;
		for (int i = 0; i < values.size(); i++)
		{
			res += values[i].toString();
			if (i != values.size() - 1) {
				res += comma + el;
			}
			else {
				res += el;
			}

		}

		res += (tab+endValues);

		return res;
	}


private:
	vector<Message> messageList_;
	map<TypeID*,vector<ID>> initialUnits_;
	bool activateSpecial_;
};