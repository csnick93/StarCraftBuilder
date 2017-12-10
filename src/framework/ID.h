#pragma once

#include <ostream>
#include <string>

using std::ostream;

/**
 * Pure numeric ID class that encapsulates a (self-incrementing) unsigned int and
 * provides comparison and ostream-operator.
 */
class ID {

private:

    static unsigned int nextID_;

    unsigned int value;

	bool valid_;

public:

    ID();

	friend ostream &operator<<(ostream &os, const ID &id);

    bool operator==(ID const &) const;

    bool operator<(ID const &) const;

	std::string toString() const;

	bool valid() const;

	static ID invalidID();


};


