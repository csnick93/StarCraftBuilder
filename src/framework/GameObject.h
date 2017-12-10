#pragma once

#include <vector>
#include <string>
#include <memory>
#include "Types.h"
#include "ID.h"
#include "TypeID.h"
#include "Inventory.h"

using std::vector;
using std::unique_ptr;

// forward decls
class Simulator;
class BaseUnit;
class BaseBuilding;


class GameObject {

    friend class Inventory;

protected:

    /**
     * The time that remains until the object is completely built
     */
    BuildTime remainingBuildTime_;

    /**
     * Unique identifier for this game object
     */
    ID id_;

    /**
     * Type identifier for this game object type
     */
    TypeID *typeID_;

    /**
     * Minerals required to build this object. Entries should be negative in order to express a cost.
     */
    Minerals minerals_;

    /**
     * Preconditions that must be satisfied in order to build this object
     */
    vector <TypeID*> preconditions_;

    /**
     * GameObject-Types that are occupied while this object is built (first free is being occupied)
     */
    vector<TypeID*> producedIn_;

    /**
     * Building that is currently occupied with building this GameObject
     */
    BaseBuilding *occupies_;

    /**
     * Supply that is provided by (value > 0) or required by (value < 0) this game object.
     */
    Supply supply_;

    /**
     * The inventory, this GameObject is bound to. nullptr if not in an inventory.
     */
    Inventory *inventory_;

    /**
     * The simulator this GameObject belongs to
     */
    Simulator* simulator_;

    /**
     * The type of GameObject that is morphed into this during the build-process. nullptr if none..
     */
    TypeID* morphedFromType_;

	/*
	*	Supply that the morphedFrom Object provides (need to know this in order to adjust after morphed object is done building!)
	*/
	Supply morphedFromTypeSupply_;

	/*
	 * ID of object that this gameObject morphed from (need to remember that ID for Report output at build-start and build-end)
	 */
	ID morphedFrom_;

public:

    GameObject(const GameObject &) = delete;

    /**
     * Construct the GameObject. Each game object must belong to exact one Simulator.
     */
    GameObject(Simulator*);

    GameObject(Simulator*, Inventory *, Minerals, Supply, BuildTime, vector<TypeID *>, vector<TypeID *>,
                       TypeID *, TypeID *, Supply morph = 0);

    /**
     * Checks, if requirements are fulfilled (cost and game objects). If yes, initiates the build-process.#
     *
     * Note: GameObject must be added to an inventory BEFORE calling build()!
	 *
	 * if initialObject is true, then no preconditions must be checked, because object belongs to start configuration
     */
    virtual void build();

    /**
     * Checks, if requirements are fulfilled (cost and game objects). If yes, initiates the build-process and
     * removes the given game object from the inventar and decreases the resources in the inventar.
     *
     * Note: GameObject must be added to an inventory BEFORE calling build()!
     *
     * @param source The GameObject that is morphed into this GameObject.
     */
    virtual void build(unique_ptr<GameObject> const &);

    /**
     * Simulate one time step
     */
    virtual void tick();

    /**
     * @return The unique type ID for this class.
     */
    TypeID *typeID() const;

    /**
    * Get the ID of the object
    */
    ID getID() const;

    /**
     * Get the cost that is required to build this GameObject.
     */
    Minerals const &requiredMinerals() const;

    /**
     * Get preconditions
     */
    vector<TypeID *> const &preconditions() const;

    /**
     * Get supply that is provided by (> 0) or required by (< 0) this game object.
     */
    Supply supply() const;

    /**
     * Set the inventory
     */
    void setInventory(Inventory *inventory);

	/**
	* Set the simulator
	*/
	void setSimulator(Simulator *s);

	/*
	get inventory pointer (required for derived classes from GameObject)
	*/
	Inventory* getInventory() const;

    /**
     * Get the type ID this object is morphed from. nullptr if none.
     */
    TypeID *getMorphedFromType() const;


    /*
    Get vector of TypeIDs that GameObject can be producedIn
    */
	virtual vector<TypeID *> const &producedIn() const;

    /**
     * Just sets remaining build time to 0
     */
    void setIsBuilt();

	/**
	* Find out if object is built
	*/
	bool isBuilt() const;

	/*
	Accessor necessary for last object to be built (to start simulation countdown)
	*/
	BuildTime getBuildTime() const;

    /**
     * Stream TypeID and ID
     */
    friend ostream &operator<<(ostream &, const GameObject &);

};


