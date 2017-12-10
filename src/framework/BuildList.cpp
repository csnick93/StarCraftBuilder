#include "BuildList.h"
#include "DefinedExceptions.h"

using std::string;
using std::ifstream;
using std::shared_ptr;
using std::unordered_set;
using std::random_device;
using std::mt19937;
using std::uniform_int_distribution;

BuildList::BuildList(shared_ptr<GameObjectManager> gM): gameObjectManager_(gM)
{
}

/*
Optimization
*/
BuildList::BuildList(const BuildList & other, unsigned int stopIndex)
{
	if (stopIndex > other.getSizeVec())
	{
		stopIndex = other.getSizeVec();
	}
	gameObjectManager_ = other.gameObjectManager_;

	for (unsigned int i = 0; i < stopIndex; ++i) {
		buildListVec_.push_back(other.getElementTypeID(i));
	}
}


void BuildList::instantiateForSimulation(Simulator * s, Inventory * i)
{
	for (auto& tid : buildListVec_)
	{
		buildList_.push_back(gameObjectManager_->create(tid, s, i));
	}
}

GameObject * const BuildList::next() const
{
	return &(*buildList_.front());
}



unique_ptr<GameObject> BuildList::getNextToBeBuiltObject()
{
	//if there is no more GameObject to be produced, we are done!
	if (buildList_.empty()) {
		return unique_ptr<GameObject>(nullptr);
	}
	else {
		unique_ptr<GameObject> nextObj;
		nextObj = std::move(buildList_.front());
		buildList_.pop_front();
		return nextObj;
	}
}

//////////////////////////////////////////////////////

void BuildList::readInBuildList(string path, Simulator* s, Inventory* i)
{
	std::ifstream in_file(path);
	string unit;
	//read in file to buffer vector
	if (in_file.is_open()) {
		while (std::getline(in_file, unit)) {
			buildList_.push_back(gameObjectManager_->create(TypeID::getTypeID(unit),s,i));
		}
		in_file.close();
	}
	else {
		ParserException pe;
		throw pe;
	}

	D(std::cout << "This is the Build List we want to simulate: \n " << *this << std::endl;);
}

//////////////////////////////////////////////////////

bool BuildList::doPreChecking() const{
	/*
	Given a race, do a quick preCheck of the BuildList given
	the starting conditions of the race

	Approach: 
		- check if enabling Requirements are met
		- enabling dependencies, producedIn, morphedFrom
		- check that at least one VespeneGasGeysir has been built if a unit
		in the buildList requires gas
		- check sufficient supply
		- check that morphing objects actually have a morphedFrom object, since morphedFrom might already be used up

	Returns:
		Flag indicating if build list passed pre-check
	*/
	// make sure that build list is not empty
	if (buildList_.size() == 0) {
		return false;
	}

	/*
	check that at most two Vespene Buildings within Build List per base
	*/
	unsigned int vespene_counter = 0;
	unsigned int base_counter = 1;
	for (auto& obj : buildList_) {
		if (obj->typeID() == gameObjectManager_->getVespeneGeysirTypeID()) {
			++vespene_counter;
		}
		else if (obj->typeID() == gameObjectManager_->getBaseTypeID()) {
			++base_counter;
		}
	}
	if (vespene_counter > 2*base_counter) {
		return false;
	}

	/*
	Before going through BuildList in detail: accumulate supply and make sure it is greater or equal to 0
	*/
	Supply initialSupply = gameObjectManager_->initialSupply();
	
	for (auto& obj : buildList_) {
		initialSupply += obj->supply();
	}

	if (initialSupply < 0) {
		return false;
	}
	

	// save the existing types in the vector
	// to be able to check if enableRequirements are met
	unordered_map<TypeID*, unsigned int> existingTypes;

	// enter starting conditions
	for (auto& tid : gameObjectManager_->getUniqueStartTypes()) {
		existingTypes.insert(tid);
	}


	/*
	Go through BuildList object by object and check:
		- preconditions_ (only one of the objects listed there must exist)
		- producedIn_ (only one of the objects listed there must exist)
		- morphedFrom 
		- if VespeneGeysir exists depending on whether the object requires Gas
	*/
	for (auto& obj : buildList_) {
		// flag that is set to false before checking each condition
		// 1) check preconditions
		bool valid = false;
		if (obj->preconditions().size() == 0) {
			valid = true;
		}
		else {
			for (auto& req : obj->preconditions()) {
				auto foundIt = existingTypes.find(req);
				// if we found it -> meeting precondition requirement
				if (foundIt != existingTypes.end()) {
					valid = true;
					break;
				}
			}
		}
		if (!valid) {
			return false;
		}

		// 2) check producedIn_
        valid = false;
		if (obj->producedIn().size() == 0) {
			valid = true;
		}
		else {
			for (auto& req : obj->producedIn()) {
				auto foundIt = existingTypes.find(req);
				// if we found it -> meeting precondition requirement
				if (foundIt != existingTypes.end()) {
					valid = true;
					break;
				}
			}
		}
		if (!valid) {
			return false;
		}

		// 3) check morphedFrom_
        valid = false;
		if (obj->getMorphedFromType() == nullptr) {
			valid = true;
		}
		else {
			auto foundIt = existingTypes.find(obj->getMorphedFromType());
			// if we found it and we still have objects of that type left -> meeting precondition requirement
			if (foundIt != existingTypes.end() && foundIt->second > 0) {
				valid = true;
			}
		}
		if (!valid) {
			return false;
		}

		// 4) Check existence of VespeneGeysir if gas required
        valid = false;
		if ((obj->requiredMinerals()).gas().getWholeUnits() > 0) {
			auto foundIt = existingTypes.find(gameObjectManager_->getVespeneGeysirTypeID());
			if (foundIt != existingTypes.end()) {
				valid = true;
			}
		} else {
			valid = true;
		}
		if (!valid) {
			return false;
		}

		/*
		all requirements met -> can be built -> add to existingTypes 
		 but check if we need to modify counter of a morphedFrom object in case obj is morphing
		*/
		//adjust morphedFrom if necessary
		if (obj->getMorphedFromType() != nullptr)
		{
			auto morphedFrom = existingTypes.find(obj->getMorphedFromType());
			--(morphedFrom->second);
		}
		
		// look for current object type, if it is already existant
		auto objectIt = existingTypes.find(obj->typeID());
		if (objectIt != existingTypes.end())
		{
			++(objectIt->second);
		}
		else
		{
			existingTypes.insert(make_pair(obj->typeID(),1));
		}
		
	}
	return true;
}



///////////////////////////
// OPTIMIZATION METHODS //
//////////////////////////

BuildList BuildList::crossover(const BuildList &other, mt19937 &rng) const
{
	BuildList child(gameObjectManager_);

	/*
	Make sure build lists have the same length -> should have happened when computing distances to each other
	*/
	if (getSizeVec() != other.getSizeVec()) {
		D(std::cout << "Non matching sizes" << std::endl;);
		throw std::logic_error("Build lists need to have the same size for reproduction. Use BuildList::fillUp before.");
	}
	
	/*
	Do crossover
	*/
	uniform_int_distribution<int> uni(0, 1);
	
	for (unsigned int i = 0; i < getSizeVec(); ++i) 
	{
		// random choice which build list to choose from
		int index = uni(rng);

		// choosing *this build list
		if (index == 0)
		{
			// valid choice -> add to child
			if (child.isValidNextObject(getElementTypeID(i)))
			{
				child.addToBuildList(getElementTypeID(i));
			}

			//not a valid choice -> try other build list
			else
			{
				if (child.isValidNextObject(other.getElementTypeID(i)))
				{
					child.addToBuildList(other.getElementTypeID(i));
				}
				
				// we cannot create valid child anymore -> throw exception
				else
				{
					throw std::logic_error("Cannot create valid child!");
				}
			}
		}

		// choosing other build list
		else
		{
			// valid choice -> add to child
			if (child.isValidNextObject(other.getElementTypeID(i)))
			{
				child.addToBuildList(other.getElementTypeID(i));
			}

			//not a valid choice -> try this build list
			else
			{
				if (child.isValidNextObject(getElementTypeID(i)))
				{
					child.addToBuildList(getElementTypeID(i));
				}

				// we cannot create valid child anymore -> throw exception
				else
				{
					throw std::logic_error("Cannot create valid child!");
				}
			}
		}
	}
	
	return child;
}

//===============================================================================

BuildList BuildList::mutate(float mutationRate, mt19937& rng) const
{
	if (mutationRate < 0 || mutationRate > 1)
	{
		D(std::cout << "Invalid mutation rate" << std::endl;);
		throw std::logic_error("Invalid mutation rate");
	}

	// create copy
	BuildList mutation(*this);

	/*
	compute number of elements to mutate
	*/
	unsigned int numMutations = floor(getSizeVec()*mutationRate);

	/*
	Delete numMutations many random elements
	*/
	set<unsigned int> deleteIndices = getRandomDistinctIndices(mutation.getSizeVec(), numMutations, rng);

	for (auto rit = deleteIndices.rbegin(); rit != deleteIndices.rend(); ++rit)
	{
		mutation.removeUnit(*rit);
	}
	
	/*
	Insert numMutations many random elements
	*/
	vector<unsigned int> insertIndices = getRandomIndices(mutation.getSizeVec(), numMutations, rng);

	for (auto index : insertIndices)
	{
		// find valid unit and insert it
		BuildList tmp(mutation, index);
		auto state = tmp.getBuildListState();
		try
		{
			mutation.insertUnit(index, tmp.getNextValidObject(get<0>(state), get<1>(state), get<2>(state), get<3>(state), get<4>(state), rng));
		}
		catch (std::logic_error& e)
		{
			throw e;
		}
	}

	return mutation;
}

//===================================================================

BuildList BuildList::strategicMutate(float mutationRate,bool strategy, TypeID* targetObj, mt19937& rng) const
{
	if (mutationRate < 0 || mutationRate > 1)
	{
		D(std::cout << "Invalid mutation rate" << std::endl;);
		throw std::logic_error("Invalid mutation rate");
	}

	// create copy
	BuildList mutation(*this);

	/*
	compute number of elements to mutate
	*/
	unsigned int numMutations = floor(getSizeVec()*mutationRate);

	/*
	Delete numMutations many random elements
	*/
	set<unsigned int> deleteIndices = getRandomDistinctIndices(mutation.getSizeVec(), numMutations, rng);

	for (auto rit = deleteIndices.rbegin(); rit != deleteIndices.rend(); ++rit)
	{
		mutation.removeUnit(*rit);
	}

	/*
	Insert numMutations many random elements
	*/
	vector<unsigned int> insertIndices = getRandomIndices(mutation.getSizeVec(), numMutations, rng);

	for (auto index : insertIndices)
	{
		// find valid unit and insert it
		BuildList tmp(mutation, index);
		auto state = tmp.getBuildListState();
		try
		{
			mutation.insertUnit(index, tmp.getNextStrategicObject(get<0>(state), get<1>(state), get<2>(state), get<3>(state), get<4>(state), strategy, targetObj,rng));
		}
		catch (std::logic_error& e)
		{
			throw e;
		}
	}

	return mutation;
}

//===============================================================================

BuildList BuildList::getRandomBuildList(unsigned int length, shared_ptr<GameObjectManager> gm, mt19937& rng)
{
	BuildList buildList(gm);

	tuple<vector<TypeID*>, Supply, unordered_map<TypeID*, unsigned int>, unsigned int, unsigned int> bl_state = buildList.getBuildListState();

	for (unsigned int i = 0; i < length; ++i) {
		try
		{
			buildList.addToBuildList(buildList.getNextValidObject(get<0>(bl_state), get<1>(bl_state), get<2>(bl_state), get<3>(bl_state), get<4>(bl_state),rng));
		}
		catch (std::logic_error& e)
		{
			throw e;
		}
	}

	return buildList;
}


//===============================================================================

BuildList BuildList::getStrategicRandomBuildList(unsigned int length, bool push, TypeID* targetObj,shared_ptr<GameObjectManager> gm, mt19937& rng)
{
	BuildList buildList(gm);

	tuple<vector<TypeID*>, Supply, unordered_map<TypeID*, unsigned int>, unsigned int, unsigned int> bl_state = buildList.getBuildListState();

	/*
	Insert length-many units in case of rush strategy
	Insert at most length-many units in case of push strategy
	*/
	
	for (unsigned int i = 0; i < length; ++i) {
		try 
		{
			buildList.addToBuildList(buildList.getNextStrategicObject(get<0>(bl_state), get<1>(bl_state), get<2>(bl_state), get<3>(bl_state), get<4>(bl_state), push, targetObj,rng));
			
			// in case of push strategy : check if it was possible to add targetObj -> break
			if (push && buildList.getBuildList().back() == targetObj)
				break;
		}
		catch (std::logic_error& e)
		{
			throw e;
		}
	}

	// in case of push strategy: check if last element is targetObj, throw exception otherwise
	/*
	if (push && buildList.getBuildList().back() != targetObj)
	{
		throw std::logic_error("Could not create useful push build list.");
	}
	*/
	return buildList;
}

//===============================================================================


TypeID* BuildList::getNextStrategicObject(vector<TypeID*>& allTypeIDs, Supply& availableSupply, unordered_map<TypeID*, unsigned int>& existingTypes,
	unsigned int& baseCounter, unsigned int& geysirCounter, bool push, TypeID* targetObj, mt19937& rng)
{

	vector<TypeID*> validObjs;

	// check all units for validity
	for (auto& tid : allTypeIDs) {
		// get GameObjectInfo

		auto objInfo = gameObjectManager_->getGameObjectInfo(tid);

		// flag that is set to false before checking each condition
		bool valid = false;

		//1) Check if obj is refinery and if so if refinery is allowed
		if (tid != gameObjectManager_->getVespeneGeysirTypeID())
		{
			valid = true;
		}
		else if (tid == gameObjectManager_->getVespeneGeysirTypeID() && (geysirCounter < 2 * baseCounter))
		{
			valid = true;
		}
		if (!valid)
		{
			continue;
		}

		//2) Check if supply is sufficient
		valid = false;
		if (-get<2>(objInfo) <= availableSupply)
		{
			valid = true;
		}
		if (!valid)
		{
			continue;
		}

		// 3) check preconditions
		valid = false;
		if (get<1>(objInfo).size() == 0)
		{
			valid = true;
		}
		else
		{
			for (auto& req : get<1>(objInfo))
			{
				auto foundIt = existingTypes.find(req);
				// if we found it -> meeting precondition requirement
				if (foundIt != existingTypes.end())
				{
					valid = true;
					break;
				}
			}
		}
		//
		if (!valid)
		{
			continue;
		}

		// 4) check producedIn_
		valid = false;
		if (get<0>(objInfo).size() == 0)
		{
			valid = true;
		}
		else {
			for (auto& req : get<0>(objInfo))
			{
				auto foundIt = existingTypes.find(req);
				// if we found it -> meeting precondition requirement
				if (foundIt != existingTypes.end())
				{
					valid = true;
					break;
				}
			}
		}
		if (!valid)
		{
			continue;
		}

		// 5) check morphedFrom_
		valid = false;
		if (get<3>(objInfo) == nullptr)
		{
			valid = true;
		}
		else {
			auto foundIt = existingTypes.find(get<3>(objInfo));
			// if we found it and at least one object left for morphing -> meeting precondition requirement
			if (foundIt != existingTypes.end() && foundIt->second >0)
			{
				valid = true;
			}
		}
		if (!valid)
		{
			continue;
		}

		// 6) Check existence of VespeneGeysir if gas required
		valid = false;
		if (!(get<4>(objInfo).gas().getWholeUnits() > 0 && geysirCounter == 0))
		{
			valid = true;
		}
		else
		{
			valid = false;
		}

		if (valid)
		{
			validObjs.push_back(tid);
		}
	}

	if (validObjs.size() == 0)
	{
		throw std::logic_error("Cannot find valid object");
	}

	// choose object to build
	TypeID* chosenObj = nullptr;

	/*
	Check if targetObj is in validObjs
	*/
	auto desiredObj = std::find(validObjs.begin(), validObjs.end(), targetObj);

	if (desiredObj != validObjs.end())
	{
		chosenObj = *desiredObj;
	}
	else
	{
		/*
		Randomly choose object
		*/
		uniform_int_distribution<int> dist(0, validObjs.size() - 1);
		int index = dist(rng);
		chosenObj = validObjs[index];
	}
	

	auto chosenObjInfo = gameObjectManager_->getGameObjectInfo(chosenObj);

	/*
	adjust variables
	*/
	availableSupply += get<2>(chosenObjInfo);

	auto objIt = existingTypes.find(chosenObj);
	if (objIt != existingTypes.end())
	{
		++(objIt->second);
	}
	else
	{
		existingTypes.insert(make_pair(chosenObj, 1));
	}

	// check if a morphedFrom object has to be modified
	if (get<3>(chosenObjInfo) != nullptr)
	{
		auto morphedIt = existingTypes.find(get<3>(chosenObjInfo));
		--(morphedIt->second);
	}


	if (gameObjectManager_->getBaseTypeID() == chosenObj)
		++baseCounter;

	if (gameObjectManager_->getVespeneGeysirTypeID() == chosenObj)
		++geysirCounter;

	return chosenObj;
}

//===============================================================================


TypeID* BuildList::getNextValidObject(vector<TypeID*>& allTypeIDs, Supply& availableSupply, unordered_map<TypeID*,unsigned int>& existingTypes, 
										unsigned int& baseCounter, unsigned int& geysirCounter, mt19937& rng)
{

	vector<TypeID*> validObjs;

	// check all units for validity
	for (auto& tid : allTypeIDs) {
		// get GameObjectInfo
		
		auto objInfo = gameObjectManager_->getGameObjectInfo(tid);
		
		// flag that is set to false before checking each condition
		bool valid = false;

		//1) Check if obj is refinery and if so if refinery is allowed
		if (tid != gameObjectManager_->getVespeneGeysirTypeID()) 
		{
			valid = true;
		}
		else if (tid == gameObjectManager_->getVespeneGeysirTypeID() && (geysirCounter < 2 * baseCounter)) 
		{
			valid = true;
		}
		if (!valid) 
		{
			continue;
		}

		//2) Check if supply is sufficient
		valid = false;
		if (-get<2>(objInfo) <= availableSupply)
		{
			valid = true;
		}
		if (!valid) 
		{
			continue;
		}

		// 3) check preconditions
		valid = false;
		if (get<1>(objInfo).size() == 0)
		{
			valid = true;
		}
		else 
		{
			for (auto& req : get<1>(objInfo))
			{
				auto foundIt = existingTypes.find(req);
				// if we found it -> meeting precondition requirement
				if (foundIt != existingTypes.end()) 
				{
					valid = true;
					break;
				}
			}
		}
		//
		if (!valid) 
		{
			continue;
		}

		// 4) check producedIn_
		valid = false;
		if (get<0>(objInfo).size() == 0)
		{
			valid = true;
		}
		else {
			for (auto& req : get<0>(objInfo))
			{
				auto foundIt = existingTypes.find(req);
				// if we found it -> meeting precondition requirement
				if (foundIt != existingTypes.end())
				{
					valid = true;
					break;
				}
			}
		}
		if (!valid) 
		{
			continue;
		}

		// 5) check morphedFrom_
		valid = false;
		if (get<3>(objInfo) == nullptr)
		{
			valid = true;
		}
		else {
			auto foundIt = existingTypes.find(get<3>(objInfo));
			// if we found it and at least one object left for morphing -> meeting precondition requirement
			if (foundIt != existingTypes.end() && foundIt->second >0) 
			{
				valid = true;
			}
		}
		if (!valid) 
		{
			continue;
		}

		// 6) Check existence of VespeneGeysir if gas required
		valid = false;
		if (!(get<4>(objInfo).gas().getWholeUnits() > 0 && geysirCounter == 0))
		{
			valid = true;
		}
		else 
		{
			valid = false;
		}
		
		if (valid) 
		{
			validObjs.push_back(tid);
		}
	}

	if (validObjs.size() == 0) 
	{
		throw std::logic_error("Cannot find valid object");
	}

	/*
	Randomly choose object
	*/
	uniform_int_distribution<int> dist(0, validObjs.size()-1);
	
	int index = dist(rng);

	auto chosenObjInfo = gameObjectManager_->getGameObjectInfo(validObjs[index]);

	/*
	adjust variables
	*/
	availableSupply += get<2>(chosenObjInfo);

	auto objIt = existingTypes.find(validObjs[index]);
	if (objIt != existingTypes.end())
	{
		++(objIt->second);
	}
	else
	{
		existingTypes.insert(make_pair(validObjs[index],1));
	}

	// check if a morphedFrom object has to be modified
	if (get<3>(chosenObjInfo) != nullptr)
	{
		auto morphedIt = existingTypes.find(get<3>(chosenObjInfo));
		--(morphedIt->second);
	}

	
	if (gameObjectManager_->getBaseTypeID() == validObjs[index])
		++baseCounter;

	if (gameObjectManager_->getVespeneGeysirTypeID() == validObjs[index])
		++geysirCounter;

	return validObjs[index];
}

//===============================================================================

bool BuildList::isValidNextObject(TypeID* nextObj) {
	/*
	Get build list state
	*/
	
	tuple<vector<TypeID*>, Supply, unordered_map<TypeID*,unsigned int>, unsigned int, unsigned int> bl_state = getBuildListState();
	Supply availableSupply = get<1>(bl_state);
	unordered_map<TypeID*,unsigned int> existingTypes = get<2>(bl_state);
	unsigned int base_counter = get<3>(bl_state);
	unsigned int geysir_counter = get<4>(bl_state);


	auto objInfo = gameObjectManager_->getGameObjectInfo(nextObj);


	/*
	Get count on bases and geysirs in current build list if nextObj is a geysir
	*/
	if (nextObj == gameObjectManager_->getVespeneGeysirTypeID())
	{
		// if addition of nextObj as geysir breaks base/geysir ratio ->false
		if ((geysir_counter + 1) > base_counter*2) 
		{
			return false;
		}
	}

	/*
	Check supply if nextObj is consumer
	*/
	if (get<2>(objInfo) < 0)
	{
		if (-get<2>(objInfo) > availableSupply) {
			return false;
		}
	}

	/*
	Check the hard requirements for object: preconditions, producedIn, morphedFrom
	*/

	// 1) check preconditions
	bool valid = false;
	if (get<1>(objInfo).size() == 0) {
		valid = true;
	}
	else {
		for (auto& req : get<1>(objInfo)) {
			auto foundIt = existingTypes.find(req);
			// if we found it -> meeting precondition requirement
			if (foundIt != existingTypes.end()) {
				valid = true;
				break;
			}
		}
	}
	if (!valid) {
		return false;
	}

	// 2) check producedIn_
	valid = false;
	if (get<0>(objInfo).size() == 0) {
		valid = true;
	}
	else {
		for (auto& req : get<0>(objInfo)) {
			auto foundIt = existingTypes.find(req);
			// if we found it -> meeting precondition requirement
			if (foundIt != existingTypes.end()) {
				valid = true;
				break;
			}
		}
	}
	if (!valid) {
		return false;
	}

	// 3) check morphedFrom_
	valid = false;
	if (get<3>(objInfo) == nullptr) {
		valid = true;
	}
	else {
		auto foundIt = existingTypes.find(get<3>(objInfo));
		// if there is a morphedFrom object with at least 1 existing object -> meeting precondition requirement
		if (foundIt != existingTypes.end() && foundIt->second > 0) {
			valid = true;
		}
	}
	

	return valid;
	
}

//===============================================================================

void BuildList::insertUnit(unsigned int index, TypeID* tid)
{
	assert(index < getSizeVec());
	buildListVec_.insert(buildListVec_.begin() + index, tid);
}

//===============================================================================

void BuildList::removeUnit(unsigned int index)
{
	assert(index < getSizeVec());
	buildListVec_.erase(buildListVec_.begin() + index);
}

//===============================================================================

unsigned int BuildList::distance(const BuildList& other) const {
	if (getSizeVec() != other.getSizeVec()) {
		throw std::logic_error("Build lists need to have the same length.");
	}
	unsigned int dist = 0;
	unsigned int l = getSizeVec();
	for (unsigned int i = 0; i < l; ++i) {
		dist += (l - i)*(getElementTypeID(i) == other.getElementTypeID(i));
	}
	return dist;
}

//===============================================================================

void BuildList::fillUp(unsigned int length, mt19937& rng)
{
	tuple<vector<TypeID*>, Supply, unordered_map<TypeID*,unsigned int>, unsigned int, unsigned int> bl_state = getBuildListState();

	for (unsigned int i = getSizeVec(); i < length; ++i) {
		try
		{
			addToBuildList(getNextValidObject(get<0>(bl_state), get<1>(bl_state), get<2>(bl_state), get<3>(bl_state), get<4>(bl_state), rng));
		}
		catch(std::logic_error& e)
		{
			throw e;
		}
	}
}

//===============================================================================

unsigned int BuildList::getOccurrences(const TypeID* tid) const {
	unsigned int counter = 0;
	for (auto& obj : buildListVec_) {
		if (obj == tid) {
			++counter;
		}
	}
	return counter;
}

//===============================================================================

void BuildList::addToBuildList(TypeID *tid)
{
	buildListVec_.push_back(tid);
}

//===============================================================================

TypeID * BuildList::getElementTypeID(unsigned int i) const
{
	assert(i < buildListVec_.size());
	return buildListVec_[i];
}

//===============================================================================

tuple<vector<TypeID*>, Supply, unordered_map<TypeID*,unsigned int>, unsigned int, unsigned int> BuildList::getBuildListState() const
{
	/*
	NOTE: WE IMPLICITLY ASSUME START CONFIGURATIONS
	*/

	/*
	Initialize counters and attributes
	*/
	unsigned int geysir_counter = 0;
	unsigned int base_counter = 1;
	unordered_map<TypeID*, unsigned int> existingTypes;		// TypeID and number of occurrences of that TypeID in build list
	vector<TypeID*> allTypeIDs = gameObjectManager_->getAllTypeIDs();
	Supply availableSupply = gameObjectManager_->initialSupply();


	/*
	enter starting conditions
	*/
	for (auto& tid : gameObjectManager_->getUniqueStartTypes()) {
		existingTypes.insert(tid);
	}

	/*
	enter rest of created build list, calculate supply, count bases, and count geysirs
	*/
	for (auto& tid : buildListVec_)
	{
		auto objInfo = gameObjectManager_->getGameObjectInfo(tid);

		auto objIt = existingTypes.find(tid);
		if (objIt != existingTypes.end())
		{
			++(objIt->second);
			//check morphedFrom condition
			if (get<3>(objInfo) != nullptr)
			{
				auto morphIt = existingTypes.find(get<3>(objInfo));
				--(morphIt->second);
			}
		}
		else
		{
			existingTypes.insert(make_pair(tid,1));
		}
		
		availableSupply += get<2>(objInfo);

		if (gameObjectManager_->getBaseTypeID() == tid)
		{
			++base_counter;
		}
		else if (gameObjectManager_->getVespeneGeysirTypeID() == tid)
		{
			++geysir_counter;
		}
	}

	return make_tuple(allTypeIDs, availableSupply, existingTypes, base_counter, geysir_counter);
}

//===============================================================================

set<unsigned int> BuildList::getRandomDistinctIndices(unsigned int range, unsigned int quantity, mt19937 & rng) const
{
	set<unsigned int> indices;
	uniform_int_distribution<int> dist(0, range - 1);

	while (indices.size() < quantity)
	{
		unsigned int index = dist(rng);
		indices.insert(index);
	}

	return indices;
}

vector<unsigned int> BuildList::getRandomIndices(unsigned int range, unsigned int quantity, mt19937 & rng) const
{
	vector<unsigned int> indices;
	uniform_int_distribution<int> dist(0, range - 1);

	for (unsigned int i = 0; i < quantity; ++i)
	{
		unsigned int index = dist(rng);
		indices.push_back(index);
	}

	return indices;
}



/////////////////////////////////////////////////////////////////////////////////////////
ostream &operator<<(ostream &os, const BuildList &b) {
	for (auto& obj : b.buildListVec_) {
		std::cout << obj << std::endl;
	}
	std::cout << "\n" << std::endl;
	return os;
}


bool BuildList::operator<(const BuildList &rhs) const {
	/**
	* If lhs is shorter than rhs, returns true, otherwise false.
	* If they are of equal length, traverses both build lists.
	* A soon as a non-equal GO is found returns
	* 		a) true, if left-hand GO has lower adress
	* 		b) false, otherwise.
	*/
    if (buildListVec_.size() != rhs.buildListVec_.size())
        return buildListVec_.size() < rhs.buildListVec_.size();

    auto itr = rhs.buildListVec_.begin();
    for (auto litem: buildListVec_) {
        auto ritem = *itr;
        if (litem != ritem)
            return litem < ritem;

        ++itr;
    }
}

unsigned BuildList::getWorkerCount() const {
    return getOccurrences(gameObjectManager_->getBasicWorkerTypeID());
}

