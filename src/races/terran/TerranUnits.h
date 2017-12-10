#pragma once
#define NDEBUG
#include <framework/Framework.h>


//===========================
// BASE UNIT
//===========================

// BaseUnit(Simulator* s, Inventory* i, Minerals m, Supply supply, BuildTime bt, vector<TypeID> producedIn_, vector<TypeID> preconditions, TypeID* tid, TypeID* morphedFrom) 
//should be fine
class Scv : public BaseUnit {
public:
	Scv(Simulator* s, Inventory* i) :
		BaseUnit(s, i, Minerals(FixedPoint(50), FixedPoint(0)), Supply(-1), BuildTime(17),
			vector<TypeID*>{TypeID::getTypeID("command_center"), TypeID::getTypeID("orbital_command"), TypeID::getTypeID("planetary_fortress")},
			vector<TypeID*>{}, TypeID::getTypeID("scv"), nullptr) {
	}
};

//===========================
// UNITS
//===========================

//===========================
// should be fine
class Marine : public GameObject {
public:
	Marine(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(50), FixedPoint(0)), Supply(-1), BuildTime(25),
			vector<TypeID*>{TypeID::getTypeID("barracks"), TypeID::getTypeID("barracks_with_tech_lab"), TypeID::getTypeID("barracks_with_reactor")},
			vector<TypeID*>{}, TypeID::getTypeID("marine"), nullptr) {}
};

//===========================
// should be fine
class Marauder : public GameObject {
public:
	Marauder(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(100), FixedPoint(25)), Supply(-2), BuildTime(30),
			vector<TypeID*>{TypeID::getTypeID("barracks_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("marauder"), nullptr) {}
};

//===========================
// should be fine
class Reaper : public GameObject {
public:
	Reaper(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(50), FixedPoint(50)), Supply(-1), BuildTime(45),
			vector<TypeID*>{TypeID::getTypeID("barracks_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("reaper"), nullptr) {}
};

//===========================
// should be fine
class Ghost : public GameObject {
public:
	Ghost(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(200), FixedPoint(100)), Supply(-2), BuildTime(40),
			vector<TypeID*>{TypeID::getTypeID("barracks_with_tech_lab")},
			vector<TypeID*>{TypeID::getTypeID("ghost_academy")}, TypeID::getTypeID("ghost"), nullptr) {}
};

//===========================
// should be fine
class Hellion : public GameObject {
public:
	Hellion(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(100), FixedPoint(0)), Supply(-2), BuildTime(30),
			vector<TypeID*>{TypeID::getTypeID("factory"), TypeID::getTypeID("factory_with_reactor"), TypeID::getTypeID("factory_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("hellion"), nullptr) {}
};

//===========================
// should be fine
class Siege_Tank : public GameObject {
public:
	Siege_Tank(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(150), FixedPoint(125)), Supply(-3), BuildTime(45),
			vector<TypeID*>{TypeID::getTypeID("factory_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("siege_tank"), nullptr) {}
};

//===========================
// should be fine
class Thor : public GameObject {
public:
	Thor(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(300), FixedPoint(200)), Supply(-6), BuildTime(60),
			vector<TypeID*>{TypeID::getTypeID("factory_with_tech_lab")},
			vector<TypeID*>{TypeID::getTypeID("armory")}, TypeID::getTypeID("thor"), nullptr) {}
};

//===========================
// should be fine
class Medivac : public GameObject {
public:
	Medivac(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(100), FixedPoint(100)), Supply(-2), BuildTime(42),
			vector<TypeID*>{TypeID::getTypeID("starport"), TypeID::getTypeID("starport_with_reactor"), TypeID::getTypeID("starport_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("medivac"), nullptr) {}
};

//===========================
// should be fine
class Viking : public GameObject {
public:
	Viking(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(150), FixedPoint(75)), Supply(-2), BuildTime(42),
			vector<TypeID*>{TypeID::getTypeID("starport"), TypeID::getTypeID("starport_with_reactor"), TypeID::getTypeID("starport_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("viking"), nullptr) {}
};

//===========================
// should be fine
class Raven : public GameObject {
public:
	Raven(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(100), FixedPoint(200)), Supply(-2), BuildTime(60),
			vector<TypeID*>{TypeID::getTypeID("starport_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("raven"), nullptr) {}
};

//===========================
// should be fine
class Banshee : public GameObject {
public:
	Banshee(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(150), FixedPoint(100)), Supply(-3), BuildTime(60),
			vector<TypeID*>{TypeID::getTypeID("starport_with_tech_lab")},
			vector<TypeID*>{}, TypeID::getTypeID("banshee"), nullptr) {}
};

//===========================
// should be fine
class Battlecruiser : public GameObject {
public:
	Battlecruiser(Simulator* s, Inventory* i) :
		GameObject(s, i, Minerals(FixedPoint(400), FixedPoint(300)), Supply(-6), BuildTime(90),
			vector<TypeID*>{TypeID::getTypeID("starport_with_tech_lab")},
			vector<TypeID*>{TypeID::getTypeID("fusion_core")}, TypeID::getTypeID("battlecruiser"), nullptr) {}
};










