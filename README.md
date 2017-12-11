# StarCraftBuilder

This is C++ solution simulates the build process for the terran race in StarCraft. The point of this project was to play around with a bigger C++ project having a fun objective.

## Star Craft General Rules

During the early phase of a StarCraft game, the players try to build up their main base and try to decide for a certain opening strategy. These can be very different: fast expansion to increase later income, developing new technology to build better units, or quickly build as many units as possible in order to rush the enemy. However, there are multiple dependencies, costs and rules when it comes to building different units. Therefore the goal of the implemented project is to simulate a given build list to determine whether it can be built within a certain amount of time, or whether it is not feasible. This kind of simulation might be very useful when it comes to finding an optimal opening strategy, using e.g. an evolutionary algorithm. The optimization was not coded in this scope, but could be relatively easily built on top of this code.
The exact build rules are described in Techtree_Key.png (describing the general rules), Techtree_Terran.pdf (describing the specific build rules for the Terran race), and the techtrees.csv which basically encodes the Techtree_Terran.pdf information in a more machine readable way.

## Usage

In order to run the code, find the *terran_simulator.exe* in*./src/races/terran/Release*. Run the code by providing an extra two command line arguments:
  - path2buildList: path to a build list to be simulated (find example build lists in the *./BuildLists* folder
  - path2outputFile: path to a output file where the simulation log can be written to
  
So, run: *terran_simulator.exe path2buildList path2outputFile*

Alternatively, load the code into Visual Studio using the *starcraft_builder.sln* and run from there.
