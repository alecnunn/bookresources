/* Copyright (C) Greg James, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Greg James, 2001"
 */
#include <ul.h>
#include <assert.h>
#ifndef UL_BIG_H
#define UL_BIG_H

#define FLOAT_FORMAT "%10.5f "


class ulVector;
class ulMatrix;


class ulMatrix1;


class ulVector1
{
public:
    //ulVector1( const ulVector1& ); // Declare copy constructor.

    int size() { return n; }

    int n;
    int alloc_n;
    
    real * v;
    void Allocate(const int &n);
    void Release();
    void Clear();
    
    ulVector1(const int &n);
    ulVector1();
    
    // access with [] or ()
    real& operator[](int i) const;
    real & operator () (const int &i) const;
    
    
    void Resize(const int &r);
    void Resize(ulVector1 & v);
    void Init();
    void Seti( int n, ...);
    void Print(char * title = 0) ;
    
    
    ulVector1 & operator = (const ulVector1 & v);
    
    
    
    // Assignment operators
    
    
    ulVector1 & operator += ( const ulVector1& v );     // incrementation by a ulVector1
    ulVector1 & operator -= ( const ulVector1& v );     // decrementation by a ulVector1
    ulVector1 & operator *= ( const real d );     // multiplication by a constant
    ulVector1 & operator += ( const real d );     // add constant
    ulVector1 & operator /= ( const real d );     // division by a constant
    
    // special functions
    
    real GetMagnitude();         // length of a ulVector1
    real GetMagnitudeSquared();  // squared length of a ulVector1    
    
    
    
    // //friends
    
    friend ulVector1 operator - (const ulVector1& v);         // -v1
    friend ulVector1 operator + (const ulVector1& v1, const ulVector1& v2);      // v1 + v2
    friend ulVector1 operator - (const ulVector1& v, const ulVector1& v2);      // v1 - v2
    friend ulVector1 operator * (const ulVector1& v, const real d);      // v1 * d
    friend ulVector1 operator * (const real d, const ulVector1& a);      // d * v1
    
    friend ulVector1 operator * (const ulMatrix1& m, const ulVector1& v);      // M * v
    friend ulVector1 operator * (const ulVector1& v, const ulMatrix1& m);      // M(transpose) * v
    
    
    
    
    friend real operator & (const ulVector1& v1, const ulVector1& v2);    // dot product
    
    friend ulVector1 operator * (const ulVector1& v1, const ulVector1& b);    // V1 * V2
    friend ulVector1 operator / (const ulVector1& v, const real d);      // v1 / 3.0
    friend ulVector1 operator CP (const ulVector1& v1, const ulVector1& v2);      // cross product
    
    friend int operator == (const ulVector1& v1, const ulVector1& v2);      // v1 == v2 ?
    friend int operator != (const ulVector1& v1, const ulVector1& v2);      // v1 != v2 ?
    friend ostream& operator << (ostream& s, ulVector1& v);      // output to stream
    friend istream& operator >> (istream& s, ulVector1& v);      // input from strm.
    
    ulVector1& operator *= (const ulVector1& v);
    ulVector1& operator /= (const ulVector1& v);
    
    // =====================================
    // Unary operators
    // =====================================
    
    friend ulVector1 operator + (const ulVector1& v);
    friend ulVector1 operator - (const ulVector1& v);
    
    
    // =====================================
    // Binary operators
    // =====================================
    
    // Addition and subtraction
    friend ulVector1 operator + (const ulVector1& v1, const ulVector1& v2);
    friend ulVector1 operator - (const ulVector1& v1, const ulVector1& v2);
    // Scalar multiplication and division
    friend ulVector1 operator * (const ulVector1& v, real s);
    friend ulVector1 operator * (real s, const ulVector1& v);
    friend ulVector1 operator / (const ulVector1& v, real s);
    // Memberwise multiplication and division
    friend ulVector1 operator * (const ulVector1& v1, const ulVector1& v2);
    friend ulVector1 operator / (const ulVector1& v1, const ulVector1& v2);
    
    // Vector dominance
    friend int operator < (const ulVector1& v1, const ulVector1& v2);
    friend int operator <= (const ulVector1& v1, const ulVector1& v2);
    
    // Bitwise equality
    friend int operator == (const ulVector1& v1, const ulVector1& v2);
    
    
    
};

class ulVector
{
public:
    
    //private:
    
    real * v; 
    real & entry(const int &i, const int &d) const;
    
    ulVector *saved_vector;
    void Allocate(const int &r, const int &dim);
    void Release();
    
public:
    
    int alloc_n, alloc_dim, alloc_size;
    
    int n, dim, size;
    ulVector(const int &n, const int &dim);
    
    ulVector(const ulVector & v);
    ulVector() { Init();}
    
    void Init();
    void ReInit();
    void Resize(const int &r, const int &dim);
    void Resize(ulVector & v);
    
    
    void SaveVector();
    void RestoreVector();
    
    void CopyVector(const ulVector & src);
    
    void Seti( int n, int dim, ...);
    void Setd( int n, int dim, ...);
    void Setf( int n, int dim, ...);
    
    
    ~ulVector();
    void Clear ();
    
    real & operator () (const int &i, const int & dim) const;
    //real & operator () (const int &i, const int & dim);
    
    //real & operator () (const int &i, const int & dim) ;
    
    //ulVector  operator * (const real &val);
    //ulVector  operator * (const ulVector & b);
    
    //ulVector  operator / (const real &val);
    //ulVector  operator + (const ulVector & a);
    //ulVector  operator - (const ulVector & a);
    
    ulVector & operator = (const ulVector & v);
    //ulVector & operator * (const ulMatrix & b);        // this * m2
    //ulVector & operator = (const real &val);
    
    //ulVector  operator & (const ulVector  & b);
    //friend ulVector  operator - (const ulVector  & v);			    // -v1
    
    friend ulVector  operator * (const ulMatrix & a, const ulVector & v);
    friend ulVector  operator * (const ulVector & v, const ulMatrix & a);
    
    //friend ulVector  operator * (const ulVector & a, const real d);
    //friend ulVector  operator * (const real &d, const ulVector & a);
    
    //friend real operator + (const ulVector & a, const ulVector & b);
    
    
    // Assignment operators
    
    
    ulVector & operator += ( const ulVector& v );     // incrementation by a ulVector
    ulVector & operator -= ( const ulVector& v );     // decrementation by a ulVector
    ulVector & operator *= ( const real d );     // multiplication by a constant
    ulVector & operator /= ( const real d );     // division by a constant
    
    // special functions
    
    real GetMagnitude();         // length of a ulVector
    real GetMagnitudeSquared();  // squared length of a ulVector    
    
    
    
    // //friends
    
    friend ulVector operator - (const ulVector& v);         // -v1
    friend ulVector operator + (const ulVector& v1, const ulVector& v2);      // v1 + v2
    friend ulVector operator - (const ulVector& v, const ulVector& v2);      // v1 - v2
    friend ulVector operator * (const ulVector& v, const real d);      // v1 * d
    friend ulVector operator * (const real d, const ulVector& a);      // d * v1
    
    friend ulVector operator * (const ulMatrix& m, const ulVector& v);      // M * v
    friend ulVector operator * (const ulVector& v, const ulMatrix& m);      // M(transpose) * v
    
    
    
    
    friend real operator & (const ulVector& v1, const ulVector& v2);    // dot product
    
    friend ulVector operator * (const ulVector& v1, const ulVector& b);    // V1 * V2
    friend ulVector operator / (const ulVector& v, const real d);      // v1 / 3.0
    friend ulVector operator CP (const ulVector& v1, const ulVector& v2);      // cross product
    
    friend int operator == (const ulVector& v1, const ulVector& v2);      // v1 == v2 ?
    friend int operator != (const ulVector& v1, const ulVector& v2);      // v1 != v2 ?
    friend ostream& operator << (ostream& s, ulVector& v);      // output to stream
    friend istream& operator >> (istream& s, ulVector& v);      // input from strm.
    
    ulVector& operator *= (const ulVector& v);
    ulVector& operator /= (const ulVector& v);
    
    // =====================================
    // Unary operators
    // =====================================
    
    friend ulVector operator + (const ulVector& v);
    friend ulVector operator - (const ulVector& v);
    
    
    // =====================================
    // Binary operators
    // =====================================
    
    // Addition and subtraction
    friend ulVector operator + (const ulVector& v1, const ulVector& v2);
    friend ulVector operator - (const ulVector& v1, const ulVector& v2);
    // Scalar multiplication and division
    friend ulVector operator * (const ulVector& v, real s);
    friend ulVector operator * (real s, const ulVector& v);
    friend ulVector operator / (const ulVector& v, real s);
    // Memberwise multiplication and division
    friend ulVector operator * (const ulVector& v1, const ulVector& v2);
    friend ulVector operator / (const ulVector& v1, const ulVector& v2);
    
    // Vector dominance
    friend int operator < (const ulVector& v1, const ulVector& v2);
    friend int operator <= (const ulVector& v1, const ulVector& v2);
    
    // Bitwise equality
    friend int operator == (const ulVector& v1, const ulVector& v2);
    
    
    void Print(char * title = 0) ;
};


template <class _Type>
class ulTemplateMatrix 
{
    typedef ulMatrix  * ulpMatrix;
private:
    _Type **m;
    _Type * entry(const int &i, const int &j, const int & d) const;
    
};



class ulMatrix 
{
public:
    typedef ulMatrix  * ulpMatrix;
public:
    ulVector  **m;
    ulMatrix *saved_matrix;
    // internal data fetch operators
    //const real & entry(const int &i, const int &j, const int & d) const;
    //real & entry(const int &i, const int &j, const int & d) ;
    real & entry(const int &i, const int &j, const int & d) const;
    
    //real & entry(const int &i, const int &j, const int & d);
    //real * entry_ptr(const int &i, const int &j, const int & d);
    
public:
    int rows, cols, dim;
    int alloc_row, alloc_col, alloc_dim;
    
    ulMatrix () { Init();}
    void Init();
    void ReInit();
    ~ulMatrix();
    ulMatrix (const int &row, const int &col, const int &dim);
    ulMatrix (const ulMatrix & from);
    
    void Clear();
    void SaveMatrix();
    void RestoreMatrix();
    void SaveRow(int r);
    void RestoreRow(int r);
    
    real safe_entry(const int &i, const int &j, const int & d) const;
    
    //void CopyDimensionZero();
    //void CopyMatrix(const ulMatrix& src);
    
    
    
    //real & operator () (const int &i, const int &j, const int & dim) const;
    //real & operator () (const int &i, const int &j, const int & dim);
    real & operator () (const int &i, const int &j, const int & dim) const;
    //real & operator () (const int &i, const int &j, const int & dim) ;
    ulVector & operator () (const int &i);
    //ulMatrix operator * (const real &val);
    
    void GetTranspose(ulMatrix & trans);
    void SetTranspose (ulMatrix & trans) ;
    void SetIdentity ();
    void SetInverse();
    void CopyMatrix(const ulMatrix& src);
    
    
    void Resize(const int &r, const int &c, const int &dim);
    void Resize(ulMatrix & m);
    
    void Allocate(const int &r, const int &c, const int &dim);
    void Release();
    
    
    void Set(const ulMatrix &mat);
    void Seti( int r, int c, int dim, ...);
    void Setd( int r, int c, int dim, ...);
    void Setf( int r, int c, int dim, ...);
    
    //ulMatrix(const real &d);
    
    // Assignment operators
    
    ulMatrix& operator  = ( const ulMatrix & m );      // assignment of a ulMatrix
    //ulMatrix& operator += ( const ulMatrix & m );      // incrementation by a ulMatrix
    //ulMatrix& operator -= ( const ulMatrix & m );      // decrementation by a ulMatrix
    //ulMatrix& operator *= ( const real &d );      // multiplication by a constant
    //ulMatrix& operator /= ( const real &d );      // division by a constant
    
    // special functions
    
    Mat3 Rot();
    void SetRotation(const Mat3 & m);
    ulVector  Trans() ;
    
    ulVector  GetViewUp() ;
    void GetViewUp(ulVector  & v);
    
    ulVector  GetViewNormal() ;
    void GetViewNormal(ulVector  &v) ;
    
    ulVector  GetViewRight() ;
    void GetViewRight(ulVector  & v);
    
    //ulMatrix  & GetulMatrix1();
    
    //void Transpose();         // transpose
    
    void Invert();            // inverse
    HRESULT InvertArbitrary();            // inverse
    HRESULT InvertAffine();            // inverse
    void GetInverse(ulMatrix & q) ;
    HRESULT GetInverseArbitrary(ulMatrix  & q) ;
    HRESULT GetInverseAffine(ulMatrix  & q) ;
    void SetInverse(const ulMatrix  & q);
    HRESULT SetInverseArbitrary(const ulMatrix  & q);
    HRESULT SetInverseAffine(const ulMatrix  & q);
    
    //void GetTranspose(ulMatrix  & q) ;
    void SetToZero();           // make this zero
    
    void SetRotateXDeg(real degrees);
    void RotateXDeg(real degrees, bool post = false);
    
    void SetRotateYDeg(real degrees);
    void RotateYDeg(real degrees, bool post = false);
    
    void SetRotateZDeg(real degrees);
    void RotateZDeg(real degrees, bool post = false);
    
    
    void SetScale(real sx, real sy, real sz);
    void SetScale(const ulVector  & tvec);
    void Scale(real sx, real sy, real sz, bool post = false);
    void Scale(const ulVector  & tvec, bool post = false);
    
    
    
    void SetTranslate(const real &x, const real &y, const real &z);
    void SetTranslate(const ulVector  & tvec);
    
    void Translate(const real &tx, const real &ty, const real &tz, bool post = false);
    void Translate(const ulVector  & tvec, bool post = false);
    
    
    void SetTranslationPoint(const ulVector &pos);
    
    friend ulMatrix operator * (const ulMatrix & a, const ulMatrix & b);        // m1 * m2
    
    
    
    //ulMatrix & operator * (const ulMatrix & b);        // this * m2
    //ulVector & operator * (const ulVector & b);        // this * m2
    
    
    
    
    //friend ulMatrix operator - (const ulMatrix & a);         // -m1
    //friend ulMatrix operator + (const ulMatrix & a, const ulMatrix & b);      // m1 + m2
    //friend ulMatrix operator - (const ulMatrix & a, const ulMatrix & b);      // m1 - m2
    //friend ulMatrix operator * (const ulMatrix & a, const real d);      // m1 * d
    //friend ulMatrix operator * (const real d, const ulMatrix & a);      // d * m1
    //friend ulMatrix operator / (const ulMatrix & a, const real d);      // m1 / d
    //friend int operator == (const ulMatrix & a, const ulMatrix & b);      // m1 == m2 ?
    //friend int operator != (const ulMatrix & a, const ulMatrix & b);      // m1 != m2 ?
    friend ostream& operator << (ostream& s, ulMatrix & m);      // output to stream
    friend istream& operator >> (istream& s, ulMatrix & m);      // input from strm.
    
    
    real XformAxis(const ulVector  &v, int axis) ;
    void Print(char * title = 0, bool bydim = true) ;
};




class ulMatrix1
{
public:
    typedef ulMatrix1  * ulpMatrix1;
public:
    real  **m;
    real *saved_matrix;
    // internal data fetch operators
    //const real & entry(const int &i, const int &j, const int & d) const;
    //real & entry(const int &i, const int &j, const int & d) ;
    //real & entry(const int &i, const int &j, const int & d) const;
    
    //real & entry(const int &i, const int &j, const int & d);
    //real * entry_ptr(const int &i, const int &j, const int & d);
    
public:
    int rows, cols;
    int alloc_row, alloc_col;
    
    ulMatrix1 () { Init();}
    void Init();
    void ReInit();
    ~ulMatrix1();
    ulMatrix1 (const int &row, const int &col);
    ulMatrix1 (const ulMatrix1 & from);
    
    void Clear();
    void SaveMatrix();
    void RestoreMatrix();
    void SaveRow(int r);
    void RestoreRow(int r);
    
    real safe_entry(const int &i, const int &j, const int & d) const;
    
    //void CopyMatrix(const ulMatrix1& src);
    
    
    
    real & operator () (const int &i, const int &j) const;
    ulVector & operator () (const int &i);
    
    void GetTranspose(ulMatrix1 & trans);
    void SetTranspose (ulMatrix1 & trans) ;
    void SetIdentity ();
    void SetInverse();
    void CopyMatrix(const ulMatrix1& src);
    
    
    void Resize(const int &r, const int &c);
    void Resize(ulMatrix1 & m);
    
    void Allocate(const int &r, const int &c);
    void Release();
    
    
    void Set(const ulMatrix1 &mat);
    void Seti( int r, int c, ...);
    void Setd( int r, int c, ...);
    void Setf( int r, int c, ...);
    
    //ulMatrix1(const real &d);
    
    // Assignment operators
    ulMatrix1& operator  = ( const ulMatrix1 & m );      // assignment of a ulMatrix1
    //ulMatrix1& operator += ( const ulMatrix1 & m );      // incrementation by a ulMatrix1
    //ulMatrix1& operator -= ( const ulMatrix1 & m );      // decrementation by a ulMatrix1
    //ulMatrix1& operator *= ( const real &d );      // multiplication by a constant
    //ulMatrix1& operator /= ( const real &d );      // division by a constant
    
    // special functions
    
    Mat3 Rot();
    void SetRotation(const Mat3 & m);
    ulVector1  Trans() ;
    
    ulVector1  GetViewUp() ;
    void GetViewUp(ulVector1  & v);
    
    ulVector1  GetViewNormal() ;
    void GetViewNormal(ulVector1  &v) ;
    
    ulVector1  GetViewRight() ;
    void GetViewRight(ulVector1  & v);
    
    ulMatrix1  & GetulMatrix1();
    
    //void Transpose();         // transpose
    
    void Invert();            // inverse
    HRESULT InvertArbitrary();            // inverse
    HRESULT InvertAffine();            // inverse
    void GetInverse(ulMatrix1 & q) ;
    HRESULT GetInverseArbitrary(ulMatrix1  & q) ;
    HRESULT GetInverseAffine(ulMatrix1  & q) ;
    void SetInverse(const ulMatrix1  & q);
    HRESULT SetInverseArbitrary(const ulMatrix1  & q);
    HRESULT SetInverseAffine(const ulMatrix1  & q);
    
    //void GetTranspose(ulMatrix1  & q) ;
    void SetToZero();           // make this zero
    
    void SetRotateXDeg(real degrees);
    void RotateXDeg(real degrees, bool post = false);
    
    void SetRotateYDeg(real degrees);
    void RotateYDeg(real degrees, bool post = false);
    
    void SetRotateZDeg(real degrees);
    void RotateZDeg(real degrees, bool post = false);
    
    
    void SetScale(real sx, real sy, real sz);
    void SetScale(const ulVector1  & tvec);
    void Scale(real sx, real sy, real sz, bool post = false);
    void Scale(const ulVector1  & tvec, bool post = false);
    
    
    
    void SetTranslate(const real &x, const real &y, const real &z);
    void SetTranslate(const ulVector1  & tvec);
    
    void Translate(const real &tx, const real &ty, const real &tz, bool post = false);
    void Translate(const ulVector1  & tvec, bool post = false);
    
    
    void SetTranslationPoint(const ulVector1 &pos);
    
    friend ulMatrix1 operator + (const ulMatrix1 & a, const ulMatrix1 & b);        // m1 * m2
    
    
    
    //ulMatrix1 & operator * (const ulMatrix1 & b);        // this * m2
    //ulVector1 & operator * (const ulVector1 & b);        // this * m2
    
    
    friend ulMatrix1 operator * (const ulMatrix1 & a, const ulMatrix1 & b);      // m1 - m2
    
    
    //friend ulMatrix1 operator - (const ulMatrix1 & a);         // -m1
    //friend ulMatrix1 operator + (const ulMatrix1 & a, const ulMatrix1 & b);      // m1 + m2
    //friend ulMatrix1 operator * (const ulMatrix1 & a, const real d);      // m1 * d
    //friend ulMatrix1 operator * (const real d, const ulMatrix1 & a);      // d * m1
    //friend ulMatrix1 operator / (const ulMatrix1 & a, const real d);      // m1 / d
    //friend int operator == (const ulMatrix1 & a, const ulMatrix1 & b);      // m1 == m2 ?
    //friend int operator != (const ulMatrix1 & a, const ulMatrix1 & b);      // m1 != m2 ?
    friend ostream& operator << (ostream& s, ulMatrix1 & m);      // output to stream
    friend istream& operator >> (istream& s, ulMatrix1 & m);      // input from strm.
    
    
    real XformAxis(const ulVector1  &v, int axis) ;
    void Print(char * title = 0) ;
};




inline real & ulMatrix1::operator () (const int &i, const int &j) const
{
    return m[i][j];
}



//void CopyToDimensionZero(ulMatrix  &dst, ulMatrix  & src);
int BigInvert(const ulMatrix & a, ulMatrix & b);

//void MatrixCrossProduct(ulMatrix & N, ulMatrix & Tu, ulMatrix & Tv,
//                        ulMatrix & x1, ulMatrix & x2, bool neg_normal);

void RationalMatrixCrossProduct(ulMatrix rawData, 
                                int u, int v, ulMatrix & normalPatch, ulMatrix & derivU, ulMatrix & derivV,
                                bool neg_normal);


void MatrixCrossProduct(ulMatrix & N, ulMatrix & Tu, ulMatrix & Tv,
                        bool neg_normal);




inline real & ulMatrix::entry(const int &i, const int &j, const int & d) const
{
#if _DEBUG
    assert (i < rows);
    assert (j < cols);
    assert (d < dim);
#endif  
    
    ulVector &p = *m[i];
    return p(j, d);
    //return m[i].v[j * dim + d];
    
}

inline real ulMatrix::safe_entry(const int &i, const int &j, const int & d) const
{
    if (i < 0 || i >= rows)
        return 0;
    
    else if (j < 0 || j >= cols)
        return 0;
    
    else if (d < 0 || d >= dim)
        return 0;
    
    else 
        return entry(i, j, d);
}


inline real & ulMatrix::operator () (const int &i, const int &j, const int & d) const
{
    return entry(i, j, d);
}


inline ulVector & ulMatrix::operator () (const int &i) 
{
    return *m[i];
}


inline real & ulVector::entry(const int &i, const int & d) const
{
#if _DEBUG
    assert (i < n);
    assert(d < dim);
    assert(v != 0);
#endif
    
    return v[i * dim + d];
} 














inline real & ulVector::operator () (const int &i, const int & d) const
//inline real & ulVector::operator () (const int &i, const int & d) 
{
    return entry(i, d);
}



inline void ulVector::Resize(ulVector & v)
{
    Resize(v.n, v.dim);
}
inline void ulVector::Resize(const int & elements, const int & d)
{
    if (elements * d > alloc_size)
    {
        Release();
        Allocate(elements, d);
    }
    else
    {
        n = elements;
        dim = d;
        size = n * d;
    } 
    
}





inline void ulMatrix::Resize(const int &r, const int &c, const int &d)
{       
    int i;
    int nr, nc, nd;
    
    if (r > alloc_row || ((c * d) > (alloc_col * alloc_dim)))
    {
        nr = ulMax(r, alloc_row);
        nc = ulMax(c, alloc_col);
        nd = ulMax(d, alloc_dim);
        
        Release();
        
        Allocate(nr, nc, nd);
    }
    
    rows = r;
    cols = c;     
    dim = d;   
    
    for(i=0; i<rows; i++)
        m[i]->Resize(cols, dim);
    
}


inline void ulMatrix::Resize(ulMatrix & src)
{
    Resize(src.rows, src.cols, src.dim);
}



inline void ulCopyVector(ulVector & dst, const ulVector & src)
{
    int i;
    dst.Resize(src.n, src.dim);
    
    for(i=0; i<src.size; i++)
        dst.v[i] = src.v[i]; 
} 



inline void ulVector::CopyVector(const ulVector & src)
{
    int i;
    Resize(src.n, src.dim);
    
    for(i=0; i<src.size; i++)
        v[i] = src.v[i]; 
} 





inline void ulCopyMatrix(ulMatrix & dest, const ulMatrix& src)
{
    int i;
    dest.Resize(src.rows, src.cols, src.dim);
    
    // copy all vectors
    for(i=0; i<src.rows; i++)
    {
        ulCopyVector(*dest.m[i], *src.m[i]);
    }
}

inline void ulMatrix::CopyMatrix(const ulMatrix& src)
{
    int i;
    Resize(src.rows, src.cols, src.dim);
    
    // copy all vectors
    for(i=0; i<src.rows; i++)
    {
        ulCopyVector(*m[i], *src.m[i]);
    }
}




inline void ulMatrixMultiply(ulMatrix & c, const ulMatrix & a, const ulMatrix & b)
{
    real sum;
    c.Resize(a.rows, b.cols, ulMax(a.dim, b.dim));
    //assert(a.row, b.col, ulMax(a.dim, b.dim));
    
    // special case of dim 1 and non dim 1
    
#if _DEBUG
    assert(a.cols == b.rows);
#endif
    int	d, i, j, k;
    
    if (a.dim == 1 && b.dim > 1)
    {
        // dimension a is duplicated
        for(d = 0; d < b.dim; d++)
        {
            for(i = 0; i < a.rows; i++)
            {
                for(j = 0; j < b.cols; j++)
                {
                    sum = 0;
                    for(k = 0; k < a.cols; k++)
                        sum += a(i, k, 0) * b(k, j, d);
                    
                    c(i, j, d) = sum;
                }
            } 
        }
        
    }
    else if (a.dim > 1 && b.dim == 1)
    {
        
        for(d = 0; d < a.dim; d++)
        {
            for(i = 0; i < a.rows; i++)
            {
                for(j = 0; j < b.cols; j++)
                {
                    sum = 0;
                    for(k = 0; k < a.cols; k++)
                        sum += a(i, k, d) * b(k, j, 0);
                    
                    c(i, j, d) = sum;
                }
            } 
        }
    }
    else
    {
        
#if _DEBUG
        assert(a.dim == b.dim);
#endif
        
        for(d = 0; d < a.dim; d++)
        {
            for(i = 0; i < a.rows; i++)
            {
                for(j = 0; j < b.cols; j++)
                {
                    sum = 0;
                    for(k = 0; k < a.cols; k++)
                        sum += a(i, k, d) * b(k, j, d);
                    
                    c(i, j, d) = sum;
                }
            } 
        }
    }
}




inline void ulVector1::Resize(ulVector1 & v)
{
    Resize(v.n);
}
inline void ulVector1::Resize(const int & elements)
{
    if (elements > alloc_n)
    {
        Release();
        Allocate(elements);
    }
    else
        n = elements;
    
}




//void ulMatrixMultiplyXYZ_W(ulMatrix & c, const ulMatrix & a, const ulMatrix & b);


void ulMatrixMultiplyXYZ_W(ulMatrix & c, const ulMatrix & a, 
                           int or1, int oc1,
                           const ulMatrix & b, 
                           int or2, int oc2);




void ulMatrixMultiplyVector (ulVector1 & av, const ulMatrix1& a, const ulVector1& v);


void ulMatrixMultiply(ulMatrix & c, const ulMatrix & a, const ulMatrix & b);
void ulMatrixMultiplyVector (ulVector & av, const ulMatrix& a, const ulVector& v);
void ulMatrixMultiplyVector (ulVector & av, const ulVector& v, const ulMatrix& a );
void ulMatrixSubtract(ulMatrix & ret, const ulMatrix & a, const ulMatrix & b);
void ulCopyMatrix(ulMatrix & dest, const ulMatrix& src);
void ulCopyVector(ulVector & dest, const ulVector& src);

void CopyRowToVector(ulVector & dest, const ulMatrix& src, int r, int start_col, int n);
void CopyVectorToRow(ulMatrix& dest, const ulVector & src, int r, int start_col, int n);

void CopyColToVector(ulVector & dest, const ulMatrix& src, int c, int start_row, int n);
void CopyVectorToCol(ulMatrix& dest, const ulVector & src, int c, int start_row, int n);

void ulMakeSquare(ulMatrix & dest, ulMatrix & src);


#endif

