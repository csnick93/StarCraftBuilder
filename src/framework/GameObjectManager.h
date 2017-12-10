#pragma once
#include "GameObject.h"
#include "Types.h"
#include "Inventory.h"
#include <vector>
#include <tuple>

using std::tuple;
using std::shared_ptr;
using std::string;
using std::vector;

// forward declaration
class Simulator;

/*
GameObjectFactory is an abstract class that returns a shared pointer to
the factory instance of the desired race. This will will most likely
be needed in the BuildList and Simulator class.
*/


class GameObjectManager {
public:
	/*
	Get vector of TypeIDs of all possible GameObjects
	*/
	virtual vector<TypeID*> getAllTypeIDs() const = 0;

	/*
	create the GameObject you need by handing over its name (create("scv"))
	*/
	virtual unique_ptr<GameObject> create(TypeID* gameObject, Simulator* s,Inventory* i) const = 0;

	/*
	get race that the factory produces
	*/
	virtual Race getRace() const = 0;

	/*
	get TypeID-pointer of basic worker
	*/
	virtual TypeID* getBasicWorkerTypeID() const = 0;

	/*
	get TypeID-pointer of Vespene Geysir
	*/
	virtual TypeID* getVespeneGeysirTypeID() const = 0;

	/*
	get TypeID-pointer of base (command_center, nexus, hatchery)
	*/
	virtual TypeID* getBaseTypeID() const = 0;

	/*
	manage the special abilities
	*/
	virtual void manageSpecialAbilities(Inventory* inventoryPtr_) const = 0;

    /**
     * Return a vector of TypeID's that represent the start configuration of a race.
	 * ATTENTION: PLEASE ADD YOUR BASE BUILDING FIRST TO THE VECTOR (otherwise, there will be issues with the supply when adding to the inventory)
     */
    virtual vector<TypeID*> getStartTypes() const = 0;

	/*
	Return vector with TypeIDs of starting types (however as a set, so no duplicates)
	*/
	virtual vector<pair<TypeID*,unsigned int>> getUniqueStartTypes() const = 0;

	/*
	Return initial available supply, given the start conditions
	*/
	virtual Supply initialSupply() const = 0;

	/*
	Get GameObjectData to TypeID: {producedIn, preconditions, supply, morphedFromType, requiredMinerals} 
	*/
	virtual const tuple<vector<TypeID*>, vector<TypeID*>, Supply, TypeID*, Minerals>& getGameObjectInfo(TypeID*) = 0;


	
protected:
	/*
	save information about all gameObjects in a map (needed for efficient optimization)
	*/
	map<TypeID*, tuple<vector<TypeID*>, vector<TypeID*>, Supply, TypeID*, Minerals>> gameObjectInfo_;

	

};


