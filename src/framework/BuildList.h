//BuildList.h
#pragma once

#include "GameObject.h"
#include "Types.h"
#include "TypeID.h"
#include "GameObjectManager.h"
#include "VespeneBuilding.h"
#include <iostream>
#include <string>
#include <exception>
#include <fstream>
#include <math.h>
#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <random>
#include <chrono>
#include <tuple>
#include <set>
#include <algorithm>
#include "Debug.h"

using std::tuple;
using std::deque;
using std::string;
using std::unordered_map;
using std::unordered_set;
using std::unique_ptr;
using std::shared_ptr;
using std::mt19937;
using std::get;
using std::set;



/*
In order to initialize a build list object:
	call default constructor
	call readInBuildList to fill up attribute buildList_
*/



class BuildList {
public:
	//default constructor
	BuildList(shared_ptr<GameObjectManager> gM);

	// RULE OF FIVE! ..
	BuildList(const BuildList& other)
            : gameObjectManager_(other.gameObjectManager_), buildListVec_(other.buildListVec_) {
        //buildList_ = deque<unique_ptr<GameObject>>();
    }

    BuildList(BuildList &&other)
            : gameObjectManager_(move(other.gameObjectManager_)), buildListVec_(move(other.buildListVec_)) { }

    BuildList &operator=(const BuildList& other) {
        gameObjectManager_ = other.gameObjectManager_;
        buildListVec_ = other.buildListVec_;
		return *this;
    }

    BuildList &operator=(BuildList&& other) {
        gameObjectManager_ = move(other.gameObjectManager_);
        buildListVec_ = move(other.buildListVec_);
		return *this;
    }

    ~BuildList() = default;

    // Copy constructor until stopIndex
	BuildList(const BuildList& other, unsigned int stopIndex);

	// instantiate buildList_ with GameObjects form buildListVec_ to do simulation
	void instantiateForSimulation(Simulator* s, Inventory* i);

	//////////////
	// METHODS //
	/////////////

	/*
	peek at what GameObject comes next in the queue
	*/
	GameObject* const next() const;


	/*
	Get a shared_ptr to the GameObject that needs to be built next
	*/
	unique_ptr<GameObject> getNextToBeBuiltObject();			

	/*
	Read in Build List
	@ path: location where build list lies in textform
	@ map: GameObjectMap of Simulator, which is necessary to look up attributes when creating GameObjects
	*/
	void readInBuildList(const string path, Simulator* s, Inventory* i);		

	/*
	Check before simulation if build list roughly makes sense
	*/
	bool doPreChecking() const;
	
	/*
	Get the number of Units that still need to be built
	*/
	unsigned int getSize() const{
		return buildList_.size();
	}

	//==================================================================================================================

	///////////////////////////////////
	// METHODS FOR OPTIMIZATION TASK //
	///////////////////////////////////
	/*
	Cross this build list with other buildList

	Throws exception if crossover fails
	*/
	BuildList crossover(const BuildList &other, mt19937 &rng) const;

	/*
	Mutation of a buildList according to a mutationRate
		deletion of num randomly chosen elements
		insertion of num valid elements at random positions

	Throws exception if mutation fails
	*/
	BuildList mutate(float mutationRate, mt19937& rng) const;

	/*
	Mutation under consideration of objective (try to incorporate targetObj)
	*/
	BuildList strategicMutate(float mutationRate, bool strategy, TypeID* targetObj, mt19937& rng) const;

	/*
	Generate a random buildList with certain length
		NOTE: START CONFIGURATION IS IMPLICITLY INCLUDED

	@length: (maximum) length of buildlist to be generated 
	@strategy: push strategy if true,  or rush (false)
	@targetObj: the unit we want to rush/push
	
	Push strategy: Randomly create build list, but as soon as build of targetObj is possible, stop (at maximum length many elements in build list)
	Rush strategy: Randomly create build list, and if to be rushed object is possible, take it (alternative idea: only give it higher probability to be chosen)

	Throws exception if generation of random build list fails
	(Throws exception in case of push strategy if unit could not be included within given maximum length)
	*/
	static BuildList getStrategicRandomBuildList(unsigned int length, bool strategy, TypeID* targetObj,shared_ptr<GameObjectManager> gm, mt19937& rng);


	/*
	Get random build list of certain length without considering objective
	*/
	static BuildList getRandomBuildList(unsigned int length, shared_ptr<GameObjectManager> gm, mt19937& rng);


	/*
	Compute weighted Hamming distance to other build list. 
	IT IS EXPECTED THAT THE BUILDLISTS HAVE THE SAME LENGTH,
	since they will need the same length when reproducing.
	Method is slightly modified to paper, such that last gene gets weighted as well:
	d = sum_{i=0}^{l-1}(l-i)(a_i - b_i)
	*/
	unsigned int distance(const BuildList& other) const;

	/*
	Fill up build list to certain length (to be able to do reproduction and before that distance computation)

	Throws exception if process of filling up fails
	*/
	void fillUp(unsigned int length, mt19937& rng);


	/*
	Count the number of occurrences of certain object in build list
	*/
	unsigned int getOccurrences(const TypeID*) const;

	/**
	 * Allow BuildLists to be put into a map (as keys)
	 */
	bool operator<(const BuildList &rhs) const;

	/**
     * Get number of workers in build list
     */
	unsigned getWorkerCount() const;

	//////////////////////////////
	// END OPTIMIZATION METHODS //
	//////////////////////////////
	//==================================================================================================================

	/**
	* Streams status information like minerals, gas, supply, gameobjects..
	*/
	friend ostream &operator<<(ostream &, const BuildList &);
private:
	/////////////////
	// ATTRIBUTES //
	////////////////
	/*
	use shared pointer for buildList entries to be able to copy buildList
	*/
	deque<unique_ptr<GameObject>> buildList_;	// used for actual simulation
	vector<TypeID*> buildListVec_;				// for optimization purposes
	shared_ptr<GameObjectManager> gameObjectManager_;
	
	/////////////
	// HELPERS //
	/////////////
	/*
	Add a gameObject to Build list (helper function)
	*/
	void addToBuildList(TypeID*);

	/*
	Check if proposed next object is valid based on current build list
	*/
	bool isValidNextObject(TypeID*);


	
	/*
	Get the Size of the BuildListVec_
	*/
	unsigned int getSizeVec() const
	{
		return buildListVec_.size();
	}

	/*
	Get const reference to buildList
	*/
	const vector<TypeID*>& getBuildList() const {
		return buildListVec_;
	}

	/*
	insert a random valid unit at index in build list
	*/
	void insertUnit(unsigned int index, TypeID* tid);

	/*
	remove unit at index in build list
	*/
	void removeUnit(unsigned int index);
	
	
	/*
	Get next valid object given current build list
	*/
	TypeID* getNextValidObject(vector<TypeID*>& allTypeIDs, Supply& availableSupply, unordered_map<TypeID*,unsigned int>& existingTypes, unsigned int& baseCounter, unsigned int& geysirCounter, mt19937& rng);

	/*
	Get next valid object given current build list, following a certain strategy
	*/
	TypeID* getNextStrategicObject(vector<TypeID*>& allTypeIDs, Supply& availableSupply, unordered_map<TypeID*, unsigned int>& existingTypes, 
									unsigned int& baseCounter, unsigned int& geysirCounter, bool strategy, TypeID* targetObj, mt19937& rng);

	/*
	Get TypeID of i-th unit in buildList -> helper function
	*/
	TypeID* getElementTypeID(unsigned int i) const;

	/*
	Get necessary information about build list to generate new objects: {allTypeIDs, availableSupply, existingTypes, base_counter, geysir_counter}
	*/
	tuple<vector<TypeID*>, Supply, unordered_map<TypeID*,unsigned int>, unsigned int, unsigned int> getBuildListState() const;


	/*
	Get quantity many distinct random numbers in the range [0,range) in increasing order
	*/
	set<unsigned int> getRandomDistinctIndices(unsigned int range, unsigned int quantity, mt19937& rng) const;

	/*
	Get quantity many random numbers in the range [0,range) in increasing order
	*/
	vector<unsigned int> getRandomIndices(unsigned int range, unsigned int quantity, mt19937& rng) const;
};

