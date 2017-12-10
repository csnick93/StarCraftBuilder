#pragma once
#include <assert.h>
#include <ostream>
#include <math.h>
#include <string>

using std::ostream;
using std::string;

/*
We need a fixed point representation of precision n=4:
	- Minerals are harvested at rate of 0.7
	- Gas is harvested at rate of 0.35
	- energy is loaded at rate of .5625
	--> do not need to be more precise than 4 places behind decimal

Represent 1.2345 as integer(1), tenth(2), hundredth(3), thousandth(4), tenthousandth(5)

*/


class FixedPoint {

public:
	FixedPoint(): integer_(0),tenth_(0), hundredth_(0), thousandth_(0), tenthousandth_(0){}

	/*
	NOTE: you can use FixedPoint like before,so : 1.5625 = FixedPoint(1,5625), 0.35 = FixedPoint(0,35)
	The only time you have to pay attention is when you want to create a fixed point where a 0 occurs before a non-zero digit in the decimal, e.g. 
	1.056 = FixedPoint(1,0,5,6)
	*/
	FixedPoint(unsigned int whole, unsigned int tenth=0, unsigned int hundredth=0, unsigned int thousandth=0, unsigned int tenthousandth=0):
		integer_(whole), tenth_(tenth), hundredth_(hundredth), thousandth_(thousandth), tenthousandth_(tenthousandth)
	{
		assert(tenth < 10000);
		assert(hundredth < 10);
		assert(thousandth < 10);
		assert(tenthousandth < 10);
		//enable initialization like: FixedPoint(0,5625)
		if (tenth > 10) {
			if (tenth < 100) {
				tenth_ = tenth / 10;
				hundredth_ = tenth % 10;
			}
			else if (tenth < 1000) {
				tenth_ = tenth / 100;
				hundredth_ = (tenth % 100) / 10;
				thousandth_ = tenth / 10;
			}
			else if (tenth < 10000) {
				tenth_ = tenth / 1000;
				hundredth_ = (tenth % 1000)/100;
				thousandth_ = (tenth % 100) / 10;
				tenthousandth_ = tenth % 10;
			}
		}
	}

	FixedPoint(const FixedPoint& other) : integer_(other.integer_), tenth_(other.tenth_), hundredth_(other.hundredth_),
		thousandth_(other.thousandth_), tenthousandth_(other.tenthousandth_) {}
	//

	void operator=(const FixedPoint& rhs) {
		integer_ = rhs.integer_;
		tenth_ = rhs.tenth_;
		hundredth_ = rhs.hundredth_;
		thousandth_ = rhs.thousandth_;
		tenthousandth_ = rhs.tenthousandth_;
	}

	void operator=(const unsigned int val) {
		integer_ = val;
		tenth_ = 0;
		hundredth_ = 0;
		thousandth_ = 0;
		tenthousandth_ = 0;
	}

	//
   
	FixedPoint operator+(const FixedPoint& other) const{
		unsigned int tmp = (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_) +
			(other.integer_ * 10000 + other.tenth_ * 1000 + other.hundredth_ * 100 + other.thousandth_ * 10 + other.tenthousandth_);
		return FixedPoint(tmp / 10000, (tmp % 10000) / 1000, (tmp % 1000) / 100, (tmp % 100) / 10, tmp % 10);
	}

	FixedPoint operator-(const FixedPoint& other) const {
		unsigned int tmp = (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_) -
			(other.integer_ * 10000 + other.tenth_ * 1000 + other.hundredth_ * 100 + other.thousandth_ * 10 + other.tenthousandth_);
		return FixedPoint(tmp / 10000, (tmp % 10000) / 1000, (tmp % 1000) / 100, (tmp % 100) / 10, tmp % 10);
	}

	FixedPoint operator*(const unsigned int other) const {
		unsigned int tmp = (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_)*other;
		return FixedPoint(tmp / 10000, (tmp % 10000) / 1000, (tmp % 1000) / 100, (tmp % 100) / 10, tmp % 10);
	}
	

	//

	FixedPoint& operator+=(const FixedPoint& other) {
		unsigned int tmp = (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_) +
			(other.integer_ * 10000 + other.tenth_ * 1000 + other.hundredth_ * 100 + other.thousandth_ * 10 + other.tenthousandth_);
		integer_ = tmp / 10000;
		tenth_ = (tmp % 10000) / 1000;
		hundredth_ = (tmp % 1000) / 100;
		thousandth_ = (tmp % 100) / 10;
		tenthousandth_ = tmp % 10;
		return *this;
	}

	FixedPoint& operator-=(const FixedPoint& other) {
		unsigned int tmp = (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_) -
			(other.integer_ * 10000 + other.tenth_ * 1000 + other.hundredth_ * 100 + other.thousandth_ * 10 + other.tenthousandth_);
		integer_ = tmp / 10000;
		tenth_ = (tmp % 10000) / 1000;
		hundredth_ = (tmp % 1000) / 100;
		thousandth_ = (tmp % 100) / 10;
		tenthousandth_ = tmp % 10;
		return *this;
	}
	

	unsigned int getNecessaryGasWorker() const {
		/*
		Get number of GasWorkers necessary to harvest *this amount of gas in one timestep
		-> round to next higher integer
		*/
		unsigned int tmp = (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_);
		return ceil(tmp / 3500.f);
	}


	
	unsigned int getNecessaryMineralWorker() const {
		/*
		Get number of MineralWorkers necessary to harvest *this amount of minerals in one timestep
		*this must be divisble by .7 since we always increment in .7-steps
		*/
		unsigned int tmp = (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_);
		return ceil(tmp / 7000.f);
	}
	


	//
	bool operator==(const FixedPoint& other) const{
		return ((integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_) ==
			(other.integer_ * 10000 + other.tenth_ * 1000 + other.hundredth_ * 100 + other.thousandth_ * 10 + other.tenthousandth_));
	}

	bool operator!=(const FixedPoint& other) const {
		return !(*this == other);
	}

    bool operator<(const FixedPoint &other) const {
		return ((integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_) <
			(other.integer_ * 10000 + other.tenth_ * 1000 + other.hundredth_ * 100 + other.thousandth_ * 10 + other.tenthousandth_));
    }

	bool operator<(const int rhs) const {
		return integer_ < rhs;
	}

    bool operator>(const FixedPoint &rhs) const {
        return rhs < *this;
    }

	bool operator>(const int rhs) const {
		return (integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_ > rhs*10000);
	}

    bool operator<=(const FixedPoint &other) const {
		return ((integer_ * 10000 + tenth_ * 1000 + hundredth_ * 100 + thousandth_ * 10 + tenthousandth_) <=
			(other.integer_ * 10000 + other.tenth_ * 1000 + other.hundredth_ * 100 + other.thousandth_ * 10 + other.tenthousandth_));
    }

	bool operator>=(const FixedPoint &rhs) const {
		return rhs <= *this;
	}

	bool operator<=(const int rhs) const {
		return !(*this > rhs);
	}

	bool operator>=(const int rhs) const {
		return !(*this < rhs);
	}


	FixedPoint operator--() {
		--integer_;
		return *this;
	}

	//
	// accessor
	unsigned int getWholeUnits() const {
		return integer_;
	}
	unsigned int getTenth() const {
		return tenth_;
	}
	unsigned int getHundredth() const {
		return hundredth_;
	}
	unsigned int getThousandth() const {
		return thousandth_;
	}
	unsigned int getTenThousandth_() const {
		return tenthousandth_;
	}


    friend ostream &operator<<(ostream &os, const FixedPoint &point) {
        os << point.getWholeUnits() << "." << point.getTenth() << point.getHundredth() << point.getThousandth() << point.getTenThousandth_();
        return os;
    }


private:
	unsigned int integer_;
	unsigned int tenth_;
	unsigned int hundredth_;
	unsigned int thousandth_;
	unsigned int tenthousandth_;
};
