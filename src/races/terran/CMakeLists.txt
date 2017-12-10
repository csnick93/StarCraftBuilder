include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

add_compile_options(-ftemplate-backtrace-limit=0)

find_package (Threads)

set(TERRAN_SIMULATOR_HEADER_FILES
        GameObjectManagerTerran.h
        TerranBuilding.h
		TerranBuildings.h
		TerranUnits.h
		)

set(TERRAN_SIMULATOR_SOURCE_FILES
		TerranBuilding.cpp
		TerranBuildings.cpp
        TerranSimulator.cpp
	GameObjectManagerTerran.cpp
        )
		
add_executable(terran_simulator ${TERRAN_SIMULATOR_HEADER_FILES} ${TERRAN_SIMULATOR_SOURCE_FILES})
target_link_libraries(terran_simulator framework)


install(TARGETS terran_simulator DESTINATION bin)