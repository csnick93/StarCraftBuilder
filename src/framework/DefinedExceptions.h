#pragma once

class BuildListException : public std::exception {
public:
	virtual const char* what() const throw()
	{
        return "Build List is not valid! Aborting simulation...";
	}
};

class SimulationDoneException : public std::exception {
public:
	virtual const char* what() const throw()
	{
		return "Simulation is over!";
	}
};


class ParserException : public std::exception {
	/*
	Exception Class that is thrown if a faulty path to the build list is provided
	*/
public:
	virtual const char* what() const throw()
	{
		return "Error reading in the build list. Potential reason being a wrong filepath.";
	}
};

