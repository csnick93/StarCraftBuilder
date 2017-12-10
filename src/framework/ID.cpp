#include <ostream>
#include "ID.h"

using std::ostream;

unsigned int ID::nextID_ = 0;

ID::ID() : value(nextID_++), valid_(true) {}

ostream &operator<<(ostream &os, const ID &id) {
    if (id.valid_)
        os << id.value;
    else
        os << "invalid";
    return os;
}

bool ID::operator==(ID const &rhs) const {
    if (valid_ && rhs.valid_)
        return value == rhs.value;

    return false;
}

bool ID::operator<(ID const &rhs) const {
    if (valid_ && rhs.valid_)
        return value < rhs.value;

    return false;
}

std::string ID::toString() const
{
    if (valid_)
        return std::to_string(value);

    return "";
}

bool ID::valid() const {
    return valid_;
}

ID ID::invalidID() {
    ID r;
    ID::nextID_--;
    r.valid_ = false;
    return r;
}
