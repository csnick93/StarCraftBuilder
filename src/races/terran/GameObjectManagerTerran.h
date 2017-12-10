#pragma once

#include <framework/Framework.h>
#include "TerranBuildings.h"
#include "TerranUnits.h"
#include <string>
#include <tuple>

using std::string;
using std::tuple;


class GameObjectManagerTerran : public GameObjectManager
{
public:
	GameObjectManagerTerran();


	// {producedIn, preconditions, supply, morphedFromType, requiredMinerals} 
	tuple<vector<TypeID*>, vector<TypeID*>, Supply, TypeID*, Minerals>& getGameObjectInfo(TypeID* tid)
	{
		auto findInfo = gameObjectInfo_.find(tid);
		return findInfo->second;
	}


	vector<TypeID*> getAllTypeIDs() const;

	Race getRace() const
	{
		return Terran;
	}

	TypeID* getBasicWorkerTypeID() const
	{
		return TypeID::getTypeID(string("scv"));
	}

	TypeID* getVespeneGeysirTypeID() const
	{
		return TypeID::getTypeID(string("refinery"));
	}


	virtual TypeID* getBaseTypeID() const
	{
		return TypeID::getTypeID(string("command_center"));
	}

	/*
	Check all orbital commands if any of them can start the MULE abiltiy
	ONLY ONE may actually start it though
	*/
	void manageSpecialAbilities(Inventory* inventoryPtr_) const;

	vector<TypeID*> getStartTypes() const;

	vector<pair<TypeID*, unsigned int>> getUniqueStartTypes() const;


	virtual Supply initialSupply() const
	{
		return 5;
	}

	unique_ptr<GameObject> create(TypeID* gameObject, Simulator* s, Inventory* i) const;
};