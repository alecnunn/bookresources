#ifndef __MTH_VECTOR_H__
#define __MTH_VECTOR_H__
//----------------------------------------------------------------------------
//
//mth_vector.h
//
//Copyright © Rishi Ramraj, 2004
//Vector Classes for 3 component vectors
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

//vector 
// - operates on its own variables
class mth_vector
{
public:
	mth_vector(void);
	mth_vector(const float * vec);
	~mth_vector(void);

	void set_vector(const float * vec);
	void set_vector(const float x,const float y,const float z);
	//multiply by a matrix
	void set_matrix(const float * mtx);
	void set_matrix_no_trans(const float * mtx);
	//multiply by an inverse matrix
	void set_matrix_inv(const float * mtx);
	void set_matrix_inv_no_trans(const float * mtx);
	void set_matrix_inv_trans(const float * mtx);//just translation
	void set_normalize(void);

	const float * get_vec(void) const;
	float get_len(void) const;
	float get_x(void) const;
	float get_y(void) const;
	float get_z(void) const;	

	const float operator[](const int index) const;

private:
	float m_vec[3];
};

//vector wrapper
// - operates on exernal veriables
class mth_vector_wrp
{
public:
	mth_vector_wrp(void);
	mth_vector_wrp(float * vec);
	~mth_vector_wrp(void);

	void wrp_vector(float * vec);
	void set_vector(const float * vec);
	void set_vector(const float x,const float y,const float z);
	void set_matrix(const float * mtx);
	void set_matrix_no_trans(const float * mtx);
	void set_matrix_inv(const float * mtx);
	void set_matrix_inv_no_trans(const float * mtx);
	void set_matrix_inv_trans(const float * mtx);//just translation
	void set_normalize(void);

	const float * get_vec(void) const;
	float get_len(void) const;
	float get_x(void) const;
	float get_y(void) const;
	float get_z(void) const;

	const float operator[](const int index) const;

private:
	float * m_vec;
};

//··········································································//
//    header :: Function Defs
//··········································································//

//----------------------------------------------------------------------------
//mth_vector.h
//Copyright © Rishi Ramraj, 2004
//----------------------------------------------------------------------------
#endif //__MTH_VECTOR_H__