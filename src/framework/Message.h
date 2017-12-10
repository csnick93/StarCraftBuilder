#pragma once
#include<string>
#include<vector>
#include"Events.h"
#include"Status.h"
#include"Types.h"

using namespace std;

class Message
{
	string begin = "{";
	string end = "}";
	string enquote = "\"";
	string comma = ",";
	string el = "\n";
	string tab = "   ";
	string beginValues = "[";
	string endValues = "]";

public:
	Message(unsigned int t, Status s, vector<shared_ptr<Event>> e, bool activateSpecial): time(to_string(t)), status(s),events(e), activateSpecial_(activateSpecial)
	{
	}

	vector<shared_ptr<Event>> getEvents() const {
		return (events);
	}

	string getTime() const {
		return (time);
	}

	Status getStatus() const
	{
		return (status);
	}

	string toString() const
	{
		string nl = "\n";
		string res = tab+tab+begin + el 
					+ getDataRow("time", time, false)
					+ status.toString() + nl
					+ getDataRow("events", events)
					+ tab + tab + end;
		
		return res;
	}

	void streamToFile() {
		cout << toString();
	}

	string getDataRow(string key, string value, bool lastDataRow) const
	{
		string res = tab + tab + tab + enquote + key + enquote + ": " +  value  + (lastDataRow ? "" : comma) + el;
		return res;
	}

	string getDataRow(string key, vector<shared_ptr<Event>> values) const
	{
		
		string res = tab + tab + tab + enquote + key + enquote + ": " + beginValues + el;
		for (int i = 0; i < values.size(); i++)
		{
			res += values[i]->toString(activateSpecial_);
			if (i != values.size() - 1) {
				res += comma + el;
			} else {
				res += el;
			}

		}

		res += (tab + tab + tab + endValues+el);

		return res;
	}



private:

	vector<shared_ptr<Event>> events;
	string time;
	Status status;
	Race race;
	bool activateSpecial_;
};

