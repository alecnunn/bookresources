


#ifndef _I_Vector3Y_H_
#define _I_Vector3Y_H_

class Vector3
{

public:

	Vector3(double _x, double _y, double _z) : x(_x), y(_y), z(_z)
	{


	}

	Vector3() : x(0), y(0), z(0) {}


	~Vector3(){}



/*	void wypisz(void)
{
		//std::cout << " x=" << x << " y=" << y << " z=" << z << "\n"; 
	}
*/

// operatory
	
	Vector3 operator+(Vector3 const &);
	Vector3 operator-(Vector3 const &);
	double operator*(Vector3 const &);	// iloczyn skalarny
	Vector3 operator*(double);	// iloczyn skalar - Vector3
	Vector3 operator%(Vector3 const &);	// iloczyn Vector3owy
	void   operator=(Vector3 const &);   // rownosc dwu Vector3ow




// metody

	double length();			// zwraca length Vector3a
	void norm();				// normuje Vector3


	double x,y,z;		// sk³adowe Vector3a		   - potem zmienic na private bez dostepu z zewnatrz

private:
	

};


#endif
