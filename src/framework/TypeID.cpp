#include "TypeID.h"

using std::string;

unsigned int TypeID::nextID_ = 0;
map<string, TypeID> TypeID::classMap_;

TypeID::TypeID(string s) : name_(s), value_(nextID_++) {}
	

TypeID* TypeID::getTypeID(string s)
{
	auto got = classMap_.find(s);
	if (got == classMap_.end()) {
		classMap_.insert(make_pair(s,TypeID(s)));
	}
	// now we have to be able to find the TypeID
	auto it = classMap_.find(s);
	return &(it->second);
}


string TypeID::toString() const{
	return name_;
}

unsigned int TypeID::getIDValue() const {
	return value_;
}

ostream &operator<<(ostream &os, TypeID const *tid) {
	os << tid->toString();
	return os;
}
