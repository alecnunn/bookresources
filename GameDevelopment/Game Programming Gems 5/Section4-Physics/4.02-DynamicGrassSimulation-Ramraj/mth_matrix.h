#ifndef __MTH_MATRIX_H__
#define __MTH_MATRIX_H__
//----------------------------------------------------------------------------
//
//mth_matrix.h
//
//Copyright © Rishi Ramraj, 2004
//Matrix classes for 4x4 matrices
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

class mth_matrix;
class mth_matrix_wrp;

//matrix
// - operates on its own variables
class mth_matrix
{
public:
	mth_matrix(void);
	mth_matrix(const float * mtx);
	~mth_matrix(void);

	friend class mth_matrix_wrp;

	void set_matrix(const float * mtx);

	void set_multiply(const float * mtx);
	void set_multiply(const mth_matrix & mtx);
	void set_multiply(const mth_matrix_wrp & mtx);

	void set_identity(void);
	void set_trans(const float * vec);

	float * get_mtx(void);
	float get_ele(const int index) const;
	
	const float operator[](const int index) const;

private:
	float m_mtx[16];
};

//matrix wrapper
// - operates on exernal veriables
class mth_matrix_wrp
{
public:
	mth_matrix_wrp(void);
	mth_matrix_wrp(float * mtx);
	~mth_matrix_wrp(void);

	friend class mth_matrix;

	void wrp_matrix(float * mtx);
	void set_matrix(const float * mtx);

	void set_multiply(const float * mtx);
	void set_multiply(const mth_matrix & mtx);
	void set_multiply(const mth_matrix_wrp & mtx);

	void set_identity(void);
	void set_trans(const float * vec);

	float * get_mtx(void);
	float get_ele(const int index) const;
	
	const float operator[](const int index) const;

private:
	float * m_mtx;
};

//··········································································//
//    header :: Function Defs
//··········································································//

//----------------------------------------------------------------------------
//mth_matrix.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__MTH_MATRIX_H__