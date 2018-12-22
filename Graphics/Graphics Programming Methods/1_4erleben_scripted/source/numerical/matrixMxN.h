////////////////////////////////////////////////////////////////////////////////
//
//  Copyright (c) 2002
//  Department of Computer Science Copenhagen University (DIKU).
//
//  All Rights Reserved.
//
//  This software is provided as is WITHOUT ANY WARRANTY; Use it on
//  your own risk.
//
//  Please send remarks, questions and bug reports to kenny@diku.dk
//
////////////////////////////////////////////////////////////////////////////////
#ifndef __MATRIX_M_N_INCLUDED__
#define __MATRIX_M_N_INCLUDED__
#include <numerical/scalar.h>
#include <string>

/**
 * A matrix
 * This class is only capable of representing
 * M X N matrices.
 *
 * If M and N is not specified the matrix is by
 * default 4 x 4.
 */
class MatrixMxN
{

  friend class VectorN;

public:

  MatrixMxN();
  virtual ~MatrixMxN();

public:

  MatrixMxN(const int & rows,const int & columns);
  MatrixMxN(const MatrixMxN & M);
	void resize(const int & m,const int & n);
  void set(const MatrixMxN & M);
  void set(scalar * entries,const int & length);
	int getRowDimension(void)const;
	int getColumnDimension(void)const;
  void setRow(const int & i,const VectorN & row);
  void setColumn(const int & i,const VectorN & column);
  void getRow(const int & i,VectorN & row);
  void getColumn(const int & i,VectorN & column);
  bool equals(const MatrixMxN & M);
  bool nearlyEquals(const MatrixMxN & M);
  void negate(void);
  void negate(const MatrixMxN & M);
  void sub(const MatrixMxN & B);
  void sub(const MatrixMxN & A,const MatrixMxN & B);
  void add(const MatrixMxN & B);
  void add(const MatrixMxN & A,const MatrixMxN & B);
  void mul(const scalar & s);
  void div(const scalar & s);
  void mul(const MatrixMxN & A,const MatrixMxN & B);
  void premul(const MatrixMxN & A);
  void postmul(const MatrixMxN & B);
  void transpose(void);
  void transpose(const MatrixMxN & A);
  void inverse(void);
	scalar ludcmp(MatrixMxN & LU,int * permutation);
	void lubksb(MatrixMxN & LU,int * permutation,VectorN  & b);
  void truncate(void);
	void setIdentity(void);
	void setZero(void);
  void orthonormalize(void);
  std::string toString(void);
  
public:

	/**
	 * Safety Checking.
	 * Boolean flag that indicates whatever dimension safety control
	 * (among other) is on or off.
	 *
	 * It is usefull to switch these safety checks on during debugging.
	 */
	static bool safety;

protected:

	/**
	 * The dimensions of the matrix.
	 * Default dimension is 4 x 4
	 */
	int rows;
	int columns;

	/**
	 * The maximum number of rows and columns the matrix
	 * can be resized to without having to reallocate space.
	 */
	int maxRows;
	int maxColumns;

public:

  /**
   * The entries of this matrix.
   * First index is the row index, the second
   * index is the column index.
   */
  scalar ** entry;

};/* End class MatrixMxN */
#endif //__MATRIX_M_N_INCLUDED__
