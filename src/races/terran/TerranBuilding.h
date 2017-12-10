#pragma once

#include <framework/Framework.h>

//===================================================================================================
/*
Class for Terran-buildings that are built by scv workers and are never responsible for building anything
*/

class IdleTerranBuilding : public GameObject {
public:
	IdleTerranBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt,
		vector<TypeID *> pp, vector<TypeID *> pd, TypeID *tid, TypeID *mo);


	/*
	need to override tick()-method to adjust setting produced_by free instead of occupied_
	*/
	void tick() override;

	/*
	need to override build()-method such that the scv-unit can be occupied
	*/
	void build() override;


private:
	// scv-worker that is building the building
	BaseUnit* producedBy_;
};


//===================================================================================================

/*
class of Buildings that are built by scv-workers and are also responsible for building units themselves
*/
class ProducingTerranBuilding : public BaseBuilding {
public:

	ProducingTerranBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt, vector<TypeID *> pp,
		vector<TypeID *> pd, TypeID *tid, TypeID *mo, Supply morph = 0);


    /*
    need to override tick()-method to adjust setting produced_by free instead of occupied_
    */
	void tick() override;

    /*
    need to override build()-method such that the scv-unit can be occupied
    */
	void build() override;

private:
    // scv-worker that is building the building
    BaseUnit *producedBy_;
};

//===================================================================================================

/*
building class for modeling reactor buildings (that have the "produce two units at a time" special ability)
*/
class ReactorBuilding : public BaseBuilding {
public:

	ReactorBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt,
		vector<TypeID *> pp, vector<TypeID *> pd, TypeID *tid, TypeID *mo);

    /*
    convert OTHER -> LAZY (so that GameObject knows it can be produced by this building)
    */
	Activity getActivity() const override;

    /*
    */
	void setActivity(Activity a) override;

	bool occupied() const override;
};