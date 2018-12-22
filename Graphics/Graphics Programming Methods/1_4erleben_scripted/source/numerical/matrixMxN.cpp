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
#if(_MSC_VER == 1200)
#pragma warning(disable:4244)   // No warnings on precision truncation
#pragma warning(disable:4305)   // No warnings on precision truncation
#pragma warning(disable:4786)   // stupid symbol size limitation
#endif
#include <numerical/matrixMxN.h>
#include <numerical/vectorN.h>
#include <utils/output.h>
#include <cmath>
#include <string>
using namespace std;


bool MatrixMxN::safety = true;

/**
 * Default constructor.
 */
MatrixMxN::MatrixMxN(){
  maxRows = rows = 4;
  maxColumns = columns = 4;
  entry = new scalar*[rows];
  for(int i=0;i<rows;++i)
    entry[i] = new scalar[columns];
	setZero();
};

/**
 * Deconstructor.
 */
MatrixMxN::~MatrixMxN(){
  delete [] entry;
};

/**
 * Special Constructor.
 */
MatrixMxN::MatrixMxN(const int & rows,const int & columns){
	this->maxRows    = this->rows    = rows;
	this->maxColumns = this->columns = columns;

  entry = new scalar*[rows];
  for(int i=0;i<rows;++i)
    entry[i] = new scalar[columns];
	
  setZero();
};

/**
 * Copy constructor.
 * The method creates a new matrix who's entries are copied
 * from another matrix.
 *
 * @param M             A reference to a matrix. The entries of
 *                      this matrix is copied into the newly
 *                      created matrix.
 */
MatrixMxN::MatrixMxN(const MatrixMxN & M){
	maxRows    = rows    = M.rows;
	maxColumns = columns = M.columns;
  
  entry = new scalar*[rows];
  for(int i=0;i<rows;++i)
    entry[i] = new scalar[columns];

  set(M);
};

/**
 * Resize Matrix.
 *
 * @param m   The new number of rows.
 * @param n   The new number of columns.
 */
void MatrixMxN::resize(const int & m,const int & n){
	if(safety){
		if(n<=0||m<=0){
			Output::error("MatrixMxN","resize","Invalid dimensions");
		}
	}
	if(m>maxRows || n>maxColumns){
		scalar ** newentry = 0;

    newentry = new scalar*[m];
    for(int r=0;r<m;++r)
      newentry[r] = new scalar[n];

		//--- copy old entries to newly allocated chunk of memory.
		for(int i=0;i<rows;i++){
			for(int j=0;j<columns;j++){
				newentry[i][j] = entry[i][j];
			}
		}
		entry = newentry;
		//--- Update dimension information with new values.
		this->rows    = this->maxRows    = m;
		this->columns = this->maxColumns = n;
	}else{
		//--- reduce the number of used entries.
		this->rows    = m;
		this->columns = n;
	}
};

/**
 * Assignment.
 * The method assigns the values of the entries
 * of a specified matrix into this matrix. That
 * is this = A.
 *
 * @param A        A reference to a matrix. The entries
 *                 of this matrix is assigned to this
 *                 matrix.
 */
void MatrixMxN::set(const MatrixMxN & M){
  //--- Notice that the dimensions of M must be equal
	//--- to the dimensions of this matrix.
	if(safety){
		if(M.rows!=this->rows || M.columns!=this->columns){
			Output::error("MatrixMxN","set(MatrixMxN)","Invalid dimensions");
		}
	}
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
      entry[r][c] = M.entry[r][c];
};
  
/**
 * Assignment.
 * The method assigns the values of the entries
 * of a specified matrix into this matrix. That
 * is this = A.
 *
 * @param entries  A reference to an array of scalars. This
 *                 array holds the values that should be
 *                 assigned to the matrix in row form.
 */
void MatrixMxN::set(scalar * entries,const int & length){
	//--- Notice that entries must have at least rows*columns elements.
	if(safety){
  	if(length!= (this->rows*this->columns)){
			Output::error("MatrixMxN","set(scalar[])","Invalid number of entries");
		}
	}
	for(int r=0;r<rows;r++){
		int i0 = r*rows;
    for(int c=0;c<columns;c++)
      entry[r][c] = entries[i0+c];
	}
};

/**
 * Retrieve the current number of rows in the matrix.
 */
int MatrixMxN::getRowDimension(void)const{
  return rows;
};

/**
 * Retrieve the current number of columns in the matrix.
 */
int MatrixMxN::getColumnDimension(void)const{
	return columns;
};

/**
 * Assigns a vector to a row.
 *
 * @param i       The index of the row.
 * @param row     A reference to a vector.
 */
void MatrixMxN::setRow(const int & i,const VectorN & row){
	for(int c=0;c<columns;c++)
		entry[i][c] = row.entry[c];
};

/**
 * Assigns a vectors to a column.
 *
 * @param i       The index of the colum.
 * @param column  A reference to the vector.
 */
void MatrixMxN::setColumn(const int & i,const VectorN & column){
	for(int r=0;r<rows;r++)
		entry[r][i] = column.entry[r];
};

/**
 * Assigns a vector to a row.
 *
 * @param i       The index of the row.
 * @param row     A reference to a vector.
 */
void MatrixMxN::getRow(const int & i,VectorN & row){
	for(int c=0;c<columns;c++)
		row.entry[c]= entry[i][c];
};

/**
 * Assigns a vectors to a column.
 *
 * @param i       The index of the colum.
 * @param column  A reference to the vector.
 */
void MatrixMxN::getColumn(const int & i,VectorN & column){
	for(int r=0;r<rows;r++)
		column.entry[r] = entry[r][i];
};

/**
 * Equality Comparison Test.
 * This method performs a test of the entries of
 * the two matrices in order to determine if the
 * are equal or not.
 *
 * @param M     A reference to a matrix. This is the matrix
 *              the comparison is done with.
 *
 * @return      The test result. The return value is true if
 *              the matrices are equal otherwise the return
 *              value is false.
 */
bool MatrixMxN::equals(const MatrixMxN & M){
	if(rows!=M.rows)
	  return false;
	if(columns!=M.columns)
	  return false;

	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
		  if(entry[r][c] != M.entry[r][c])
		    return false;
	return true;
};

/**
 * Equality Comparison Test.
 * This method performs a test of the entries
 * of the two matrices in order to determine if
 * the are equal or not.
 * If none of the entries differ by no more than
 * the precision then the matrices are determined
 * to be equal.
 *
 * @param M        A reference to a matrix. This is the matrix
 *                 the comparison is done with.
 *
 * @return         The test result. The return value is true if
 *                 the matrices are equal otherwise the return
 *                 value is false.
 */
bool MatrixMxN::nearlyEquals(const MatrixMxN & M){
	if(rows!=M.rows)
	  return false;
	if(columns!=M.columns)
	  return false;

	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			if(fabs(entry[r][c]-M.entry[r][c])>=precision)
			  return false;
	return true;
};

/**
 * Negation.
 * This mehod negats all the entries of this matrix.
 */
void MatrixMxN::negate(void){
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			entry[r][c] = -entry[r][c];
};

/**
 * Negation.
 * This mehod negats all the entries of specified matrix
 * and sets this matrix equal to the result. That is: A = -M.
 */
void MatrixMxN::negate(const MatrixMxN & M){
	if(safety){
		if(M.rows!=this->rows || M.columns!=this->columns){
			Output::error("MatrixMxN","negate(MatrixMxN)","Invalid dimensions");
		}
	}
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			entry[r][c] = -M.entry[r][c];
};

/**
 * Subtraction.
 *
 * @param B       A reference to a matrix whos entries
 *               should be subtracted from this matrix.
 */
void MatrixMxN::sub(const MatrixMxN & B){
  if(safety){
  	if(B.rows!=this->rows || B.columns!=this->columns){
			Output::error("MatrixMxN","sub(MatrixMxN)","Invalid dimensions");
		}
	}
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			entry[r][c] -= B.entry[r][c];
};

/**
 * Subtraction.
 * This method assigns the value of A-B to this matrix.
 *
 * @param A      A reference to a matrix.
 * @param B      A reference to a matrix.
 */
void MatrixMxN::sub(const MatrixMxN & A,const MatrixMxN & B){
  if(safety){
    if(A.rows!=this->rows || A.columns!=this->columns){
  		Output::error("MatrixMxN","sub(MatrixMxN,MatrixMxN)","Invalid dimensions");
		}
		if(B.rows!=this->rows || B.columns!=this->columns){
			Output::error("MatrixMxN","sub(MatrixMxN,MatrixMxN)","Invalid dimensions");
		}
	}
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			entry[r][c] = A.entry[r][c] - B.entry[r][c];
};

/**
 * Addition.
 *
 * @param B      A reference to a matrix whos entries
 *               should be added to this matrix.
 */
void MatrixMxN::add(const MatrixMxN & B){
  if(safety){
  	if(B.rows!=this->rows || B.columns!=this->columns){
			Output::error("MatrixMxN","add(MatrixMxN)","Invalid dimensions");
		}
	}
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			entry[r][c] += B.entry[r][c];
};

/**
 * Addition.
 * This method assigns the value of A+B to this matrix.
 *
 * @param A      A reference to a matrix.
 * @param B      A reference to a matrix.
 */
void MatrixMxN::add(const MatrixMxN & A,const MatrixMxN & B){
  if(safety){
    if(A.rows!=this->rows || A.columns!=this->columns){
  		Output::error("MatrixMxN","add(MatrixMxN,MatrixMxN)","Invalid dimensions");
		}
		if(B.rows!=this->rows || B.columns!=this->columns){
			Output::error("MatrixMxN","add(MatrixMxN,MatrixMxN)","Invalid dimensions");
		}
	}
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			entry[r][c] = A.entry[r][c] + B.entry[r][c];
};

/**
 * Scalar multiplication.
 * This mehod multiplies this matrix by a scalar. That is A = s*A.
 *
 * @param s       A scalar. This is the value that all the
 *                entries of this matrix is multiplied with.
 */
void MatrixMxN::mul(const scalar & s){
	for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
			entry[r][c] *= s;
};

/**
 * Scalar division.
 * This mehod divides this matrix by a scalar. That is A = A/s.
 *
 * @param s       A scalar. This is the value that all the entries of this matrix is divided with.
 */
void MatrixMxN::div(const scalar & s){
  if(s!=0){
  	for(int r=0;r<rows;r++)
			for(int c=0;c<columns;c++)
				entry[r][c] /= s;
  }else{
    Output::warning("MatrixMxN","div","division by zero");
  }
};

/**
 * Matrix multiplication.
 * This method multiplies two matrices and stores
 * the result in this matrix. That is C = A*B.
 *
 * @param A     A reference to a matrix.
 * @param B     A reference to a matrix.
 */
void MatrixMxN::mul(const MatrixMxN & A,const MatrixMxN & B){
	//--- Notice that the dimension of this matrix
	//--- must be A.rows X B.columns
	//--- Also A.columns must be equal to B.rows
	if(safety){
		if(this->rows!=A.rows || this->columns!=B.columns){
			Output::error("MatrixMxN","mul(MatrixMxN,MatrixMxN)","Invalid dimensions");
		}
		if(A.columns!=B.rows){
			Output::error("MatrixMxN","mul(MatrixMxN,MatrixMxN)","Invalid dimensions");
		}
	}
  for(int c=0; c<B.columns; c++)
    for(int r=0; r<A.rows; r++){
      entry[r][c] = 0.0f;
      for(int i=0; i<A.columns; i++){
        entry[r][c] += A.entry[r][i] * B.entry[i][c];
      }
    }
};

/**
 * Matrix multiplication.
 * This method pre multiplies the specified
 * matrix with this matrix. That is this = A*this->
 *
 * @param A     A reference to a matrix.
 */
void MatrixMxN::premul(const MatrixMxN & A){
	//--- Notice that A.columns must be equal to this->rows
	if(safety){
		if(A.columns!=this->rows){
			Output::error("MatrixMxN","premul()","Invalid dimensions");
		}
	}
	MatrixMxN value(A.rows,this->columns);
  for(int c=0;c<this->columns;c++)
    for(int r=0;r<A.rows;r++){
      value.entry[r][c] = 0.0f;
      for(int i=0;i<A.columns;i++){
        value.entry[r][c] += A.entry[r][i] * entry[i][c];
      }
    }
  set(value);
};
  
/**
 * Matrix multiplication.
 * This method post multiplies the specified
 * matrix with this matrix. That is this = this*B.
 *
 * @param A     A reference to a matrix.
 */
void MatrixMxN::postmul(const MatrixMxN & B){
	//--- Notice that B.rows must be equal to this->colums
	if(safety){
		if(this->columns!=B.rows){
			Output::error("MatrixMxN","premul()","Invalid dimensions");
		}
	}
	MatrixMxN value(this->rows,B.columns);
  for(int c=0;c<B.columns;c++)
    for(int r=0;r<this->rows;r++){
      value.entry[r][c] = 0.0f;
      for(int i=0;i<this->columns;i++){
        value.entry[r][c] += entry[r][i] * B.entry[i][c];
      }
    }
  set(value);
};

/**
 * Changes this matrix into its transpose. That is A = A^T
 */
void MatrixMxN::transpose(void){
  for(int r=0;r<rows;r++){
		for(int c=r+1;c<columns;c++){
			scalar temp  = entry[r][c];
			entry[r][c] = entry[c][r];
			entry[c][r] = temp;
		}
	}
};

/**
 * Sets this matrix equal to the transpose of the argument. That is this = A^T
 */
void MatrixMxN::transpose(const MatrixMxN & A){
  set(A);
  transpose();
};

/**
 * Matrix inversion.
 * This method calculates the inverse of this matrix.
 */
void MatrixMxN::inverse(void){
	//--- Test matrix dimensions.
	if(safety){
		if(this->columns!=this->rows){
			Output::error("MatrixMxN","inverse()","Invalid dimensions");
		}
	}
	//--- We are going to use LU decomposition in order
	//--- to compute the inverse matrix.
	MatrixMxN LU(rows,columns);
	int * permutation = new int[rows];

	ludcmp(LU,permutation);
	VectorN column(rows);

	for(int j=0;j<LU.columns;j++){
		column.setZero();
		column.entry[j]=1.0f;
		lubksb(LU,permutation,column);
		setColumn(j,column);
	}

  delete [] permutation;
};

/**
 * LU Decomposition.
 * This method performs a LU decomposition of this matrix. That
 * is it computes two matrices L and U such that:
 *
 * A = L U
 *
 * Where L is a lower triangular matrix with all its diagonal
 * elements set to 1 and U is an upper triangular matrix. The
 * structure of LU allow us to return the result of the algorithm
 * in a single matrix, this way we wont waste to much space.
 *
 * Inorder to make the algorithm stable a partial privot algorithm
 * is used, which is based on implicit scaling of each row. Unfortunately
 * this has the "unwanted" sideeffect that the order of the rows of
 * the computed result might be permutated. A permutation array is
 * used in order to do (and know) the booking that is involved.
 *
 * @param LU          A reference to a matrix, which upon return will
 *                    contain the computed L and U matrixes. The LU
 *                    matrix must have the same dimensions as this
 *                    matrix.
 * @param permuation  A reference to a permutation array. If entry i
 *                    holds the value j, then it means that row i of LU
 *                    is actually in row j and not in row i as it should
 *                    have been. The length of the array should have same
 *                    value as the number of rows in LU.
 *
 * @return            Upon succesfull completion of the algorithm the return
 *                    value is +1 if the number of row interchanges (due to
 *                    the pivotting) was even and -1 if they were uneven.
 */
scalar MatrixMxN::ludcmp(MatrixMxN & LU,int * permutation){
	//--- We start out by doing some simple safety tests in order
	//--- to determine if the input have the correct dimensions.
	if(safety){
		if(rows!=columns){
			Output::error("MatrixMxN","ludcmp","The matrix was not a square matrix");
		}
	  //f(permutation.length<rows){
		//	Output::error("MatrixMxN","ludcmp","Permutation array was to small");
		//}
	}

	int n = rows;
	LU.set(*this);
	//--- This vector is used to store the implicit
	//--- scaling of each row. The vector is used as
	//--- part of the pivoting strategy.
	VectorN vv(n);

	int i=0,imax=0,j=0,k=0;
	scalar big=0.0f,dum=0.0f,sum=0.0f,temp=0.0f;

	//--- Upon completion of the algorithm this variable
	//--- would have the value +1 if the number of row
	//--- interchanges (due to the pivotting) was even
	//--- and the value -1 if they were uneven.
	scalar d=1.0f;

	//--- First we are going to initialize the implicit
	//--- scaling vector, vv, and at the same time
	//--- we will test if the matrix is singular.
	for(i=0;i<n;i++){
		big=0.0f;
		for (j=0;j<n;j++){
			temp=(scalar)fabs(LU.entry[i][j]);
			if (temp > big){
				big=temp;
			}
		}
		if(big == 0.0f){
			Output::error("MatrixMxN","ludcmp","Singular matrix in routine ludcmp");
		}
		vv.entry[i]=1.0f/big;
	}

	//--- By now we are ready to begin on the decomposition.
	//--- We loop through the columns of Crout's method.
	for(j=0;j<n;j++){

		//--- Computation of all elements Uij. where i < j.
		for(i=0;i<j;i++){
			//---
			//--- The formula is:
			//---
			//---               i-1
			//---   Uij = Aij - sum(Lik Ukj)
			//---               k=0
			//---
			//--- What important to notice is that the computation
			//--- of Uij, only dependens on those elements of Ukj
			//--- with k < i. In other words only those elements in
			//--- the same colum lying above the element we are trying
			//--- to compute.
			//---
			//--- Observe that Uij is only dependent on Lik that
			//--- "lies" to the of itself.
			//---
			//--- If we draw the dependcies of Uij in the composite
			//--- matrix LU then it would look something like this:
			//---          j
			//---     |    X       |
			//---     |    X       |
			//--- i-> |XXXX#       |
			//---     |            |
			//---     |            |
			//---
			sum=LU.entry[i][j];
			for(k=0;k<=i-1;k++){
				sum -= LU.entry[i][k]*LU.entry[k][j];
			}
			LU.entry[i][j]=sum;
		}

		//--- Initialization for the search of the
		//--- largest pivot element.
		big=0.0f;

		//--- Computation of all elements Ujj. and Lij where i > j.
		//--- The formulas for Ujj and Lij is almost identifical the
		//--- only difference is that in the formula for Lij we need
		//--- to divide by Ujj and in both cases the number of terms
		//--- is different (with the maximum number for Ujj)
		//---
		//--- Observe that at this stage we have completed the
		//--- computation of all the first j-1 columns of LU and
		//--- the first i-1 rows of colum j in LU. The dependencies
		//--- are exactly the same as previously described. The beauty
		//--- of all this is that we have allready computed all the
		//--- entires, which our subsequent computations depends on.
		for(i=j;i<n;i++){
			sum = LU.entry[i][j];
			for(k=0;k<=j-1;k++){
				sum -= LU.entry[i][k]*LU.entry[k][j];
			}
			LU.entry[i][j] = sum;

			//--- This is part of the pivoting strategy, we are searching
			//--- for the row with the largest implicit scale.
			dum= vv.entry[i]*(scalar)fabs(sum);
			if(dum >= big){
				big=dum;
				imax=i;
			}
		}
		//---
		//--- Uptil now we have computed all rows as though they were
		//--- Lij, we have completely ignored Ujj. The reason is that
		//--- perhaps another row with a row index larger than j would
		//--- be a better candidate for Ujj. In order to determine that
		//--- we need to examine the result of our pivot search.
		//---
		if(j != imax){
			//--- By now we have discovered that there exist another
			//--- row, which is far better to use for the Ujj computation.
			//--- So we need to swap the two rows.
			for(k=0;k<n;k++){
				dum=LU.entry[imax][k];
				LU.entry[imax][k]=LU.entry[j][k];
				LU.entry[j][k]=dum;
			}
			d = -(d);
			vv.entry[imax]=vv.entry[j];
		}
		permutation[j]=imax;

		//--- If the pivot element is zero the matrix is singular (at
		//--- least to the precision of the algorithm). For some
		//--- applications on singular matrices, it is desirable
		//--- to substitute a small value for zero.
		//---
		//--- In other words the following little if-statement handles
		//--- the case of potentially singularity in a simple practical
		//--- way instead of halting the entire algorithm.
		if(LU.entry[j][j] == 0.0f){
			LU.entry[j][j]=tiny;
		}

		//--- The (potential) swapping we did previously has taken
		//--- care of the computation of Ujj, we know need to fix
		//--- the computations of all the remaining Lij's.
		if (j != (n-1)){
			dum=1.0f/(LU.entry[j][j]);
			for(i=j+1;i<n;i++){
				LU.entry[i][j] *= dum;
			}
		}

	}
	//--- We have finally succesfully complted the algorithm. We
	//--- finish off by letting the caller know if the number of
	//--- row interchanges was even or uneven.
	return d;
};

/**
 * LU Substitution.
 * This method solves the linear equation system L U x = b.
 *
 * An LU decomposition and its corresponding permutation can
 * be obtained by invoking the method ludcmp.
 *
 * @param LU            A reference to a matrix, which represents
 *                      a LU decomposition of some matrix A. Fortunately
 *                      we know that all diagonal elements of L have the
 *                      value 1. This is why we can represent two matrices
 *                      in a single matrix.
 * @param permutation   A reference to an array, which contains
 *                      information about any permutation of the
 *                      LU matrix. During the decomposition of a
 *                      matrix into a lower trianglular and an upper
 *                      triangular matrix, rows might get interchanged
 *                      due to the pivoting strategy.
 * @param b             A reference holding the vector for which we
 *                      seek a solution for. Note that upon return this
 *                      vector also contains the solution for x.
 */
void MatrixMxN::lubksb(MatrixMxN & LU,int * permutation,VectorN & b){
	//--- We are trying to solve the equation A x = b
	//--- where we know A x = (L U) x = L (U x) = b
	int n = LU.rows;

	//--- We start out by doing some safety tests inorder
	//--- to determine if the input have to correct
	//--- dimensions.
	if(safety){
		if(b.n!=n){
			Output::error("MatrixMxN","lubksb","Dimension of b was incorrect");
		}
		//if(permutation.length<n){
		//	Output::error("MatrixMxN","lubksb","Dimension of permutation array was incorrect");
		//}
		if(LU.rows!=LU.columns){
			Output::error("MatrixMxN","lubksb","LU cannot be a proper decomposition of a square matrix");
		}
	}

	int i=0,ii=0,ip=0,j=0;
	scalar sum=0.0f;

	//--- Perform forward substitution
	//--- Here we are solving for y in the equation L y = b
	ii=-1;

	for(i=0;i<n;i++){
		ip=permutation[i];
		sum=b.entry[ip];
		b.entry[ip]=b.entry[i];
		if(ii!=-1){
			for(j=ii;j<=i-1;j++){
				sum -= LU.entry[i][j]*b.entry[j];
			}
		}else if(sum!=0.0f){
			ii=i;
		}
		b.entry[i]=sum;
	}

	//--- Perform backward substitution
	//--- Here we are solving for x in the equation U x = y
	for(i=n-1;i>=0;i--){
		sum=b.entry[i];
		for(j=i+1;j<n;j++){
			sum -= LU.entry[i][j]*b.entry[j];
		}
		b.entry[i]=sum/LU.entry[i][i];
	}

};

/**
 * Truncates entries to zero.
 * This method forces every entry in the matrix
 * that is numerical less than the precision to
 * zero.
 */
void MatrixMxN::truncate(void){
  for(int r=0;r<rows;r++)
    for(int c=0;c<columns;c++)
      ::truncate(entry[r][c]);
};

/**
 * Set to Identity Matrix.
 */
void MatrixMxN::setIdentity(void){
	setZero();
	for(int i=0;i<rows;i++)
 		entry[i][i] = 1.0f;
};

/**
 * Set to Identity Matrix.
 */
void MatrixMxN::setZero(void){
	for(int r=0;r<rows;r++)
		for(int c=0;c<columns;c++)
			entry[r][c] = 0.0f;
};

/**
 * Orthonormalize Matrix.
 */
void MatrixMxN::orthonormalize(void){
  if(rows!=columns){
    Output::error("MatrixMxN","orthonormalize","Matrix must be a square matrix");
    return;
  }

  //--- Oh no this is quite expensive, memory waste if done multiple times...
  VectorN row0(rows);
  VectorN row1(rows);
  VectorN proj(rows);

  getRow(0,row0);
  row0.unit();
  setRow(0,row0);
  for(int r=1;r<rows;r++){
    getRow(r,row1);
    for(int c=r-1;c>=0;c--){
      getRow(c,row0);
      scalar pdot = row1.dot(row0);
      proj.set(row0);
      proj.mul(pdot);
      row1.sub(proj);
    }
    row1.unit();
    setRow(r,row1);
  }
};

/**
 * Converts this matrix into a corresponding string
 * literal.
 */
std::string MatrixMxN::toString(void){
  std::string matrix;
/*
  matrix = "[";
	//--- Fix row 0
	matrix += "[";
	matrix += ""+entry[0][0];
	for(int c=1;c<columns;c++){
		matrix += "," + entry[0][c];
	}
	matrix += "]";
	//--- fix the remaining m-1 rows
	for(int r=1;r<rows;r++){
		matrix += ",[";
		matrix += ""+entry[r][0];
    for(int c=1;c<columns;c++){
			matrix += "," + entry[r][c];
		}
		matrix += "]";
	}
	matrix += "]";
*/
  return matrix;
};
  
