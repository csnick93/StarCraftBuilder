#pragma once

#include <string>
#include <iostream>
#include <map>


using std::string;
using std::pair;
using std::make_pair;
using std::map;
using std::ostream;


class TypeID {
	/*
	Description: instead of creating TypeIDs, you ask for a TypeId with the static method
				getTypeId(string s). In return you get a reference to a TypeID that you can work with.
				We do not need a bool operator anymore, because we can directly compare the TypeId references
				in order to check whether we have objects of the same class.
	*/

private:
	TypeID(string s);

	unsigned int value_;
	string name_;

	/*
	save IDs that have been given out to classes (be able to look up IDs of classes)
	*/
	static map<string, TypeID> classMap_;

	/*
	counter for giving out new IDs
	*/
	static unsigned int nextID_;
public:
	/*
	return reference to TypeID in classMap_
	*/
	static TypeID* getTypeID(string s);

	// should not be necessary since we can directly compare the pointer addresses
	//bool operator==(const TypeID&) const;

	string toString() const;

	unsigned int getIDValue() const;

	/**
	 * stream operator
	 */
	friend ostream &operator<<(ostream &, TypeID const *);

};
