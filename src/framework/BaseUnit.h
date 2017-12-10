#pragma once

#include "GameObject.h"
#include "Simulator.h"


class BaseUnit : public GameObject {

public:

    //BaseUnit(Simulator *s) : GameObject(s), activity_(MINERALS) {};

	/**
	 * @param s Simulator object
	 * @param i Inventory object
	 * @param m Minerals (gas/minerals) required to build this object
	 * @param supply Supply required/provided by this object
	 * @param bt Time required to build this object
	 * @param pp Vector of TypeID's that define in which buildings this object can be produced
	 * @param pd Vector of TypeID's that must be built in order to build this object (preconditions)
	 * @param tid TypeID of this GO
	 * @param mo TypeID that specified from which type this object is morphed
	 */
	BaseUnit(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt, vector<TypeID *> pp,
			 vector<TypeID *> pd, TypeID *tid, TypeID *mo) :
			GameObject(s, i, m, supply, bt, pp, pd, tid, mo), activity_(LAZY) {};

    /**
     * Possible activities, a Unit can do
     */
	enum Activity {
		LAZY,           // unassigned
		GAS,            // harvesting gas
		MINERALS,       // harvesting minerals
		OTHER           // other activity, may be useful for implementing the races
	};

   

    virtual void tick() override;

    /**
     * Get the current activity
     */
    Activity getActivity() const;

    /**
     * Set the current activity
     */
    void setActivity(Activity);

    /**
     * activity != LAZY?
     */
    bool occupied() const;

	/**
	* activity_ != OTHER
	*/
	bool availableForBuild() const;

private:

	/**
	* The activity, this Unit is currently doing
	*/
	Activity activity_;

	/**
	* Produce minerals according to current activity and deliver them to the Inventory
	*/
	void harvest() const;
};


