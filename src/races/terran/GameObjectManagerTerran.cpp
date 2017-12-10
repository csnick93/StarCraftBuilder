#include "GameObjectManagerTerran.h"

GameObjectManagerTerran::GameObjectManagerTerran():GameObjectManager()
{
	gameObjectInfo_ = map<TypeID*, tuple<vector<TypeID*>, vector<TypeID*>, Supply, TypeID*, Minerals>>{
		make_pair(TypeID::getTypeID("scv"),make_tuple(vector<TypeID*>{TypeID::getTypeID("command_center"),TypeID::getTypeID("orbital_command"),TypeID::getTypeID("planetary_fortress")},vector<TypeID*>{},-1,nullptr,Minerals(FixedPoint(50), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("marine"),make_tuple(vector<TypeID*>{TypeID::getTypeID("barracks"),TypeID::getTypeID("barracks_with_tech_lab"),TypeID::getTypeID("barracks_with_reactor")},vector<TypeID*>{},-1,nullptr,Minerals(FixedPoint(50), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("marauder"),make_tuple(vector<TypeID*>{TypeID::getTypeID("barracks_with_tech_lab")},vector<TypeID*>{},-2,nullptr,Minerals(FixedPoint(100), FixedPoint(25)))),
		make_pair(TypeID::getTypeID("reaper"),make_tuple(vector<TypeID*>{TypeID::getTypeID("barracks_with_tech_lab")},vector<TypeID*>{},-1,nullptr,Minerals(FixedPoint(50), FixedPoint(50)))),
		make_pair(TypeID::getTypeID("ghost"),make_tuple(vector<TypeID*>{TypeID::getTypeID("barracks_with_tech_lab")},vector<TypeID*>{TypeID::getTypeID("ghost_academy")},-2,nullptr,Minerals(FixedPoint(200), FixedPoint(100)))),
		make_pair(TypeID::getTypeID("hellion"),make_tuple(vector<TypeID*>{TypeID::getTypeID("factory"),TypeID::getTypeID("factory_with_reactor"),TypeID::getTypeID("factory_with_tech_lab")},vector<TypeID*>{},-2,nullptr,Minerals(FixedPoint(100), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("siege_tank"),make_tuple(vector<TypeID*>{TypeID::getTypeID("factory_with_tech_lab")},vector<TypeID*>{},-3,nullptr,Minerals(FixedPoint(150), FixedPoint(125)))),
		make_pair(TypeID::getTypeID("thor"),make_tuple(vector<TypeID*>{TypeID::getTypeID("factory_with_tech_lab")},vector<TypeID*>{TypeID::getTypeID("armory")},-6,nullptr,Minerals(FixedPoint(300), FixedPoint(200)))),
		make_pair(TypeID::getTypeID("medivac"),make_tuple(vector<TypeID*>{TypeID::getTypeID("starport"),TypeID::getTypeID("starport_with_reactor"),TypeID::getTypeID("starport_with_tech_lab")},vector<TypeID*>{},-2,nullptr,Minerals(FixedPoint(100), FixedPoint(100)))),
		make_pair(TypeID::getTypeID("viking"),make_tuple(vector<TypeID*>{TypeID::getTypeID("starport"),TypeID::getTypeID("starport_with_reactor"),TypeID::getTypeID("starport_with_tech_lab")},vector<TypeID*>{},-2,nullptr,Minerals(FixedPoint(150), FixedPoint(75)))),
		make_pair(TypeID::getTypeID("raven"),make_tuple(vector<TypeID*>{TypeID::getTypeID("starport_with_tech_lab")},vector<TypeID*>{},-2,nullptr,Minerals(FixedPoint(100), FixedPoint(200)))),
		make_pair(TypeID::getTypeID("banshee"),make_tuple(vector<TypeID*>{TypeID::getTypeID("starport_with_tech_lab")},vector<TypeID*>{},-3,nullptr,Minerals(FixedPoint(150), FixedPoint(100)))),
		make_pair(TypeID::getTypeID("battlecruiser"),make_tuple(vector<TypeID*>{TypeID::getTypeID("starport_with_tech_lab")},vector<TypeID*>{TypeID::getTypeID("fusion_core")},-6,nullptr,Minerals(FixedPoint(400), FixedPoint(300)))),
		make_pair(TypeID::getTypeID("command_center"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{},-0,nullptr,Minerals(FixedPoint(400), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("orbital_command"),make_tuple(vector<TypeID*>{TypeID::getTypeID("command_center")},vector<TypeID*>{TypeID::getTypeID("barracks"),TypeID::getTypeID("barracks_with_tech_lab"),TypeID::getTypeID("barracks_with_reactor")},-0,TypeID::getTypeID("command_center"),Minerals(FixedPoint(150), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("planetary_fortress"),make_tuple(vector<TypeID*>{TypeID::getTypeID("command_center")},vector<TypeID*>{TypeID::getTypeID("engineering_bay")},-0,TypeID::getTypeID("command_center"),Minerals(FixedPoint(150), FixedPoint(150)))),
		make_pair(TypeID::getTypeID("refinery"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{},-0,nullptr,Minerals(FixedPoint(75), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("engineering_bay"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{},-0,nullptr,Minerals(FixedPoint(125), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("missile_turret"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("engineering_bay")},-0,nullptr,Minerals(FixedPoint(100), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("sensor_tower"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("engineering_bay")},-0,nullptr,Minerals(FixedPoint(125), FixedPoint(100)))),
		make_pair(TypeID::getTypeID("barracks"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("supply_depot")},-0,nullptr,Minerals(FixedPoint(150), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("factory"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("barracks"),TypeID::getTypeID("barracks_with_tech_lab"),TypeID::getTypeID("barracks_with_reactor")},-0,nullptr,Minerals(FixedPoint(150), FixedPoint(100)))),
		make_pair(TypeID::getTypeID("armory"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("factory"),TypeID::getTypeID("factory_with_tech_lab"),TypeID::getTypeID("factory_with_reactor")},-0,nullptr,Minerals(FixedPoint(150), FixedPoint(100)))),
		make_pair(TypeID::getTypeID("bunker"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("barracks"),TypeID::getTypeID("barracks_with_tech_lab"),TypeID::getTypeID("barracks_with_reactor")},-0,nullptr,Minerals(FixedPoint(100), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("ghost_academy"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("barracks"),TypeID::getTypeID("barracks_with_tech_lab"),TypeID::getTypeID("barracks_with_reactor")},-0,nullptr,Minerals(FixedPoint(150), FixedPoint(50)))),
		make_pair(TypeID::getTypeID("starport"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("factory"),TypeID::getTypeID("factory_with_tech_lab"),TypeID::getTypeID("factory_with_reactor")},-0,nullptr,Minerals(FixedPoint(150), FixedPoint(100)))),
		make_pair(TypeID::getTypeID("fusion_core"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{TypeID::getTypeID("starport"),TypeID::getTypeID("starport_with_tech_lab"),TypeID::getTypeID("starport_with_reactor")},-0,nullptr,Minerals(FixedPoint(150), FixedPoint(150)))),
		make_pair(TypeID::getTypeID("supply_depot"),make_tuple(vector<TypeID*>{TypeID::getTypeID("scv")},vector<TypeID*>{},-0,nullptr,Minerals(FixedPoint(100), FixedPoint(0)))),
		make_pair(TypeID::getTypeID("barracks_with_reactor"),make_tuple(vector<TypeID*>{TypeID::getTypeID("barracks")},vector<TypeID*>{},-0,TypeID::getTypeID("barracks"),Minerals(FixedPoint(50), FixedPoint(50)))),
		make_pair(TypeID::getTypeID("barracks_with_tech_lab"),make_tuple(vector<TypeID*>{TypeID::getTypeID("barracks")},vector<TypeID*>{},-0,TypeID::getTypeID("barracks"),Minerals(FixedPoint(50), FixedPoint(25)))),
		make_pair(TypeID::getTypeID("starport_with_reactor"),make_tuple(vector<TypeID*>{TypeID::getTypeID("starport")},vector<TypeID*>{},-0,TypeID::getTypeID("starport"),Minerals(FixedPoint(50), FixedPoint(50)))),
		make_pair(TypeID::getTypeID("starport_with_tech_lab"),make_tuple(vector<TypeID*>{TypeID::getTypeID("starport")},vector<TypeID*>{},-0,TypeID::getTypeID("starport"),Minerals(FixedPoint(50), FixedPoint(25)))),
		make_pair(TypeID::getTypeID("factory_with_reactor"),make_tuple(vector<TypeID*>{TypeID::getTypeID("factory")},vector<TypeID*>{},-0,TypeID::getTypeID("factory"),Minerals(FixedPoint(50), FixedPoint(50)))),
		make_pair(TypeID::getTypeID("factory_with_tech_lab"),make_tuple(vector<TypeID*>{TypeID::getTypeID("factory")},vector<TypeID*>{},-0,TypeID::getTypeID("factory"),Minerals(FixedPoint(50), FixedPoint(25))))
	};
}

vector<TypeID*> GameObjectManagerTerran::getAllTypeIDs() const
{
	vector<string> units{ string("scv"), string("marine"), string("marauder"),
		string("reaper"), string("ghost"), string("hellion"), string("siege_tank"),
		string("thor"), string("medivac"), string("viking"), string("raven"), string("banshee"),
		string("battlecruiser"), string("command_center"), string("orbital_command"),
		string("planetary_fortress"), string("refinery"), string("engineering_bay"),
		string("missile_turret"), string("sensor_tower"), string("barracks"), string("factory"),
		string("armory"), string("bunker"), string("ghost_academy"), string("starport"),
		string("fusion_core"), string("supply_depot"), string("barracks_with_reactor"),
		string("barracks_with_tech_lab"), string("starport_with_reactor"), string("starport_with_tech_lab"),
		string("factory_with_reactor"), string("factory_with_tech_lab") };

	vector<TypeID*> allTypeIds;
	for (auto& obj : units) {
		allTypeIds.push_back(TypeID::getTypeID(obj));
	}
	return allTypeIds;
}

void GameObjectManagerTerran::manageSpecialAbilities(Inventory * inventoryPtr_) const
{
	if (inventoryPtr_->has(TypeID::getTypeID("orbital_command"))) {
		deque<unique_ptr<GameObject>> const &orbital_command_units = inventoryPtr_->getObjectsByTypeID(
			TypeID::getTypeID("orbital_command"));
		for (auto& unit : orbital_command_units) {
			// as soon as one special ability has started, exit the function
			if (dynamic_cast<Orbital_Command*>(unit.get())->startSpecialAbility()) {
				return;
			}
		}
	}
}

vector<TypeID*> GameObjectManagerTerran::getStartTypes() const
{
	vector<TypeID*> startTypes;
	startTypes.push_back(getBaseTypeID());
	for (unsigned int i = 0; i < 6; ++i) {
		startTypes.push_back(getBasicWorkerTypeID());
	}
	return startTypes;
}

vector<pair<TypeID*, unsigned int>> GameObjectManagerTerran::getUniqueStartTypes() const
{
	vector<pair<TypeID*, unsigned int>> startTypes{ make_pair(getBaseTypeID(),1),make_pair(getBasicWorkerTypeID(),6) };
	return startTypes;
}

unique_ptr<GameObject> GameObjectManagerTerran::create(TypeID * gameObject, Simulator * s, Inventory * i) const
{
	
	if (gameObject->toString() == "scv") {
		return unique_ptr<GameObject>(new Scv(s, i));
	}
	else if (gameObject->toString() == "marine") {
		return unique_ptr<GameObject>(new Marine(s, i));
	}
	else if (gameObject->toString() == "marauder") {
		return unique_ptr<GameObject>(new Marauder(s, i));
	}
	else if (gameObject->toString() == "reaper") {
		return unique_ptr<GameObject>(new Reaper(s, i));
	}
	else if (gameObject->toString() == "ghost") {
		return unique_ptr<GameObject>(new Ghost(s, i));
	}
	else if (gameObject->toString() == "hellion") {
		return unique_ptr<GameObject>(new Hellion(s, i));
	}
	else if (gameObject->toString() == "siege_tank") {
		return unique_ptr<GameObject>(new Siege_Tank(s, i));
	}
	else if (gameObject->toString() == "thor") {
		return unique_ptr<GameObject>(new Thor(s, i));
	}
	else if (gameObject->toString() == "medivac") {
		return unique_ptr<GameObject>(new Medivac(s, i));
	}
	else if (gameObject->toString() == "viking") {
		return unique_ptr<GameObject>(new Viking(s, i));
	}
	else if (gameObject->toString() == "raven") {
		return unique_ptr<GameObject>(new Raven(s, i));
	}
	else if (gameObject->toString() == "banshee") {
		return unique_ptr<GameObject>(new Banshee(s, i));
	}
	else if (gameObject->toString() == "battlecruiser") {
		return unique_ptr<GameObject>(new Battlecruiser(s, i));
	}
	else if (gameObject->toString() == "command_center") {
		return unique_ptr<GameObject>(new Command_Center(s, i));
	}
	else if (gameObject->toString() == "orbital_command") {
		return unique_ptr<GameObject>(new Orbital_Command(s, i));
	}
	else if (gameObject->toString() == "planetary_fortress") {
		return unique_ptr<GameObject>(new Planetary_Fortress(s, i));
	}
	else if (gameObject->toString() == "refinery") {
		return unique_ptr<GameObject>(new Refinery(s, i));
	}
	else if (gameObject->toString() == "engineering_bay") {
		return unique_ptr<GameObject>(new Engineering_Bay(s, i));
	}
	else if (gameObject->toString() == "missile_turret") {
		return unique_ptr<GameObject>(new Missile_Turret(s, i));
	}
	else if (gameObject->toString() == "sensor_tower") {
		return unique_ptr<GameObject>(new Sensor_Tower(s, i));
	}
	else if (gameObject->toString() == "barracks") {
		return unique_ptr<GameObject>(new Barracks(s, i));
	}
	else if (gameObject->toString() == "factory") {
		return unique_ptr<GameObject>(new Factory(s, i));
	}
	else if (gameObject->toString() == "armory") {
		return unique_ptr<GameObject>(new Armory(s, i));
	}
	else if (gameObject->toString() == "bunker") {
		return unique_ptr<GameObject>(new Bunker(s, i));
	}
	else if (gameObject->toString() == "ghost_academy") {
		return unique_ptr<GameObject>(new Ghost_Academy(s, i));
	}
	else if (gameObject->toString() == "starport") {
		return unique_ptr<GameObject>(new Starport(s, i));
	}
	else if (gameObject->toString() == "fusion_core") {
		return unique_ptr<GameObject>(new Fusion_Core(s, i));
	}
	else if (gameObject->toString() == "supply_depot") {
		return unique_ptr<GameObject>(new Supply_Depot(s, i));
	}
	else if (gameObject->toString() == "barracks_with_reactor") {
		return unique_ptr<GameObject>(new Barracks_With_Reactor(s, i));
	}
	else if (gameObject->toString() == "barracks_with_tech_lab") {
		return unique_ptr<GameObject>(new Barracks_With_Tech_Lab(s, i));
	}
	else if (gameObject->toString() == "starport_with_reactor") {
		return unique_ptr<GameObject>(new Starport_With_Reactor(s, i));
	}
	else if (gameObject->toString() == "starport_with_tech_lab") {
		return unique_ptr<GameObject>(new Starport_With_Tech_Lab(s, i));
	}
	else if (gameObject->toString() == "factory_with_reactor") {
		return unique_ptr<GameObject>(new Factory_With_Reactor(s, i));
	}
	else if (gameObject->toString() == "factory_with_tech_lab") {
		return unique_ptr<GameObject>(new Factory_With_Tech_Lab(s, i));
	}
	else {
		throw std::logic_error("Trying to create non-existent unit!");
	}
}
