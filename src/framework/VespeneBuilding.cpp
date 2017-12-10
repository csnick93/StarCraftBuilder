#include "VespeneBuilding.h"

VespeneBuilding::VespeneBuilding(Simulator *s, Inventory *i, Minerals m, Supply supply, BuildTime bt,
	vector<TypeID *> pp, vector<TypeID *> pd, TypeID *tid, TypeID* mo):
	GameObject(s, i, m, supply, bt, pp, pd,tid,mo), openSpots_(3) {
};

void VespeneBuilding::decrement() {
	if (openSpots_ == 0) {
		throw std::logic_error("No more openSpots!");
	}
	D(std::cout << "Decrementing spots in geysir" << std::endl;);
	--openSpots_;
}

void VespeneBuilding::increment() {
	if (openSpots_ == 3) {
		throw std::logic_error("Already no workers at building harvesting gas!");
	}
	++openSpots_;
}

unsigned int VespeneBuilding::getOpenSpots() const {
	return openSpots_;
}

