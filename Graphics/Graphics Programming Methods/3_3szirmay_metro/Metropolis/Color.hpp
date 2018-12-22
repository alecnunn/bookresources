// Graphics Programming Methods
// Metropolis sampling in global illumination
// György Antal, Lászlo Szirmay-Kalos
// 2003.

#pragma once

// the Color class represents the color
//-----------------------------------------------------------------
class Color {
public:
	double r, g, b;		// color coefficients on the representative wavelengths

	Color() {}
	Color(double rr, double gg, double bb) { 
		r = rr; g = gg; b = bb;
	} 

	void Set (double rr, double gg, double bb) {
		r = rr; g = gg; b = bb;
	}

	// binary operators
	Color operator+(const Color& c) const {
		return Color (r + c.r, g + c.g, b + c.b);
	}

	Color operator-(const Color& c) const {
		return Color (r - c.r, g - c.g, b - c.b);
	}

	Color operator*(double f) const {
		return Color (r * f, g * f, b * f);
	}

	Color operator*(const Color& c) const {
		return Color (r * c.r, g * c.g, b * c.b);
	}

	Color operator/(double f) const {
		return Color (r / f, g / f, b / f);
	}

	// unary operators
	void operator+=(const Color& c) {
		r += c.r; g += c.g; b += c.b;
	}

	void operator*=(const Color& c) {
		r *= c.r; g *= c.g; b *= c.b;
	}

	void operator*=(double c) {
		r *= c; g *= c; b *= c;
	}

	// other methods
	double Lum() const {
		return (r + g + b) / 3.0;
	}
	friend Color operator*(double f, const Color& c);
};

inline Color operator*(double f, const Color& c) {
	return Color (f * c.r, f * c.g, f * c.b);
}