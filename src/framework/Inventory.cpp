#include <vector>
#include <map>
#include <memory>
#include <algorithm>
#include <numeric>
#include "TypeID.h"
#include "FixedPoint.h"
#include "Inventory.h"
#include "GameObject.h"
#include "BaseBuilding.h"
#include "BaseUnit.h"
#include "Debug.h"
#include "Simulator.h"

using std::vector;
using std::map;
using std::unique_ptr;
using std::logic_error;
using std::out_of_range;
using std::find;
using std::move;
using std::accumulate;
using std::cout;
using std::endl;

Inventory::Inventory() : supplyAvailable_(0), supplyUsed_(0), minerals_(), cachedMinerals_(), objects_(),
                         objectsUnderConstruction_(), mineralCollectors_(0), gasHarvesters_(0) {}

void Inventory::checkout(const Minerals &minerals) {
	if (!has(minerals))
        throw logic_error("Not enough minerals to check out");

    minerals_ -= minerals;
}

unique_ptr<GameObject>* Inventory::objectsPushBack(unique_ptr<GameObject> o) {
    deque<unique_ptr<GameObject>> *list;

    if (o->isBuilt()) {
        try {
            list = &(objectsByTypeID(o->typeID()));
        } catch (out_of_range) {
            auto n = objects_.emplace(o->typeID(), deque<unique_ptr<GameObject>>());
            list = &(*n.first).second;
        }
    } else {
        list = &objectsUnderConstruction_;
    }

    list->push_back(move(o));
	return &(list->back());
}

void Inventory::initialize(vector<unique_ptr<GameObject>> &objects, Minerals minerals) {
    minerals_ += minerals;

    for (auto &o: objects) {
        if (o->supply() < 0)
            supplyUsed_ += -o->supply();
        else
            supplyAvailable_ += o->supply();

        objectsPushBack(move(o));
    }

    objects.clear();
}

unique_ptr<GameObject>* Inventory::add(unique_ptr<GameObject> o) {
    // must not be nullptr
    assert(o);

    // If there is not enough supply, return the GameObject again
    if (!has(o->supply())) {
        D(cout << "Attempted to add " << *o << " to the inventory but supply did not suffice (low supply)" << endl);
        return nullptr;
    }
    o->setInventory(this);

    D(cout << "Successfully added " << *o << " to the inventory" << endl);

    return objectsPushBack(move(o));
}

unique_ptr<GameObject> Inventory::remove(unique_ptr<GameObject> const &v) {
    // must not be nullptr
    assert(v);

    auto list = v->isBuilt() ? &(objectsByTypeID(v->typeID())) : &objectsUnderConstruction_;
    auto o = find(list->begin(), list->end(), v);

    if (o == list->end())
        throw out_of_range("GameObject not found in inventory");

    auto ret = move(*o);
    list->erase(o);

    D(cout << "Removed " << *ret << " from the inventory" << endl);

    return ret;
}

bool Inventory::has(const Minerals &minerals) const {
    return minerals <= minerals_;
}

bool Inventory::has(TypeID *typeID) const {
    // must not be nullptr
    assert(typeID);

    try {
        return (getObjectsByTypeID(typeID).size() > 0);
    } catch (out_of_range) {
        return false;
    }
}


bool Inventory::has(vector<TypeID *> const &typeids) const {
	/*
	Only one of the preconditions has to be true, not all of them!
	*/
	if (typeids.size() == 0) {
		return true;
	}
	for (auto& obj : typeids) {
		if (has(obj)) {
			return true;
		}
	}
	return false;
	
}

bool Inventory::has(Supply supply) const {
	// adding supply since BaseUnit->supply() is negative, and BaseBuilding->supply() is positive
    return supplyAvailable_ - supplyUsed_ + supply >= 0;
}

bool Inventory::canBuild(const GameObject &o) const {
    return has(o.supply()) && has(o.requiredMinerals()) && has(o.preconditions());
}

void Inventory::deliverMinerals(const Minerals &minerals) {
    cachedMinerals_ += minerals;
}

void Inventory::updateMinerals() {
    minerals_ += cachedMinerals_;
    cachedMinerals_ = 0;
}

deque<unique_ptr<GameObject> > &Inventory::objectsByTypeID(TypeID *typeID) {
    // must not be nullptr
    assert(typeID);

    return objects_.at(typeID);
}

deque<unique_ptr<GameObject> > const &Inventory::getObjectsByTypeID(TypeID *typeID) const {
    // must not be nullptr
    assert(typeID);

    return objects_.at(typeID);
}

BaseBuilding *Inventory::occupyOneOf(const vector<TypeID *> &typeIDs) const {
    for (auto tid: typeIDs) {
        // must not be nullptr
        assert(tid);

        auto const &list = getObjectsByTypeID(tid);

        for (auto const &object : list) {
            // must not be nullptr
            assert(object);

			auto b = dynamic_cast<BaseBuilding *>(object.get());

			if (b == nullptr)
				throw std::logic_error("occupyOneOf must only be called with TypeIDs that refer to buildings.");


            if (!b->occupied()) {
                b->setActivity(BaseBuilding::OCCUPIED);

                D(cout << "Occupied building " << *b << endl);

                return b;
            }
        }
    }

    if (!typeIDs.empty())
        throw std::logic_error("Could not occupy any of the given TypeIDs.");

    return nullptr;
}


BaseUnit *Inventory::occupyBuildUnit(const vector<TypeID *> &typeIDs) const {
    for (auto tid : typeIDs) {
        // must not be nullptr
        assert(tid);

		auto const &list = getObjectsByTypeID(tid);

        for (auto const &object : list) {
            // must not be nullptr
            assert(object);

			auto b = dynamic_cast<BaseUnit *>(object.get());

			if (b == nullptr)
				throw std::logic_error("occupyBuildUnit must only be called with TypeIDs that refer to BaseUnits.");

			if (b->availableForBuild()) {
				b->setActivity(BaseUnit::OTHER);

                D(cout << "Occupied unit " << *b << endl);

				return b;
			}
		}
	}

	if (!typeIDs.empty())
		throw std::logic_error("Could not occupy any of the given TypeIDs.");

	return nullptr;
}



void Inventory::addSupply(Supply s) {
	if (s < 0) {
		throw std::logic_error("Only possible to add, not subtract supply!");
	}
	supplyAvailable_ += s;
}

void Inventory::removeSupply(Supply s)
{
	supplyAvailable_ -= s;
}



unsigned Inventory::getNumberMineralCollectors() const {
    return mineralCollectors_;
}

unsigned Inventory::getNumberVespeneCollectors() const {
    return gasHarvesters_;
}

FixedPoint Inventory::getNumberMinerals() const {
    return minerals_.minerals();
}

FixedPoint Inventory::getNumberGas() const {
    return minerals_.gas();
}

Supply Inventory::getNumberAvailableSupply() const {
    return supplyAvailable_;
}

Supply Inventory::getNumberSupplyUsed() const {
    return supplyUsed_;
}

void Inventory::tick() {
    for (auto const &list: objects_) {
        for (auto &object: list.second) {
            object->tick();
        }
    }
	
	std::deque<unsigned int> deleteIndex;
	unsigned int counter = 0;
    for (auto it = objectsUnderConstruction_.begin(); it < objectsUnderConstruction_.end(); ++it) {
        (*it)->tick();

        // if build has finished, move object to objects_
        if ((*it)->isBuilt()) {
            auto tmp = move(*it);
            //objectsUnderConstruction_.erase(it);
            objectsPushBack(move(tmp));
			deleteIndex.push_back(counter);
        }
		++counter;
    }

	//delete all the entries in objectsUnderConstruction that are finished building
	for (auto it = deleteIndex.begin(); it < deleteIndex.end();++it) {
		objectsUnderConstruction_.erase(objectsUnderConstruction_.begin() + *it);
	}
}

ostream &operator<<(ostream &os, const Inventory &i) {
    os << "RESOURCES:" << endl;
    os << "\tMinerals: " << i.minerals_.minerals() << endl;
    os << "\tGas: " << i.minerals_.gas() << endl;
    os << "\tSupply available (total): " << i.supplyAvailable_ << endl;
    os << "\tSupply used: " << i.supplyUsed_ << endl;

    os << "UNITS:" << endl;
    for (auto const &l: i.objects_) {
        os << "\t" << l.first << ": " << l.second.size() << endl;
    }

    return os;
}

void Inventory::notifyBuildStart(GameObject *o) {
    if (o->supply_ < 0)
        // object needs supply -> effective from build start on
        supplyUsed_ += -(o->supply_);
}

void Inventory::notifyBuildEnd(GameObject *o) {
    if (o->supply_ > 0) {
        // object provides supply -> effective as soon as build has finished
        supplyAvailable_ += o->supply_;

		// check if we morphed from another object
		// if yes: we need to subtract the supply that the morphedFromType supplied
		if (o->morphedFromType_ != nullptr) {
			supplyAvailable_ -= o->morphedFromTypeSupply_;
		}
    }
}

bool Inventory::hasObjectsUnderConstruction() {
    return objectsUnderConstruction_.size() > 0;
}

deque<unique_ptr<GameObject>> const &Inventory::getObjectsUnderConstruction() const {
    return objectsUnderConstruction_;
}
