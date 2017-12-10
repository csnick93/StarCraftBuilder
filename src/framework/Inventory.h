#pragma once

#include <deque>
#include <map>
#include <memory>
#include <vector>
#include "Types.h"
#include "TypeID.h"
#include "FixedPoint.h"

using std::map;
using std::unique_ptr;
using std::deque;
using std::vector;

// forward declaration
class GameObject;

class BaseBuilding;

class BaseUnit;

class Inventory {

    friend class GameObject;

    friend class BaseUnit;

private:

    /**
     * The game objects that are currently in the inventar, grouped by the type
     * ID to allow for faster access
     */
    map<TypeID *, deque<unique_ptr<GameObject> > > objects_;

    /**
     * The game objects that are currently being built
     */
    deque<unique_ptr<GameObject>> objectsUnderConstruction_;

    /**
     * Amount of minerals available
     */
    Minerals minerals_;

    /**
     * Minerals that have been delivered by not added to minerals_ yet
     */
    Minerals cachedMinerals_;

    /**
     * Amount of supply that is available
     */
    Supply supplyAvailable_;

    /**
     * Amount of supply that is used
     */
    Supply supplyUsed_;

    /**
     * Get the list of objects by a TypeID. Returns nullptr if no objects of that type are in the inventory.
     */
    deque<unique_ptr<GameObject> > &objectsByTypeID(TypeID *);

    /**
     * Add a GameObject to the objects_ list
     */
	unique_ptr<GameObject>* objectsPushBack(unique_ptr<GameObject>);

    /**
     * # of units assigned to mineral collection
     */
    unsigned mineralCollectors_;

    /**
     * # units assigned to gas harvesting
     */
    unsigned gasHarvesters_;


public:
    // Must not be assigned or copied
    Inventory(Inventory const &) = delete;
    void operator=(Inventory const &) = delete;

    Inventory();

    /**
     * Initializes the inventory
     */
    void initialize(vector<unique_ptr<GameObject>> &, Minerals);

    /**
     * Modify minerals_ according to the provided minerals object.
     */
    void checkout(Minerals const &);

    /**
     * Add the given object to the inventar. Adjusts supply accordingly.
	 * @return: returns nullptr if not successfully added, returns unique_ptr<GameObject>* otherwise (so pointer to gameObject in map)
     *				(this is necessary to be able to work with GameObject in Simulator::manageSpecialAbilityAndBuildStart)
     */
	unique_ptr<GameObject>* add(unique_ptr<GameObject> const);

    /**
     * Removes the given object from the inventar. Adjusts supply accordingly.
     *
     * @return The GameObject that whose ownership has been removed from Inventar
     */
    unique_ptr<GameObject> remove(unique_ptr<GameObject> const &);

    /**
     * Checks if at least as many minerals (negative values expected!), as specified, are available.
     */
    bool has(Minerals const &) const;

    /**
     * Checks if the given type is in the inventar (and is completely built).
     */
    bool has(TypeID *) const;

    /**
     * Checks if the given types are in the inventar (and are completely build). A single TypeID may occur more than,
     * in which case the exact number of occurences is set as a requirement.
     */
    bool has(const vector<TypeID *> &) const;

    /**
     * Checks if the given amount of supply (negative value!) is available.
     */
    bool has(Supply) const;

    /**
     * Checks if the given GameObject can be built according to the requirements.
     */
    bool canBuild(GameObject const &) const;

    /**
     * Called by workers deliver produced minerals to the inventory.
     *
     * Note: Must be cached by the inventory and only added to minerals_ as soon as updateResources is called by
     * the simulator.
     */
    void deliverMinerals(Minerals const &);

    /**
     * Update minerals_ according to the minerals that have been delivered in the last time step
     */
    void updateMinerals();

    /**
     * Get the list of objects by a TypeID
     */
    deque<unique_ptr<GameObject> > const &getObjectsByTypeID(TypeID *) const;

    /**
     * Occupy one GameObject of the given TypeID's. Returns the occupied Building.
     */
    BaseBuilding *occupyOneOf(const vector<TypeID *> &) const;

	/**
	* Find GameObject to build the building. Returns the occupied BaseUnit.
	*/
    BaseUnit *occupyBuildUnit(const vector<TypeID *> &typeIDs) const;

	
	/*
	 * Add supply (used for buildings that are not GameObjects)
	 */
	void addSupply(Supply s);

	/*
	 * Remove supply (necessary when morphing from objects -> supply needs to be adjusted)
	 */
	void removeSupply(Supply s);

	/**
	* GameObjects notify on build start (need this to be public for derived classes of GameObject)
	*/
	void notifyBuildStart(GameObject *);

	/**
	* GameObjects notify on build end (need this to be public for derived classes of GameObject)
	*/
	void notifyBuildEnd(GameObject *);

    /**
     * Simulate one timestep
     */
    void tick();

    // Some accessors following... //

    unsigned getNumberMineralCollectors() const;

    unsigned getNumberVespeneCollectors() const;

    FixedPoint getNumberMinerals() const;

    FixedPoint getNumberGas() const;

    Supply getNumberSupplyUsed() const;

    Supply getNumberAvailableSupply() const;

    /**
     * Streams status information like minerals, gas, supply, gameobjects..
     */
    friend ostream &operator<<(ostream &, const Inventory &);

    /**
     * Are there currently any objects under construction?
     */
    bool hasObjectsUnderConstruction();

    /**
     * Access the list of objects under construction
     */
    deque<unique_ptr<GameObject>> const &getObjectsUnderConstruction() const;

};


