#pragma once
#include "Debug.h"
#include "GameObject.h"


class VespeneBuilding : public GameObject {

private:
	// has between 0 and 3 open spots
	unsigned int openSpots_;

public:
	VespeneBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt,
		vector<TypeID *> pp, vector<TypeID *> pd, TypeID *tid, TypeID* mo);

	void decrement();

	void increment();

	unsigned int getOpenSpots() const;

};