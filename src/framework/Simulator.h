//Simulator.h
#pragma once

#include "BaseUnit.h"
#include "Inventory.h"
#include "Message.h"
#include "Messenger.h"
#include "BuildList.h"
#include "GameObjectManager.h"
#include "Events.h"
#include "TypeID.h"
#include "Types.h"
#include "VespeneBuilding.h"
#include "DefinedExceptions.h"
#include <unordered_map>
#include <memory>
#include <vector>
#include <exception>
#include <tuple>
#include <utility>


using std::unique_ptr;
using std::shared_ptr;
using std::vector;
using std::unordered_map;
using std::string;
using std::pair;
using std::make_tuple;


class Simulator {
public:
	/*
	Constructor
	*/
	Simulator(bool activateSpecial = true);

	/*
	instantiate parameters in Simulator
	*/
	void create(shared_ptr<GameObjectManager> gameObjectManager, string path);

	/*
	instantiate parameters in Simulator
	*/
	void create(shared_ptr<GameObjectManager> gameObjectManager, BuildList& bl);

	/*
	method to run the complete simulation
	*/
	tuple<bool, unsigned int, string> run(unsigned int maxTimeSteps = 1000);

	/*
	Do the five required steps in the time step according to instructions
	*/
	void simulateOneTimeStep();
	
	/*
	Update the resources -> let the inventory update itself
	*/
	void updateResources() const;

	/*
	check for finished builds (not necessary as gameObjects notify simulator automatically)
	*/
	void checkFinishedBuilds() const;

	/*
	Manage start and stop of special abilities -> ask inventory if start of special ability is possible (and also the built of a unit)
	Only ONE build start event per second!
	*/
	void manageSpecialAbilityAndBuildStart();

	/*
	Redistribute the workers (Workers can harvest minerals and if the corresponding requirements are fulfilled also at the Vespene Gas geysir)
	 -> Naive strategy at first: if vespene gas geysir enabled, send as much workers there as possible
	*/
	bool redistributeWorkers();

	/*
	create the Report -> collect information from Inventory and create message
	*/
	void createReport(bool);

	/*
	Let the GameObjects notify Simulator about events (build start, build end, special)
	The GameObjects create this event within their class und send it to Simulator
	The Simulator adds those events to a queueing map
	*/
	void notify(shared_ptr<Event> event);

	GameObjectManager* getObjectManager() const;

private:
	bool activateSpecialAbilities_;
	unique_ptr<Inventory> inventoryPtr_;
	unique_ptr<BuildList> buildListPtr_;
	unsigned int time_;
	vector<shared_ptr<Event>> eventList_;						// accumulate events in eventMap 
	shared_ptr<GameObjectManager> gameObjectManagerPtr_;		// factory to create new GameObjects
	unique_ptr<GameObject> nextGameObject_;
	Messenger messenger_;
	string finalReport_;										//report that is generated at the very end
	void accumulateBuildEndEvents();	
};