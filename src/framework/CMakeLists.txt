set(FRAMEWORK_HEADER_FILES
        BaseBuilding.h
        BaseUnit.h
        BuildList.h
		DefinedExceptions.h
        Events.h
        FixedPoint.h
		Framework.h
        GameObject.h
        GameObjectManager.h
        ID.h
        Inventory.h
        Message.h
		Messenger.h
        Resources.h
        Simulator.h
        Status.h
        TypeID.h
        Types.h
		VespeneBuilding.h
        Workers.h
		Debug.h
		Optimizer.h)

set(FRAMEWORK_SOURCE_FILES
        BaseBuilding.cpp
        BaseUnit.cpp
        BuildList.cpp
        GameObject.cpp
        ID.cpp
        Inventory.cpp
        Simulator.cpp
        TypeID.cpp
        VespeneBuilding.cpp)

add_library(framework STATIC ${FRAMEWORK_HEADER_FILES} ${FRAMEWORK_SOURCE_FILES})
