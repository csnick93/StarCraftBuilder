#include "BaseBuilding.h"

BaseBuilding::Activity BaseBuilding::getActivity() const {
	return activity_;
}

void BaseBuilding::setActivity(Activity activity) {
    activity_ = activity;
}

bool BaseBuilding::occupied() const {
    return activity_ != LAZY;
}
