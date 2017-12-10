#pragma once

#include "FixedPoint.h"

class Minerals {

private:

    FixedPoint minerals_;

    FixedPoint gas_;

public:

    /**
     * Construct this cost object
     */
    Minerals(FixedPoint minerals = 0, FixedPoint gas = 0)
            : minerals_(minerals), gas_(gas) {}

	/*
	Copy Constructor
	*/
	Minerals(const Minerals& other):minerals_(other.minerals_), gas_(other.gas_){}

    /**
     * Get the amount of minerals.
     * @return
     */
	FixedPoint minerals() const {
		return minerals_;
	}

    /**
     * Get this amount of gas.
     * @return
     */
	FixedPoint gas() const {
		return gas_;
	}

    /**
     * Add mineral objects
     */
	Minerals operator+(const Minerals &m) const {
		return Minerals(minerals_ + m.minerals(), gas_ + m.gas());
	}

    /**
     * Substract mineral objects
     */
	Minerals operator-(const Minerals &m) const {
		return Minerals(minerals_ - m.minerals(), gas_ - m.gas());
	}

    /**
     * Divide mineral objects
     */
    //Minerals operator/(const Minerals &) const;

    /**
     * Multiply mineral objects
     */
    //Minerals operator*(const Minerals &) const;

    /**
     * Add mineral objects
     */
	Minerals &operator+=(const Minerals &m) {
		minerals_ += m.minerals();
		gas_ += m.gas();
		return *this;
	}

    /**
     * Substract mineral objects
     */
	Minerals &operator-=(const Minerals &m) {
		minerals_ -= m.minerals();
		gas_ -= m.gas();
		return *this;
	}

    /**
     * Divide mineral objects
     */
    //Minerals &operator/=(const Minerals &);

    /**
     * Multiply mineral objects
     */
    //Minerals &operator*=(const Minerals &);


    /**
     * Assign mineral objects
     */
	Minerals &operator=(const Minerals &m) {
		minerals_ = m.minerals();
		gas_ = m.gas();
	}

    /**
     * Assign number (applied to all members)
     */
	Minerals &operator=(const FixedPoint fp) {
		minerals_ = fp;
		gas_ = fp;
        return *this;
	}

    /**
     * Element wise comparison
     */
    bool operator<(const Minerals &rhs) const {
        return minerals_ < rhs.minerals_ && gas_ < rhs.gas_;
    }

    /**
     * Element wise comparison
     */
    bool operator>(const Minerals &rhs) const {
        return minerals_ > rhs.minerals_ && gas_ > rhs.gas_;
    }

    /**
     * Element wise comparison
     */
    bool operator<=(const Minerals &rhs) const {
        return minerals_ <= rhs.minerals_ && gas_ <= rhs.gas_;
    }

    /**
     * Element wise comparison
     */
    bool operator>=(const Minerals &rhs) const {
        return minerals_ >= rhs.minerals_ && gas_ >= rhs.gas_;
    }

    /**
     * Element wise comparison
     */
    bool operator==(const Minerals &rhs) const {
        return minerals_ == rhs.minerals_ && gas_ == rhs.gas_;
    }

};

typedef FixedPoint BuildTime;

typedef int Supply;

class Cost {
public:
	Cost(Minerals minerals, BuildTime time, Supply supply) : resources_(minerals), buildTime_(time), supply_(supply){}

private:
	BuildTime buildTime_;
	Supply supply_;
	Minerals resources_;
};

enum Race { Protoss, Zerg, Terran };