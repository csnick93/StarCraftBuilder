//Simulator.cpp
#include "Simulator.h"
#include "BaseBuilding.h"

/*
CONSTRUCTOR
*/
Simulator::Simulator(bool activateSpecial) : time_(1), activateSpecialAbilities_(activateSpecial),
													messenger_(Messenger(activateSpecial))
{
}


//=======================================================================================================================

void Simulator::create(shared_ptr<GameObjectManager> gameObjectManager, string path) {
	/*
	Instantiate the attributes of Simulator and precheck buildList.

	Args:
		gameObjectManager: Game Object Manager of Simulation (produces game objects)
		path: file path to the build list (rules for the game)
	*/
	gameObjectManagerPtr_ = gameObjectManager;
	buildListPtr_ = unique_ptr<BuildList>(new BuildList(gameObjectManagerPtr_));

	// 1) create inventory  
	inventoryPtr_ = unique_ptr<Inventory>(new Inventory());

	//read in build list: throw exception if there is a parsing error
	try {
		buildListPtr_->readInBuildList(path, this, inventoryPtr_.get());
	}
	catch (ParserException& e) {
		throw e;
	}

	// do precheck of plausibility of build list 
	if (!(buildListPtr_->doPreChecking())) {
		cout << messenger_.streamSimulatorOutput(gameObjectManagerPtr_->getRace(), false);
		BuildListException ble;
		throw ble;
	}

	// create initial objects
	vector<unique_ptr<GameObject>> objs;
	map<TypeID*, vector<ID>> initialObjects;
	for (auto &obj_TypeID : gameObjectManagerPtr_->getStartTypes()) {
		auto obj = gameObjectManagerPtr_->create(obj_TypeID, this, inventoryPtr_.get());
		obj->setIsBuilt();

		// if this is a worker -> make it collecting minerals
		auto objc = dynamic_cast<BaseUnit *>(&(*obj));
		if (objc != nullptr)
			objc->setActivity(BaseUnit::MINERALS);

		// add pair <TypeID,ID> to map initialUnits (needed for report in case of special abiltiies activated)
		if (activateSpecialAbilities_) {
			auto got = initialObjects.find(obj_TypeID);
			if (got == initialObjects.end()) {
				initialObjects.insert(make_pair(obj_TypeID, vector<ID>{obj->getID()}));
			}
			else {
				got->second.push_back(obj->getID());
			}
		}

		objs.push_back(move(obj));
	}

	

	if(activateSpecialAbilities_)
		messenger_.setInitialUnits(initialObjects);

	// initialize inventory
	inventoryPtr_->initialize(objs, Minerals(FixedPoint(50), FixedPoint(0)));

	// get first unit that needs to be built
	nextGameObject_ = buildListPtr_->getNextToBeBuiltObject();
}

//=======================================================================================================================
// alternative create method : for optimization project 

void Simulator::create(shared_ptr<GameObjectManager> gameObjectManager, BuildList& bl)
{
	gameObjectManagerPtr_ = gameObjectManager;
	

	// 1) create inventory  
	inventoryPtr_ = unique_ptr<Inventory>(new Inventory());

	// initialize buildList
	buildListPtr_ = unique_ptr<BuildList>(new BuildList(bl));
	buildListPtr_->instantiateForSimulation(this, inventoryPtr_.get());

	// do precheck of plausibility of build list 
	if (!(buildListPtr_->doPreChecking())) {
		cout << messenger_.streamSimulatorOutput(gameObjectManagerPtr_->getRace(), false);
		BuildListException ble;
		throw ble;
	}

	// create initial objects
	vector<unique_ptr<GameObject>> objs;
	map<TypeID*, vector<ID>> initialObjects;
	for (auto &obj_TypeID : gameObjectManagerPtr_->getStartTypes()) {
		auto obj = gameObjectManagerPtr_->create(obj_TypeID, this, inventoryPtr_.get());
		obj->setIsBuilt();

		// if this is a worker -> make it collecting minerals
		auto objc = dynamic_cast<BaseUnit *>(&(*obj));
		if (objc != nullptr)
			objc->setActivity(BaseUnit::MINERALS);

		// add pair <TypeID,ID> to map initialUnits (needed for report in case of special abiltiies activated)
		if (activateSpecialAbilities_) {
			auto got = initialObjects.find(obj_TypeID);
			if (got == initialObjects.end()) {
				initialObjects.insert(make_pair(obj_TypeID, vector<ID>{obj->getID()}));
			}
			else {
				got->second.push_back(obj->getID());
			}
		}

		objs.push_back(move(obj));
	}



	if (activateSpecialAbilities_)
		messenger_.setInitialUnits(initialObjects);

	// initialize inventory
	inventoryPtr_->initialize(objs, Minerals(FixedPoint(50), FixedPoint(0)));

	// get first unit that needs to be built
	nextGameObject_ = buildListPtr_->getNextToBeBuiltObject();
}


//=======================================================================================================================

tuple<bool, unsigned int,string> Simulator::run(unsigned int maxTimeSteps) {
	/*
	Method running complete simulation.

	Args:
		maxTimeSteps: the maximum amount of allowed time steps to build the given build list

	Returns:
		Tuple: (Flag if building build list was successful, timesteps to build it, the log report of the simulation)
	*/
	try {
		while (time_ <= maxTimeSteps) {
			D(cout << "---------------------------------------- BEGIN TIME STEP " << time_ << " ----------------------------------------" << endl);
			simulateOneTimeStep();
		}
		finalReport_ = messenger_.streamSimulatorOutput(gameObjectManagerPtr_->getRace(), false);
		return make_tuple(false, maxTimeSteps, finalReport_);
	}
	catch (SimulationDoneException& e) {
		D(cout << e.what() << endl;);
		return make_tuple(true, time_, finalReport_);
	}
}

//=======================================================================================================================

void Simulator::simulateOneTimeStep()
{
	/*
	Simulation of one timestep
	*/

	if (nextGameObject_.get() != nullptr) {
		D(cout << "Next GameObject to be built: " << *nextGameObject_ << endl;);
	}
	else {
		D(cout << "No more objects to be built, waiting for last objects to finish" << endl;);
	}
	
	// call the tick method -> inventory makes all GameObjects in the inventory call their tick method
	// in that tick method, the objects should do everything they need to do: 
	//	harvest the minerals/gas, decrease their buildTime, notify the simulator about any events ...
	inventoryPtr_->tick();

	//let inventory update itself (based on what changed during the tick method)
	updateResources();

	// check for finished builds (not really necessary since GameObjects notify Simulator automatically)
	checkFinishedBuilds();


	// check if nextGameObject_ can be built
	//  decide if special abilities should be started (only possible if no build-start has been initiated), 
	//		and see if any special abilities must be stopped (TODO: maybe add another Stop Special Ability Event?)
	manageSpecialAbilityAndBuildStart();

	// redistribute workers: choose between minerals and gas
	// assign workers per default to collecting minerals
	// if possible though, assign to collecting gas (depending on if Vespene geysir has open capacities)
	bool changedDistribution = redistributeWorkers();

	// create the report based on all the events that happened during the time step
	createReport(changedDistribution);

	if (nextGameObject_.get() == nullptr && !inventoryPtr_->hasObjectsUnderConstruction()) {
		finalReport_ = messenger_.streamSimulatorOutput(gameObjectManagerPtr_->getRace(), true);
		SimulationDoneException done;
		throw done;
	}
	++time_;
}

//=======================================================================================================================
void Simulator::updateResources() const{
	/*
	Update the resources in the inventory during the tick method.
	*/
	D(cout << "\n=====Updating the resources==== \n" << endl;);
	inventoryPtr_->updateMinerals();
	D(cout << "Inventory now contains: \n" << *inventoryPtr_ << endl);
}

//=======================================================================================================================

/*
This should happen automatically, as GameObjects should notify
Simulator if builds are finished
*/
void Simulator::checkFinishedBuilds() const
{
}

//=======================================================================================================================


void Simulator::manageSpecialAbilityAndBuildStart()
{
	/*
	1) Check if nextGameObject_ can be built. If so, build it and forbid start of any special abilities.
	2) Depending on the race, let units manage their special ability.
	*/
	D(cout << "\n=====Managing Special Abilities and the Build Start=====\n" << endl;);
	bool allowSpecialAbility;
	unique_ptr<GameObject>* nextGameObjectPtr_ = nullptr;

	//check if last object is currently being built (so nextGameObject_ == nullptr)
	if (nextGameObject_.get() == nullptr){
		D(cout << "No more builds started, as currently building last object" << endl;);
		allowSpecialAbility = true;
	}
	else{
		//1)
		try {
			// check if nextGameObject can be successfully added to inventory
			if (inventoryPtr_->has(nextGameObject_->supply())) {
				D(cout << "GameObject " << *nextGameObject_ << " is being added to inventory" << endl;);
				nextGameObjectPtr_ = inventoryPtr_->add(std::move(nextGameObject_));
			}
			else{
				throw std::logic_error("Adding Object to inventory did not work. Not enough supply.");
			}

			// check if morphing is required
			TypeID* morphUnit = (nextGameObjectPtr_->get())->getMorphedFromType();

			/*
			MORPH BUILD (look for source-morph object)
			*/
			if (morphUnit != nullptr) {
				D(cout << "Need " << morphUnit << " for building " << **nextGameObjectPtr_ << endl;);

				if (inventoryPtr_->has(morphUnit)) {
					// get gameObject candidates for morphing
					deque<unique_ptr<GameObject>> const &morph_Candidates = inventoryPtr_->getObjectsByTypeID(morphUnit);

					// make sure we have potential candidates
					if (morph_Candidates.size() > 0) {

						// check if it is base building type -> need to make sure it is in LAZY state
						if (dynamic_cast<BaseBuilding *>(morph_Candidates.front().get()) != nullptr) {
							bool foundMorphObject = false;
							for (auto &cand : morph_Candidates) {
								// if basebuilding && lazy -> start building/morphing
								if ((dynamic_cast<BaseBuilding *>(cand.get())->getActivity() == BaseBuilding::LAZY)) {
									D(cout << "Found " << *cand << " to morph from" << endl;);
									foundMorphObject = true;
									nextGameObjectPtr_->get()->build(cand);
									break;
								}
							}
							if (!foundMorphObject)
								throw std::logic_error("Objects to morph from are busy!");
						}

						// if not basebuilding, just start building/morphing
						else {
							D(cout << "Found " << *morph_Candidates.front() << " to morph from" << endl;);
							nextGameObjectPtr_->get()->build(morph_Candidates.front());
						}

					}
					// no candidates -->  cannot build
					else {
						throw std::logic_error("No object to be morphed from present!");
					}
				}
				else {
					throw std::logic_error("No object to be morphed from present!");
				}
			}
			/*
			NORMAL BUILD (WITHOUT MORPHING)
			*/
			else {
				D(cout << "Do normal build of (no morphing involved) " << **nextGameObjectPtr_ << endl;);
				nextGameObjectPtr_->get()->build();
			}
			/*
			BUILD WAS SUCCESSFUL
			*/
			D(cout << "Build was successfully started" << endl;);
			allowSpecialAbility = false;
			//build succesful, get next GameObject_
			nextGameObject_ = buildListPtr_->getNextToBeBuiltObject();
			//check if we are done with Simulation
			if (nextGameObject_.get() == nullptr) {
				D(cout << "No more objects to build. Waiting for last object to be built." << endl;);
			}
			if (nextGameObject_.get() != nullptr){
				D(cout << "The next object that needs to built is : " << *nextGameObject_ << endl;);
			}

		}
		/*
		BUILD FAILED
		*/
		catch (std::logic_error& e) {
			D(cout << "The build process failed: " << e.what() << endl;);
			// remove gameObject from inventory if it was successfully added, since it could not be built 
			if (nextGameObject_ == nullptr) {
				nextGameObject_ = inventoryPtr_->remove(*nextGameObjectPtr_);
			}
			// allow special ability start in this time step since build start was not successful
			allowSpecialAbility = true;
		}
	}

	/*
	MANAGE SPECIAL ABILITIES
	*/
	//2) manage special ability via GameObjectmanager
	if (activateSpecialAbilities_ && allowSpecialAbility) {
		D(cout << "Managing special abilities" << endl;);
		gameObjectManagerPtr_->manageSpecialAbilities(inventoryPtr_.get());
	}
}


//=======================================================================================================================
bool Simulator::redistributeWorkers() {
	/*
	Redistribute workers: choose between minerals and gas, and assign workers to those activities that are necessary to build nextGameObject_.
	1) if we do not have sufficient gas -> assign as many workers to gas as needed/possible
	2) otherwise: assign workers to minerals
	*/
	/*
	SMART IMPLEMENTATION OF REDISTRIBUTION
	*/
	bool changeDistribution = false;

	D(cout << "\n=====Redistributing the Workers=====\n" << endl;);

	// if currently building last gameObject, we do not care about redistribution anymore
	if (nextGameObject_.get() == nullptr){
		D(cout << "No redistribution necessary, currently building last object" << endl;);
		return changeDistribution;
	}

	//1) Check how many more units of gas is necessary to build nextGameObject_ (for reasonable assignment)

	FixedPoint necessaryGas_;
	FixedPoint necessaryMinerals_;
	if (nextGameObject_->requiredMinerals().gas() > inventoryPtr_->getNumberGas()) {
		necessaryGas_ = nextGameObject_->requiredMinerals().gas() - inventoryPtr_->getNumberGas();
	}
	if (nextGameObject_->requiredMinerals().minerals() > inventoryPtr_->getNumberMinerals()) {
		necessaryMinerals_ = nextGameObject_->requiredMinerals().minerals() - inventoryPtr_->getNumberMinerals();
	}
	
	// 2) Find out current worker distribution
	unsigned int gasHarvesters = inventoryPtr_->getNumberVespeneCollectors();
	unsigned int mineralHarvesters = inventoryPtr_->getNumberMineralCollectors();
	

    if (inventoryPtr_->has(gameObjectManagerPtr_->getBasicWorkerTypeID())) {
        // 1) Get all basic workers
        deque<unique_ptr<GameObject>> const &basic_workers = inventoryPtr_->getObjectsByTypeID(
                gameObjectManagerPtr_->getBasicWorkerTypeID());

        // 2) Get number of free spots in vespene geysirs
        unsigned int openSpots[] = {0, 0};
        deque<unique_ptr<GameObject>> const *geysirs = nullptr;
        if (inventoryPtr_->has(gameObjectManagerPtr_->getVespeneGeysirTypeID())) {
			D(cout << "There are geysirs present\n" << endl;);
            geysirs = &(inventoryPtr_->getObjectsByTypeID(
                    gameObjectManagerPtr_->getVespeneGeysirTypeID()));
            if (geysirs->size() == 2) {
                openSpots[0] = dynamic_cast<VespeneBuilding *>(geysirs->front().get())->getOpenSpots();
                openSpots[1] = dynamic_cast<VespeneBuilding *>(geysirs->back().get())->getOpenSpots();
				D(cout << "There are " << openSpots[0] + openSpots[1] << " spots open" << endl;);
            } else if (geysirs->size() == 1) {
                openSpots[0] = dynamic_cast<VespeneBuilding *>(geysirs->front().get())->getOpenSpots();
				D(cout << "There are " << openSpots[0] << " spots open" << endl;);
            }
        }

		unsigned int gasSpotsOpen;
        
		// 3) Reassign to following scheme:
		//	a) Worker that are occupied with OTHER are not reassigned
		//	b) Assign lazy workers per default to minerals
		//	c) If more gas necessary and not all open spots filled up -> assign non gas workers to gas
		//	d) If no more minerals necessary and not all open spots at geysir filled up -> fill up
		//	e) If more minerals necessary and no more gas -> assign gas workers to minerals
        
        for (auto &worker : basic_workers) {
			BaseUnit::Activity act = dynamic_cast<BaseUnit*>(worker.get())->getActivity();
			gasSpotsOpen = openSpots[0] + openSpots[1];
			//a)
			if (act == BaseUnit::OTHER) {
				continue;
			}
			//b)
			else if (act == BaseUnit::LAZY) {
				dynamic_cast<BaseUnit*>(worker.get())->setActivity(BaseUnit::MINERALS);
				changeDistribution = true;
			}
			//c) + d)
			else if (act == BaseUnit::MINERALS && ((necessaryGas_ > 0 && gasSpotsOpen > 0)||(necessaryMinerals_ <= 0 && gasSpotsOpen > 0))) {
				dynamic_cast<BaseUnit*>(worker.get())->setActivity(BaseUnit::GAS);
				if (openSpots[0] > 0) {
					--openSpots[0];
				}
				else {
					--openSpots[1];
				}
				changeDistribution = true;
			}
			//e)
			else if (act == BaseUnit::GAS && necessaryMinerals_ > 0 && necessaryGas_ <= 0) {
				dynamic_cast<BaseUnit*>(worker.get())->setActivity(BaseUnit::MINERALS);
				if (openSpots[0] > 0) {
					++openSpots[0];
				}
				else {
					++openSpots[1];
				}
				changeDistribution = true;
			}
			else {
				continue;
			}

		}
	} else {
        throw std::logic_error("No basic workers found in inventory. This makes no sense.");
    }
	return changeDistribution;
}

//=======================================================================================================================

void Simulator::createReport(bool changeWorkerDistribution) {
	/*
	Create report after each timestep.

	Args:
		changeWorkerDistribution: flag indicating whether the worker distribution has changed during the timestep
	*/

	D(cout << "\n=====Sending status report to messenger=====\n\n" << endl;);

	
	if (eventList_.size() > 0 || changeWorkerDistribution) {
		D(cout << "WORKER DISTRIBUTION:\nMinerals: "<< inventoryPtr_->getNumberMineralCollectors() << "\nGas: " << inventoryPtr_->getNumberVespeneCollectors() << endl;);

		//accumulate build end events in event list
		accumulateBuildEndEvents();

		Message m(time_,
			Status(
				Workers(inventoryPtr_->getNumberMineralCollectors(), inventoryPtr_->getNumberVespeneCollectors()),
				Resources(inventoryPtr_->getNumberMinerals(), inventoryPtr_->getNumberGas(),
					inventoryPtr_->getNumberSupplyUsed(), inventoryPtr_->getNumberAvailableSupply())),
			eventList_,
			activateSpecialAbilities_);

		messenger_.addMessage(m);

		//empty the event list, as we have streamed the events of the last timestep 
		eventList_.clear();
	}
	
	//debug
	D(cout << "\n\n" << *inventoryPtr_ << endl;);
	
}

//=======================================================================================================================


void Simulator::notify(shared_ptr<Event> eventPtr)
{
	/*
	Let GameObjects notify Simulator about any type of event.

	Args:
		eventPtr: pointer containing information about an event 
	*/
	eventList_.push_back(eventPtr);
}

//=======================================================================================================================

GameObjectManager * Simulator::getObjectManager() const
{
	return gameObjectManagerPtr_.get();
}


//=======================================================================================================================

void Simulator::accumulateBuildEndEvents()
{
	/*
	Accumulate BuildEndEvents by name_producerID as key.
	*/
	map <pair<TypeID*,ID>, vector<ID> > map;
	unsigned int counter = 0;
	vector<unsigned int> deleteIndex;	// only delete necessary build end events (those where a key already exists)
	for (auto it = eventList_.begin(); it < eventList_.end(); ++it) {
		auto event = dynamic_cast<BuildEndEvent *>(it->get());
		if (event != nullptr) {
			auto find = map.find(event->getAttributes());
			if (find == map.end()) {
				map.insert(make_pair(event->getAttributes(), vector<ID>{event->getProducedID()}));
			}
			// found a corresponding build end event -> remember spot to delete it later
			else {
				deleteIndex.push_back(counter);
				find->second.push_back(event->getProducedID());
			}

		}
		++counter;
	}

	// 1) delete superfluous events (those that get merged with a corresponding build end event)
	// 2) and create the merged build end events

	//1) 
	for (auto it = deleteIndex.rbegin(); it != deleteIndex.rend(); ++it) {
		eventList_.erase(eventList_.begin() + *it);
	}

	//2)
	for (auto it = map.begin(); it != map.end(); ++it) {
		if(it->second.size()>1)
			eventList_.push_back(make_shared<BuildEndEvent>(it->first.first, it->first.second, it->second));
	}

}