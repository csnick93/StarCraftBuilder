#include "BaseUnit.h"
#include "Debug.h"

using std::cout;
using std::endl;

void BaseUnit::harvest() const {
    switch (activity_) {
        case GAS:
            inventory_->deliverMinerals(Minerals(0, FixedPoint(0, 35)));
			D(std::cout << "I " << *this << " am collecting gas" << std::endl;);
            break;
        case MINERALS:
            inventory_->deliverMinerals(Minerals(FixedPoint(0, 70), 0));
			D(std::cout << "I " << *this << " am collecting minerals" << std::endl;);
            break;
    }
}

void BaseUnit::tick() {
	if (isBuilt())
		harvest();
    GameObject::tick();
}

BaseUnit::Activity BaseUnit::getActivity() const {
    return activity_;
}

void BaseUnit::setActivity(Activity activity) {
    assert(isBuilt() || activity == LAZY);

    // book-keeping
	if (activity_ == GAS && activity != GAS) {
		// when switching away from gas, spots in geysirs need to be adjusted
		deque<unique_ptr<GameObject>> const *geysirs = &(inventory_->getObjectsByTypeID(simulator_->getObjectManager()->getVespeneGeysirTypeID()));
		if (geysirs->size() == 2) {
			try {
				dynamic_cast<VespeneBuilding*>(geysirs->front().get())->increment();
			}
			catch (...) {
				dynamic_cast<VespeneBuilding*>(geysirs->back().get())->increment();
			}
		}
		else if (geysirs->size() == 1) {
			dynamic_cast<VespeneBuilding*>(geysirs->front().get())->increment();
		}
		else {
			throw std::logic_error("There must be geysirs, otherwise worker could not have harvested gas!");
		}
		--inventory_->gasHarvesters_;
	}

    if (activity_ == MINERALS && activity != MINERALS)
        --inventory_->mineralCollectors_;

	if (activity == GAS && activity_ != GAS) {
		// when switching to gas, spots in geysirs need to be adjusted
		deque<unique_ptr<GameObject>> const *geysirs = &(inventory_->getObjectsByTypeID(simulator_->getObjectManager()->getVespeneGeysirTypeID()));
		if (geysirs->size() == 2) {
			try {
				dynamic_cast<VespeneBuilding*>(geysirs->front().get())->decrement();
			}
			catch (...) {
				dynamic_cast<VespeneBuilding*>(geysirs->back().get())->decrement();
			}
		}
		else if (geysirs->size() == 1) {
			dynamic_cast<VespeneBuilding*>(geysirs->front().get())->decrement();
		}
		else {
			throw std::logic_error("There must be geysirs, otherwise worker could not have harvested gas!");
		}

		++inventory_->gasHarvesters_;
	}
    if (activity == MINERALS && activity_ != MINERALS)
        ++inventory_->mineralCollectors_;

    activity_ = activity;
}

bool BaseUnit::occupied() const {
    return activity_ != LAZY;
}

bool BaseUnit::availableForBuild() const
{
    return isBuilt() && activity_ != OTHER;
}
