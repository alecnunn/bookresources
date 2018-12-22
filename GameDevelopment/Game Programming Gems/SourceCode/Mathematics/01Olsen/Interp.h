/* Copyright (C) John Olsen, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) John Olsen, 2000"
 */
// Interp.h
// Game Gems sample code showing for different interpolation methods.
// By John M. Olsen

#ifndef INTERP_H
#define INTERP_H

// The linear interpolation class calculates a constant velocity at
// setup time, then applies that velocity to the data point based on
// the specified time slice (usually one frame).
// It is frame rate independent, meaning varying frame rates should
// give consistent results.
class CLinearInterpolation
{
public:
	bool Setup(float from, float to, float time)
	{
		if(time < 0)
		{
			return false;
		}
		_remainingTime = time;
		_value = from;
		_step = (to-from)/time; // Calculate distance per second.
		return true;
	}
	// Return TRUE when the target has been reached or passed.
	bool Interpolate(float deltaTime)
	{
		_remainingTime -= deltaTime;
		_value += _step*deltaTime;
		return (_remainingTime <= 0);
	}
	float GetValue()
	{
		return _value;
	}
private:
	float _value;
	float _step;
	float _remainingTime;
};

// This is a version of the following equation:
// a = (a*(n-1) + b)/n
// or an example which has been simplified and had constants added:
// a = (a*7 + b)/8
// or
// a = (a*7 + b)>>3
// The second form is about as fast as you can get on hardware which 
// does not have good floating point support.
// This gives a heavy weighting to the original value, but will
// tend toward the destination value over time.
// It gets rather strong aliasing problems when the source and 
// destination are close to each other, and the shift value must 
// be integer based, which restricts the rates at which you 
// approach the destination. It is frame rate dependent, meaning 
// varying frame rates will give varying results.
class CEaseOutShiftInterpolation
{
public:
	bool Setup(int from, int to, int shift)
	{
		if(shift <= 0)
		{
			return false;
		}
		_value = from;
		_target = to;
		_shift = shift;
		return true;
	}
	bool Interpolate() // Note: Not time dependent.
	{
		int oldValue = _value;
		if(_shift > 0)
		{
			_value = (_value * ((1 << _shift) - 1) + _target) >> _shift;
		}
		// lots more likely than with float version.
		return (_value == oldValue);
	}
	int GetValue()
	{
		return _value;
	}
private:
	int _value;
	int _target;
	int _shift;
};

// This is roughly equal to the integer shift interpolation method, 
// but uses floating point multiplies and divides based on the same 
// equation:
// a = (a*(n-1) + b) / n
// In this case, the equation is used directly instead of being 
// converted into shifts. This gives it a wider number of possible 
// rates of movement. It is frame rate dependent, meaning varying 
// frame rates will give varying results.
class CEaseOutDivideInterpolation : CEaseOutShiftInterpolation
{
public:
	bool Setup(float from, float to, float divisor)
	{
		if(divisor <= 0)
		{
			return false;
		}
		_value = from;
		_target = to;
		_divisor = divisor;
		return true;
	}
	bool Interpolate() // Note: Not time dependent.
	{
		float oldValue = _value;
		if(_divisor > 0) 
		{
			_value = (_value * (_divisor-1.0f) + _target)/_divisor;
		}
		// Not likely to be true very often.
		return (_value == oldValue);
	}
	float GetValue()
	{
		return _value;
	}
private:
	float _value;
	float _target;
	float _divisor;
};

// Ease In and Ease Out Interpolation is done by calculating the 
// acceleration which will cover exactly half the distance in half 
// the specified time. That acceleration is applied for the first 
// half, then the inverse is applied for the second half to slow 
// down so the data point stops as it reaches the target point.
class CEaseInOutInterpolation
{
public:
	bool Setup(float from, float to, float time)
	{
		if(time <= 0)
		{
			return false;
		}
		_value = from;
		_target = to;
		_speed = 0.0f;
		// derived from x=x0 + v0*t + a*t*t/2
		_acceleration = (to-from)/(time*time/4);
		_remainingTime = _totalTime = time;
		return true;
	}
	bool Interpolate(float deltaTime)
	{
		_remainingTime -= deltaTime;
		if(_remainingTime < _totalTime/2)
		{
			// Deceleration
			_speed -= _acceleration * deltaTime;
		}
		else
		{
			// Acceleration
			_speed += _acceleration * deltaTime;
		}
		_value += _speed*deltaTime;
		return (_remainingTime <= 0);
	}
	float GetValue()
	{
		return _value;
	}
private:
	float _value;
	float _target;
	float _remainingTime;
	float _totalTime;
	float _speed;
	float _acceleration;
};



#endif // INTERP_H