#pragma once

#include <framework/Framework.h>
#include "TerranBuilding.h"

/*
TODO:
- introduce an additional occupies_-attribute (producedBy_) for buildings that are built by scv-units (some buildings built/morphed from other building)
- adjust tick()-method for Buildings that are built by scv
- overwrite build()-method for Buildings that are built by scv (need to be able to occupy worker-units)
- think about Buildings that are morphed from other buildings (e.g. barracks_with_reactor: is there morphing involved, or what kind of production is it?)
--> so all buildings that are built by morphing from other buildings, producedIn_ needs to be empty and morphedFrom_ needs to contain the corresponding building
*/

/*
TYPES OF BUILDINGS USED HERE:
	1) BaseBuildings : used for building units AND morphed from other building
	2) ProducingTerranBuilding: used for building units AND built by scv-workers
	3) IdleTerranBuilding: NOT used for building units AND built by scv-workers
*/

//==========================================================================================
// BUILDINGS
//===========================
// should be fine
class Command_Center : public ProducingTerranBuilding {
public:
	Command_Center(Simulator* s, Inventory* i);
};

//==========================================================================================
// should be fine (SPECIAL ABILITY BUILDING)
class Orbital_Command : public BaseBuilding {
public:
	Orbital_Command(Simulator* s, Inventory* i);

	void tick() override;

	/*
	Start special ability if enough energy present and special ability not active yet
	Return:
	True, if we activate special ability
	False, if not
	*/
	bool startSpecialAbility();

private:
	// energy status of orbital command
	FixedPoint energy_;
	// remaining time of special abilities
	deque<unsigned int> specialAbility_times;
	// 
	
};

//==========================================================================================
//TODO: test behavior of planetary fortress!
class Planetary_Fortress : public BaseBuilding {
public:
	Planetary_Fortress(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine (VESPENE BUILDING)
class Refinery : public VespeneBuilding {
public:
	Refinery(Simulator* s, Inventory* i);

	/*
	need to override tick()-method to adjust setting produced_by free instead of occupied_
	*/
	void tick() override;

	/*
	need to override build()-method such that the scv-unit can be occupied
	*/
	void build() override;
private:
	// scv-worker that is building the building
	BaseUnit* producedBy_;
};

//==========================================================================================
//should be fine
class Engineering_Bay : public IdleTerranBuilding {
public:
	Engineering_Bay(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine
class Missile_Turret : public IdleTerranBuilding {
public:
	Missile_Turret(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine
class Sensor_Tower : public IdleTerranBuilding {
public:
	Sensor_Tower(Simulator* s, Inventory* i);
};

//==========================================================================================
// should be fine
class Barracks : public ProducingTerranBuilding {
public:
	Barracks(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine
class Factory : public ProducingTerranBuilding {
public:
	Factory(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine
class Armory : public IdleTerranBuilding {
public:
	Armory(Simulator* s, Inventory* i);
};


//==========================================================================================
//should be fine
class Bunker : public IdleTerranBuilding {
public:
	Bunker(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine
class Ghost_Academy : public IdleTerranBuilding {
public:
	Ghost_Academy(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine
class Starport : public ProducingTerranBuilding {
public:
	Starport(Simulator* s, Inventory* i);
};

//==========================================================================================
// should be fine
class Fusion_Core : public IdleTerranBuilding {
public:
	Fusion_Core(Simulator* s, Inventory* i);
};

//=========================================================================================
//should be fine
class Supply_Depot : public IdleTerranBuilding {
public:
	Supply_Depot(Simulator* s, Inventory* i);
};

//==========================================================================================
//add functionality that it can produce two units at a time (Activity::LAZY->not producing, Activity::OTHER -> producing one unit, Acitivity::OCCUPIED -> producing two units)
/*
IDEA: outsiders using this unit think there is only LAZY && OCCUPIED, so convert OTHER state accordingly
*/
class Barracks_With_Reactor : public ReactorBuilding {
public:
	Barracks_With_Reactor(Simulator* s, Inventory* i);
};

//==========================================================================================
// should be fine
class Barracks_With_Tech_Lab : public BaseBuilding {
public:
	Barracks_With_Tech_Lab(Simulator* s, Inventory* i);
};

//==========================================================================================
// should be fine
class Starport_With_Reactor : public ReactorBuilding {
public:
	Starport_With_Reactor(Simulator* s, Inventory* i);
};

//==========================================================================================
// should be fine
class Starport_With_Tech_Lab : public BaseBuilding {
public:
	Starport_With_Tech_Lab(Simulator* s, Inventory* i);
};

//==========================================================================================
// should be fine
class Factory_With_Reactor : public ReactorBuilding {
public:
	Factory_With_Reactor(Simulator* s, Inventory* i);
};

//==========================================================================================
//should be fine
class Factory_With_Tech_Lab : public BaseBuilding {
public:
	Factory_With_Tech_Lab(Simulator* s, Inventory* i);
};

//==========================================================================================
