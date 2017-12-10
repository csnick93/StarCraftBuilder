#include "TerranBuilding.h"

/**
* @param s Simulator object
* @param i Inventory object
* @param m Minerals (gas/minerals) required to build this object
* @param supply Supply required/provided by this object
* @param bt Time required to build this object
* @param pp Vector of TypeID's that define in which buildings this object can be produced
* @param pd Vector of TypeID's that must be built in order to build this object (preconditions)
* @param tid TypeID of this GO
* @param mo TypeID that specified from which type this object is morphed
* @param morph The supply provided by the morphedFrom Object (needed for adjusting supply accordingly when morph process is finished)
*/
//===================================================================================================
/*
Class for Terran-buildings that are built by scv workers and are never responsible for building anything
*/


IdleTerranBuilding::IdleTerranBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt,
		vector<TypeID *> pp, vector<TypeID *> pd, TypeID *tid, TypeID *mo) :
		GameObject(s, i, m, supply, bt, pp, pd, tid, mo), producedBy_(nullptr) {}



void IdleTerranBuilding::tick() {
	/*
	Override tick()-method to adjust setting produced_by free instead of occupied_
	*/
	if (remainingBuildTime_ > 0 && --remainingBuildTime_ == 0) {
		// fire build end event
		if (producedBy_ != nullptr) {
			simulator_->notify(make_shared<BuildEndEvent>(typeID(), producedBy_->getID(), getID()));
			// remove occupied status
			producedBy_->setActivity(BaseUnit::LAZY);
			producedBy_ = nullptr;
		}
		else {
			throw std::logic_error("There is some mistake, this Terran-Building must be produced by a BaseUnit!");
		}
		getInventory()->notifyBuildEnd(this);
		D(cout << "Build end " << typeID()->toString() << endl);
	}

}


void IdleTerranBuilding::build() {
	/*
	Override build()-method such that the scv-unit can be occupied
	*/
	if (inventory_ == nullptr)
		throw std::logic_error("GameObject must be assigned to an inventory before being build.");

	// check preconditions (supply is checked in Inventory::add, minerals in Inventory::checkout)
	if (!inventory_->has(preconditions_))
		throw std::logic_error("Preconditions for building this object are not met.");

	//try to find a worker to build the building
	producedBy_ = inventory_->occupyBuildUnit(producedIn_);

	// decrease minerals in inventory
	try {
		inventory_->checkout(minerals_);
	}
	catch (...) {
		D(cout << "Resetting occupied status of " << *producedBy_ << endl);
		producedBy_->setActivity(BaseUnit::MINERALS);
		producedBy_ = nullptr;
		throw;
	}

	if (producedBy_ != nullptr)
		simulator_->notify(make_shared<BuildStartEvent>(typeID(), producedBy_->getID()));
	else
		throw std::logic_error("There is some mistake, this IdleTerran-Building must be produced by a BaseUnit!");

	// notify inventory
	inventory_->notifyBuildStart(this);

	D(cout << "Build start " << typeID()->toString() << endl);
}




//===================================================================================================

/*
class of Buildings that are built by scv-workers and are also responsible for building units themselves
*/


ProducingTerranBuilding::ProducingTerranBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt, vector<TypeID *> pp,
		vector<TypeID *> pd, TypeID *tid, TypeID *mo, Supply morph) :
		BaseBuilding(s, i, m, supply, bt, pp, pd, tid, mo, morph), producedBy_(nullptr) {}



void ProducingTerranBuilding::tick(){
	/*
	Override tick()-method to adjust setting produced_by free instead of occupied_
	*/
	if (remainingBuildTime_ > 0 && --remainingBuildTime_ == 0) {
		// fire build end event
		if (producedBy_ != nullptr) {
			simulator_->notify(make_shared<BuildEndEvent>(typeID(), producedBy_->getID(), getID()));
			// remove occupied status
			producedBy_->setActivity(BaseUnit::LAZY);
			producedBy_ = nullptr;
		}
		else {
			throw std::logic_error("There is some mistake, ProducingTerranBuilding must be produced by a BaseUnit!");
		}
		getInventory()->notifyBuildEnd(this);
		D(cout << "Build end " << typeID()->toString() << endl);
	}
}


void ProducingTerranBuilding::build() {
	/*
	Override build()-method such that the scv-unit can be occupied
	*/
	if (inventory_ == nullptr)
		throw std::logic_error("GameObject must be assigned to an inventory before being build.");

	// check preconditions (supply is checked in Inventory::add, minerals in Inventory::checkout)
	if (!inventory_->has(preconditions_))
		throw std::logic_error("Preconditions for building this object are not met.");

	//try to find a worker to build the building
	producedBy_ = inventory_->occupyBuildUnit(producedIn_);

	// decrease minerals in inventory
	try {
		inventory_->checkout(minerals_);
	}
	catch (...) {
		producedBy_->setActivity(BaseUnit::MINERALS);
		producedBy_ = nullptr;
		throw;
	}

	if (producedBy_ != nullptr)
		simulator_->notify(make_shared<BuildStartEvent>(typeID(), producedBy_->getID()));
	else
		throw std::logic_error("There is some mistake, ProducingTerranBuilding must be produced by a BaseUnit!");

	// notify inventory
	inventory_->notifyBuildStart(this);

	D(cout << "Build start " << typeID()->toString() << endl);
}


//===================================================================================================

/*
building class for modeling reactor buildings (that have the "produce two units at a time" special ability)
*/
ReactorBuilding::ReactorBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt,
		vector<TypeID *> pp, vector<TypeID *> pd, TypeID *tid, TypeID *mo) :
		BaseBuilding(s, i, m, supply, bt, pp, pd, tid, mo) {}


ReactorBuilding::Activity ReactorBuilding::getActivity() const {
	if (activity_ == BaseBuilding::OTHER) {
		return BaseBuilding::LAZY;
	}
	return activity_;
}

void ReactorBuilding::setActivity(Activity a) {
	if (activity_ == BaseBuilding::LAZY && a == BaseBuilding::OCCUPIED) {
		activity_ = BaseBuilding::OTHER;
	}
	else if (activity_ == BaseBuilding::OCCUPIED && a == BaseBuilding::LAZY) {
		activity_ = BaseBuilding::OTHER;
	}
	else {
		activity_ = a;
	}
}

bool ReactorBuilding::occupied() const {
	return activity_ == BaseBuilding::OCCUPIED;
}
