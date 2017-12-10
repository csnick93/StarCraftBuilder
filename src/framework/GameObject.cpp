#include <stdexcept>
#include <memory>
#include "GameObject.h"
#include "BaseBuilding.h"
#include "Simulator.h"

using std::vector;
using std::unique_ptr;
using std::make_shared;

GameObject::GameObject(Simulator* simulator)
        : id_(ID()), minerals_(Minerals()), preconditions_(vector<TypeID *>()), producedIn_(vector<TypeID *>()),
          occupies_(nullptr), supply_(0), inventory_(nullptr), simulator_(simulator), morphedFromType_(nullptr), morphedFromTypeSupply_(0) {}


GameObject::GameObject(Simulator* s, Inventory *i, Minerals m, Supply supply, BuildTime bt,
                       vector<TypeID *> pp, vector<TypeID *> pd, TypeID *tid, TypeID* mo, Supply morph)
        : id_(ID()),
          simulator_(s),
          minerals_(m),
          supply_(supply),
          remainingBuildTime_(bt),
          producedIn_(pp),
          preconditions_(pd),
          typeID_(tid),
          morphedFromType_(mo),
          occupies_(nullptr),
          inventory_(i),
		  morphedFromTypeSupply_(morph),
		  morphedFrom_(ID::invalidID()){}


void GameObject::build() {
    if (inventory_ == nullptr)
        throw std::logic_error("GameObject must be assigned to an inventory before being build.");

    // check preconditions (supply is checked in Inventory::add, minerals in Inventory::checkout)
    if (!inventory_->has(preconditions_))
        throw std::logic_error("Preconditions for building this object are not met.");

    // try to occupy one of producedIn_
    occupies_ = inventory_->occupyOneOf(producedIn_);

    // decrease minerals in inventory
    try {
        inventory_->checkout(minerals_);
    }
    catch (...) {
        // if a unit had to be occupied for the build, reset it to lazy
        if (occupies_ != nullptr) {
            D(cout << "Resetting occupied status of " << *occupies_ << endl);
            occupies_->setActivity(BaseBuilding::LAZY);
            occupies_ = nullptr;
        }
        throw;
    }

    if (occupies_ != nullptr)
        simulator_->notify(make_shared<BuildStartEvent>(typeID(), occupies_->id_));
	else if (morphedFrom_.valid())
		simulator_->notify(make_shared<BuildStartEvent>(typeID(), morphedFrom_));
    else{
		D(cout << "Occupy pointer is nullptr at build start for " << *this << endl;);
        simulator_->notify(make_shared<BuildStartEvent>(typeID(), ID::invalidID()));
	}
	// notify inventory
	inventory_->notifyBuildStart(this);

	D(cout << "Build start " << typeID()->toString() << endl);
}


void GameObject::build(unique_ptr<GameObject> const &src) {
    if (inventory_ == nullptr)
        throw std::logic_error("GameObject must be assigned to an inventory before being build.");

    // consume source object (is then deleted automatically)
    auto obj = inventory_->remove(src);

	// rememember ID of src
	morphedFrom_ = obj->getID();

    try {
        build();
    }
    catch (...) {
		// reset morphedFrom ID
		morphedFrom_ = ID::invalidID();
		
        // re-add consumed object if build failed and rethrow
        inventory_->add(move(obj));
        throw;
    }
}


void GameObject::tick() {
    auto oldBt = remainingBuildTime_;

    // BuiltTime is unsigned!! -> make sure we do not get below 0
    remainingBuildTime_ -= oldBt >= BuildTime(1, 0) ? BuildTime(1, 0) : oldBt;

    if (oldBt > BuildTime(0, 0) && remainingBuildTime_ == BuildTime(0, 0)) {

        // fire build end event
		if (occupies_ != nullptr) {
			simulator_->notify(make_shared<BuildEndEvent>(typeID(), occupies_->id_, id_));
		}
		else if(morphedFrom_.valid()){
			simulator_->notify(make_shared<BuildEndEvent>(typeID(),morphedFrom_, id_));
        } else {
            simulator_->notify(make_shared<BuildEndEvent>(typeID(), ID::invalidID(), id_));
        }

        if (occupies_ != nullptr) {
            occupies_->setActivity(BaseBuilding::LAZY);
            occupies_ = nullptr;
        }

        inventory_->notifyBuildEnd(this);


        D(cout << "Build end " << typeID()->toString() << endl);
    }
}


Minerals const &GameObject::requiredMinerals() const {
    return minerals_;
}


vector<TypeID *> const &GameObject::preconditions() const {
    return preconditions_;
}


Supply GameObject::supply() const {
    return supply_;
}


void GameObject::setInventory(Inventory *inventory) {
    inventory_ = inventory;
}

void GameObject::setSimulator(Simulator * s)
{
	simulator_ = s;
}

Inventory * GameObject::getInventory() const
{
	return inventory_;
}

TypeID *GameObject::typeID() const {
    return typeID_;
}

ID GameObject::getID() const {
    return id_;
}

TypeID *GameObject::getMorphedFromType() const {
    return morphedFromType_;
}

vector<TypeID *> const &GameObject::producedIn() const {
	return producedIn_;
}

void GameObject::setIsBuilt() {
    remainingBuildTime_ = 0;
}

bool GameObject::isBuilt() const
{
	return remainingBuildTime_ == 0;
}

BuildTime GameObject::getBuildTime() const{
	return remainingBuildTime_;
}

ostream &operator<<(ostream &os, const GameObject &go) {
    os << go.typeID() << " (id: " << go.getID() << ")";
    return os;
}

