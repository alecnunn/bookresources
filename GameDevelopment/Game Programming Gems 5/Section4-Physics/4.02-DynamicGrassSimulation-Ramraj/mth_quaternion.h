#ifndef __MTH_QUATERNION_H__
#define __MTH_QUATERNION_H__
//----------------------------------------------------------------------------
//
//mth_quaternion.h
//
//Copyright © Rishi Ramraj, 2004
//Quaternion classes for 4 dimentional quaternion vectors
//----------------------------------------------------------------------------

//··········································································//
//    header :: Inclusions
//··········································································//

//··········································································//
//    header :: Definitions
//··········································································//

//··········································································//
//    header :: Structures
//··········································································//

//··········································································//
//    header :: Class Defs
//··········································································//

class mth_quat;
class mth_quat_wrp;

//encoding:
//x,y,z,w

//quaternion
// - operates on its own variables
class mth_quat
{
public:
	mth_quat(void);
	mth_quat(const float * quat);
	~mth_quat(void);

	void set_quat(const float * quat);
	void set_quat(const mth_quat & quat);
	void set_quat(const mth_quat_wrp & quat);

	//create a rotation matrix from the quaternion
	void set_matrix(float * mtx);
	void set_slerp(const float * q1, const float * q2, float i_val);

	void set_identity(void);
	void set_inv(void);

	const float * get_quat(void) const;	

private:
	float m_quat[4];
};

//quaternion wrapper
// - operates on exernal veriables
class mth_quat_wrp
{
public:
	mth_quat_wrp(void);
	mth_quat_wrp(float * quat);
	~mth_quat_wrp(void);

	void wrp_quat(float * quat);
	void set_quat(const float * quat);
	void set_quat(const mth_quat & quat);
	void set_quat(const mth_quat_wrp & quat);

	//create a rotation matrix from the quaternion
	void set_matrix(float * mtx);
	void set_slerp(const float * q1, const float * q2, float i_val);

	void set_identity(void);
	void set_inv(void);

	const float * get_quat(void) const;	

private:
	float * m_quat;
};


//··········································································//
//    header :: Function Defs
//··········································································//

//----------------------------------------------------------------------------
//mth_quaternion.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__MTH_QUATERNION_H__