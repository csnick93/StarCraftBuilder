#include "TerranBuildings.h"

/*
TYPES OF BUILDINGS USED HERE:
1) BaseBuildings : used for building units AND morphed from other building
2) ProducingTerranBuilding: used for building units AND built by scv-workers
3) IdleTerranBuilding: NOT used for building units AND built by scv-workers
*/


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

//===========================
// BUILDINGS                =
//===========================

Command_Center::Command_Center(Simulator* s, Inventory* i) :
		ProducingTerranBuilding(s, i, Minerals(FixedPoint(400), FixedPoint(0)), Supply(11), BuildTime(100),
			vector<TypeID *>{TypeID::getTypeID("scv")},
			vector<TypeID *>{}, TypeID::getTypeID("command_center"), nullptr) {}


//==========================================================================================
// should be fine (SPECIAL ABILITY BUILDING)

Orbital_Command::Orbital_Command(Simulator* s, Inventory* i) :
		BaseBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(0)), Supply(11), BuildTime(35),
			vector<TypeID*>{},
			vector<TypeID*>{TypeID::getTypeID("barracks"), TypeID::getTypeID("barracks_with_tech_lab"), TypeID::getTypeID("barracks_with_reactor")},
			TypeID::getTypeID("orbital_command"), TypeID::getTypeID("command_center"), Supply(11)), energy_(FixedPoint(50)), specialAbility_times(deque<unsigned int>{}) {}

void Orbital_Command::tick() {
	// build-time is over
	if (remainingBuildTime_ > 0 && --remainingBuildTime_ == 0) {
		// fire build end event
		if (morphedFrom_.valid()) {
			simulator_->notify(make_shared<BuildEndEvent>(typeID(), morphedFrom_, id_));
		}
		else
			throw std::logic_error("Orbital command morphs from command_center, need valid morphFrom_ ID!");

		inventory_->notifyBuildEnd(this);

		D(cout << "Build end " << typeID()->toString() << endl);

	}
	// orbital command is built -> load up on energy (if we are not exceeding the 200 energy units)
	// and execute special ability if activated
	else if (remainingBuildTime_ == 0) {
		// load up on energy
		if ((energy_ + FixedPoint(0, 5625)).getWholeUnits() < 200) {
			energy_ += FixedPoint(0, 5625);
		}
		// execute special ability
		if (specialAbility_times.size()>0) {
			inventory_->deliverMinerals(Minerals(FixedPoint(2, 8)*specialAbility_times.size(), FixedPoint()));
			//update timers and check if we run out of time
			/*
			Lowest times are always in front -> deque is sorted increasingly (take advantage of that for deletion)
			delete all the entries that only have one remaining second (those must be in front)
			*/
			while (specialAbility_times.front() == 1) {
				D(cout << "****Stopping special ability****" << endl);
				specialAbility_times.pop_front();
			}
			for (auto it = specialAbility_times.begin(); it < specialAbility_times.end(); ++it) {
				--(*it);
			}
		}
	}
}


bool Orbital_Command::startSpecialAbility() {
	/*
	Start special ability if enough energy present and special ability not active yet
	Returns:
		True, if we activate special ability
		False, if not
	*/
	bool startAbility = false;
	// if we have enough energy and do not already have special ability activated, activate special ability
	while (energy_.getWholeUnits() >= 50) {
		D(cout << "****Starting special ability****" << endl);
		energy_ -= FixedPoint(50);
		specialAbility_times.push_back(90);
		// create special ability event
		simulator_->notify(make_shared<SpecialAbilityEvent>(Terran, getID(), ID::invalidID()));
		startAbility = true;
	}
	return startAbility;
}

//==========================================================================================

Planetary_Fortress::Planetary_Fortress(Simulator* s, Inventory* i) :
		BaseBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(150)), Supply(11), BuildTime(50),
			vector<TypeID*>{},
			vector<TypeID*>{TypeID::getTypeID("engineering_bay")}, TypeID::getTypeID("planetary_fortress"), TypeID::getTypeID("command_center"), Supply(11)) {}


//==========================================================================================
Refinery::Refinery(Simulator* s, Inventory* i) :
		VespeneBuilding(s, i, Minerals(FixedPoint(75), FixedPoint(0)), Supply(0), BuildTime(30),
			vector<TypeID*>{TypeID::getTypeID("scv")},
			vector<TypeID*>{}, TypeID::getTypeID("refinery"), nullptr) {}

void Refinery::tick() {
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
			throw std::logic_error("There is some mistake, refinery must be produced by a BaseUnit!");
		}

		//adjust supply in inventory
		inventory_->notifyBuildEnd(this);

		D(cout << "Build end " << typeID()->toString() << endl);
	}
}


void Refinery::build() {
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
		throw std::logic_error("Refinery must be built by Scv");

	// notify inventory
	inventory_->notifyBuildStart(this);

	D(cout << "Build start " << typeID()->toString() << endl);
}

//==========================================================================================
Engineering_Bay::Engineering_Bay(Simulator* s, Inventory* i) :
		IdleTerranBuilding(s, i, Minerals(FixedPoint(125), FixedPoint(0)), Supply(0), BuildTime(35),
			vector<TypeID *>{TypeID::getTypeID("scv")},
			vector<TypeID *>{}, TypeID::getTypeID("engineering_bay"), nullptr) {}


//==========================================================================================
Missile_Turret::Missile_Turret(Simulator* s, Inventory* i) :
		IdleTerranBuilding(s, i, Minerals(FixedPoint(100), FixedPoint(0)), Supply(0), BuildTime(25),
			vector<TypeID *>{TypeID::getTypeID("scv")},
			vector<TypeID *>{TypeID::getTypeID("engineering_bay")},
			TypeID::getTypeID("missile_turret"), nullptr) {}


//==========================================================================================
Sensor_Tower::Sensor_Tower(Simulator* s, Inventory* i) :
	IdleTerranBuilding(s, i, Minerals(FixedPoint(125), FixedPoint(100)), Supply(0), BuildTime(25),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("engineering_bay")},
		TypeID::getTypeID("sensor_tower"), nullptr) {}


//==========================================================================================
Barracks::Barracks(Simulator* s, Inventory* i) :
	ProducingTerranBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(0)), Supply(0), BuildTime(65),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("supply_depot")}, TypeID::getTypeID("barracks"),
		nullptr) {}

//==========================================================================================
Factory::Factory(Simulator* s, Inventory* i) :
	ProducingTerranBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(100)), Supply(0), BuildTime(60),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("barracks"),
		TypeID::getTypeID("barracks_with_tech_lab"),
		TypeID::getTypeID("barracks_with_reactor")},
		TypeID::getTypeID("factory"), nullptr) {}

//==========================================================================================
Armory::Armory(Simulator* s, Inventory* i) :
	IdleTerranBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(100)), Supply(0), BuildTime(65),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("factory"),
		TypeID::getTypeID("factory_with_tech_lab"),
		TypeID::getTypeID("factory_with_reactor")},
		TypeID::getTypeID("armory"), nullptr) {}

//==========================================================================================
Bunker::Bunker(Simulator* s, Inventory* i) :
	IdleTerranBuilding(s, i, Minerals(FixedPoint(100), FixedPoint(0)), Supply(0), BuildTime(35),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("barracks"),
		TypeID::getTypeID("barracks_with_tech_lab"),
		TypeID::getTypeID("barracks_with_reactor")},
		TypeID::getTypeID("bunker"), nullptr) {}

//==========================================================================================
Ghost_Academy::Ghost_Academy(Simulator* s, Inventory* i) :
	IdleTerranBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(50)), Supply(0), BuildTime(40),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("barracks"),
		TypeID::getTypeID("barracks_with_tech_lab"),
		TypeID::getTypeID("barracks_with_reactor")},
		TypeID::getTypeID("ghost_academy"), nullptr) {}


//==========================================================================================
Starport::Starport(Simulator* s, Inventory* i) :
	ProducingTerranBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(100)), Supply(0), BuildTime(50),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("factory"), TypeID::getTypeID("factory_with_tech_lab"),
		TypeID::getTypeID("factory_with_reactor")}, TypeID::getTypeID("starport"),
		nullptr) {}


//==========================================================================================

Fusion_Core::Fusion_Core(Simulator* s, Inventory* i) :
	IdleTerranBuilding(s, i, Minerals(FixedPoint(150), FixedPoint(150)), Supply(0), BuildTime(65),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{TypeID::getTypeID("starport"),
		TypeID::getTypeID("starport_with_tech_lab"),
		TypeID::getTypeID("starport_with_reactor")},
		TypeID::getTypeID("fusion_core"), nullptr) {}

//==========================================================================================

Supply_Depot::Supply_Depot(Simulator* s, Inventory* i) :
	IdleTerranBuilding(s, i, Minerals(FixedPoint(100), FixedPoint(0)), Supply(8), BuildTime(30),
		vector<TypeID *>{TypeID::getTypeID("scv")},
		vector<TypeID *>{}, TypeID::getTypeID("supply_depot"), nullptr) {}

//==========================================================================================

Barracks_With_Reactor::Barracks_With_Reactor(Simulator* s, Inventory* i) :
	ReactorBuilding(s, i, Minerals(FixedPoint(50), FixedPoint(50)), Supply(0), BuildTime(50),
		vector<TypeID *>{},
		vector<TypeID *>{}, TypeID::getTypeID("barracks_with_reactor"),
		TypeID::getTypeID("barracks")) {}

//==========================================================================================
Barracks_With_Tech_Lab::Barracks_With_Tech_Lab(Simulator* s, Inventory* i) :
	BaseBuilding(s, i, Minerals(FixedPoint(50), FixedPoint(25)), Supply(0), BuildTime(25),
		vector<TypeID*>{},
		vector<TypeID*>{}, TypeID::getTypeID("barracks_with_tech_lab"), TypeID::getTypeID("barracks")) {}


//==========================================================================================
Starport_With_Reactor::Starport_With_Reactor(Simulator* s, Inventory* i) :
	ReactorBuilding(s, i, Minerals(FixedPoint(50), FixedPoint(50)), Supply(0), BuildTime(50),
		vector<TypeID *>{},
		vector<TypeID *>{}, TypeID::getTypeID("starport_with_reactor"),
		TypeID::getTypeID("starport")) {}

//==========================================================================================
Starport_With_Tech_Lab::Starport_With_Tech_Lab(Simulator* s, Inventory* i) :
	BaseBuilding(s, i, Minerals(FixedPoint(50), FixedPoint(25)), Supply(0), BuildTime(25),
		vector<TypeID*>{},
		vector<TypeID*>{}, TypeID::getTypeID("starport_with_tech_lab"), TypeID::getTypeID("starport")) {}
//==========================================================================================
Factory_With_Reactor::Factory_With_Reactor(Simulator* s, Inventory* i) :
	ReactorBuilding(s, i, Minerals(FixedPoint(50), FixedPoint(50)), Supply(0), BuildTime(50),
		vector<TypeID *>{},
		vector<TypeID *>{}, TypeID::getTypeID("factory_with_reactor"),
		TypeID::getTypeID("factory")) {}


//==========================================================================================
Factory_With_Tech_Lab::Factory_With_Tech_Lab(Simulator* s, Inventory* i) :
	BaseBuilding(s, i, Minerals(FixedPoint(50), FixedPoint(25)), Supply(0), BuildTime(25),
		vector<TypeID*>{},
		vector<TypeID*>{}, TypeID::getTypeID("factory_with_tech_lab"), TypeID::getTypeID("factory")) {}

//==========================================================================================

