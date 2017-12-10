#pragma once

#include "GameObject.h"



class BaseBuilding : public GameObject {


public:

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
	 * @param morph The supply provided by the morphedFrom Object (needed for adjusting supply accordingly when morph process is finished)
     */
	BaseBuilding(Simulator* s, Inventory *i, Minerals m, Supply supply, BuildTime bt, vector<TypeID *> pp,
				 vector<TypeID *> pd, TypeID *tid, TypeID *mo, Supply morph = 0):
			GameObject(s, i, m, supply, bt, pp, pd, tid, mo, morph), activity_(LAZY) {};

	/**
	* Possible activities, a Building can do
	*/
	enum Activity {
		LAZY,           // unassigned
		OCCUPIED,       // producing a unit
		OTHER
	};

    /**
     * Get the current activity
     */
	virtual Activity getActivity() const;


    /**
     * Set the current activity
     */
    virtual void setActivity(Activity);

    /**
     * activity != LAZY?
     */
    virtual bool occupied() const;

protected:

	/**
	* The activity, this Building is currently doing
	*/
	Activity activity_;
};


