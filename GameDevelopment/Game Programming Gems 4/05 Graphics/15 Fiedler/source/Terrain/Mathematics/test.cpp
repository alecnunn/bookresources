// mathematics regression test

#pragma warning( disable : 4127 )  // conditional expression is constant
#pragma warning( disable : 4100 )  // unreferenced formal parameter
#pragma warning( disable : 4702 )  // unreachable code

#ifndef _DEBUG
#pragma warning( disable : 4189 )  // local variable is initialized but not referenced (because of asserts)
#endif

#include "Matrix.h"
#include "Vector.h"
#include "Complex.h"
#include "Quaternion.h"
#include "Plane.h"
#include "Box.h"
#include "Sphere.h"
#include "Ellipsoid.h"
#include "Intersection.h"

#include <assert.h>

using namespace Mathematics;


void main()
{
	// test vector class

	Vector a(1,2,3);
	Vector b(4,5,6);
	Vector c(7,8,9);

	printf("testing vector negate\n");

	c = a;
	c.negate();
	c.multiply(-1);
	assert(c==a);

	printf("testing vector add\n");

	c = a;
	c.add(b);
	
	printf("testing vector subtract\n");

	c.subtract(b);
	assert(c==a);

	printf("testing vector scalar multiply\n");

	c = a;
	c.multiply(10.0f);
	assert(c.x==10*a.x);
	assert(c.y==10*a.y);
	assert(c.z==10*a.z);
	
	printf("testing vector scalar divide\n");

	c = a;
	c.divide(10.0f);
	assert(Float::equal(c.x,a.x/10));
	assert(Float::equal(c.y,a.y/10));
	assert(Float::equal(c.z,a.z/10));

	printf("testing vector dot product\n");

	Vector i(1,0,0);
	Vector j(0,1,0);
	Vector k(0,0,1);
	assert(i.dot(j)==0);
	assert(j.dot(k)==0);
	assert(k.dot(i)==0);

	i = a;
	i.normalize();
	j = i;
	j.multiply(-1);
	assert(Float::equal(i.dot(j), -1.0f));

	printf("testing vector cross product\n");

	i = Vector(1,0,0);
	j = Vector(0,1,0);
	k = Vector(0,0,1);

	Vector result;
	i.cross(j, result);
	assert(result==k);

	j.cross(k, result);
	assert(result==i);

	k.cross(i, result);
	assert(result==j);

	printf("testing vector length squared\n");

	assert(Float::equal(i.length_squared(), 1));
	assert(Float::equal(j.length_squared(), 1));
	assert(Float::equal(k.length_squared(), 1));

	printf("testing vector length\n");

	c = a;
	c.multiply(10);
	assert(Float::equal(a.length() * 10, c.length()));

	printf("testing vector normalize\n");

	c.normalize();
	assert(Float::equal(c.length(), 1));

	printf("testing vector operators\n");
	{
		Vector u = Vector(1,2,3);
		Vector v = Vector(2,3,4);
		Vector w = Vector(3,4,5);
		Vector zero = Vector(0,0,0);

		const float a = 5;
		const float b = 3;
		const float c = 2;

		assert(u+u==2*u);
		assert(v+v==2*v);
		assert(w+w==2*w);

		assert(a*u + b*v + c*w == u*a + v*b + w*c);
		assert(a*u - b*v - c*w == u*a - v*b - w*c);
		assert(a*(u + v + w) == a*u + a*v + a*w);
		assert(a*(u + v + w) == u*a + v*a + w*a);
		assert(u+(v+w) == (u+v)+w);
		assert(u-(v-w) == (u-v)+w);
		assert(zero - v == -v);

		Vector i(1,0,0);
		Vector j(0,1,0);
		Vector k(0,0,1);
		assert(i*j==k);
		assert(j*k==i);
		assert(k*i==j);
		assert(j*i==-k);
		assert(k*j==-i);
		assert(i*k==-j);

		Vector x = u;
		x += v;
		assert(x==u+v);

		x = u;
		x -= v;
		assert(x==u-v);

		x = i;
		x *= j;
		assert(x==i*j);

		x = u;
		x *= a;
		assert(x==u*a);

		x = u;
		x /= a;
		assert(x==u/a);
	}

	printf("testing vector orthonormal basis");
	{
		Vector u(1,2,3);
		u.normalize();

		Vector v,w;

		Vector::orthonormal_basis(u,v,w);
	}

	// test matrix class

	printf("testing matrix zero\n");

	a = Vector(1,1,1);
	Matrix matrix;
	matrix.zero();
	matrix.transform(a);
	assert(a==Vector(0,0,0));

	printf("testing matrix identity\n");

	a = Vector(8,6,5);
	matrix.zero();
	matrix.transform(a);
	assert(a==Vector(0,0,0));

	printf("testing matrix translate\n");

	a = Vector(1,2,3);
	b = Vector(1,1,1);
	c = a;
	c.add(b);
	matrix.identity();
	matrix.translate(b.x, b.y, b.z);
	matrix.transform(a);
	assert(a==c);

	printf("testing matrix scale\n");

	a = Vector(1,2,3);
	b = a;
	b.multiply(3);
	matrix.scale(3);
	matrix.transform(a);
	assert(a==b);

	a = Vector(1,2,3);
	b = a;
	b.x *= 3;
	b.y *= 4;
	b.z *= 5;
	matrix.diagonal(3,4,5);
	matrix.transform(a);
	assert(a==b);

	printf("testing matrix determinant\n");

	a = Vector(1,2,3);
	matrix = Matrix(a, a, a);
	assert(Float::equal(matrix.determinant(),0));

	b = Vector(3,2,1);
	matrix = Matrix(a, a, b);
	assert(Float::equal(matrix.determinant(),0));

	c = Vector(5,5,6);
	matrix = Matrix(a, b, c);
	assert(!Float::equal(matrix.determinant(),0));

	printf("testing matrix inverse\n");
	
	Matrix product;
	Matrix inverse;
	matrix.inverse(inverse);
	matrix.multiply(inverse, product);
	Matrix identity;
	identity.identity();
	assert(product==identity);
	Matrix output;
	inverse.inverse(output);
	assert(output==matrix);

	printf("testing matrix transpose\n");

	Matrix transpose;
	matrix.transpose(transpose);
	transpose.transpose(output);
	assert(output==matrix);

	printf("testing matrix transform\n");

	Matrix A(matrix);		// Ax = b <-> x = Ainv b
	Vector x(1,2,3);
	A.transform(x, b);
	result = x;
	A.transform(result);
	assert(b==result);
	Matrix Ainv;
	A.inverse(Ainv);
	Vector y;
	Ainv.transform(b, y);
	assert(x==y);

	printf("testing matrix multiply and add\n");

	Matrix B(Vector(1,5,3), Vector(0,1,2), Vector(5,6,10));
	x = Vector(1,2,3);
	Vector AplusB_x, Ax_plus_Bx;
	Matrix AplusB;
	AplusB = A;
	AplusB.add(B);
	AplusB.transform(x,AplusB_x);
	Vector Ax, Bx;
	A.transform(x,Ax);
	B.transform(x,Bx);
	Ax_plus_Bx = Ax;
	Ax_plus_Bx.add(Bx);
	assert(AplusB_x==Ax_plus_Bx);

	printf("testing matrix multiply and subtract\n");

	B = Matrix(Vector(1,5,3), Vector(0,1,2), Vector(5,6,10));
	x = Vector(1,2,3);
	Vector AminusB_x, Ax_minus_Bx;
	Matrix AminusB;
	AminusB = Matrix(A);
	AminusB.subtract(B);
	AminusB.transform(x,AminusB_x);
	A.transform(x,Ax);
	B.transform(x,Bx);
	Ax_minus_Bx = Ax;
	Ax_minus_Bx.subtract(Bx);
	assert(AminusB_x==Ax_minus_Bx);

	printf("testing matrix multiply and scalar multiply\n");

	const float s = 10;
	Vector s_Ax, sA_x;
	Matrix sA = A;
	sA.multiply(s);
	sA.transform(x,sA_x);
	A.transform(x,s_Ax);
	s_Ax.multiply(s);
	assert(s_Ax==sA_x);

	{
		printf("testing matrix rotate\n");

		Vector x(1,2,3);

		Vector axis(2,3,4);
		axis.normalize();

		Matrix R;
		R.rotate(2*Float::pi, axis);
		assert(R*x==x);
		assert(x*R==x);

		const float angle = 0.1f;

		R.rotate(angle, axis);
		Vector _x = R*x;
		R.rotate(angle, -axis);
		_x = R*_x;
		assert(_x==x);

		R.rotate(angle, Vector(1,0,0));
		_x = R*x;
		assert(Float::equal(_x.x,x.x));
		assert(Float::equal(_x.length(), x.length()));

		R.rotate(angle, Vector(0,1,0));
		_x = R*x;
		assert(Float::equal(_x.y,x.y));
		assert(Float::equal(_x.length(), x.length()));

		R.rotate(angle, Vector(0,0,1));
		_x = R*x;
		assert(Float::equal(_x.z,x.z));
		assert(Float::equal(_x.length(), x.length()));
	}

	// test projection matrix

	// test orthogonal matrix

	// test lookat matrix

	printf("testing matrix operators\n");
	{
		Matrix A( 1,0,0,
			      2,1,2,
				  3,2,1 );

		Matrix B( 4,0,0,
			      0,2,0,
				  0,0,3 );

		Matrix C( 5,6,2,
			      2,5,2,
				  1,2,3 );

		const float a = 2;
		const float b = 3;
		const float c = 4;

		Matrix zero;
		zero.zero();

		Matrix identity;
		identity.identity();

		Vector x(1,2,3);
		Vector y(2,3,4);
		Vector z(3,4,5);

		Vector result;
		A.transform(x, result);
		assert(result==A*x);

		assert(B.transpose()*A.transpose()==(A*B).transpose());
		assert(A.transpose()*B.transpose()==(B*A).transpose());

		assert(B.inverse()*A.inverse()==(A*B).inverse());
		assert(A.inverse()*B.inverse()==(B*A).inverse());

		assert(A.inverse()*A==identity);
		assert(A*A.inverse()==identity);
		
		assert(identity.inverse()==identity);

		result = x;
		result *= A;
		assert(result==A*x);

		assert(A*zero==zero);
		assert(zero*A==zero);

		assert(a*A+a*B==a*(A+B));
		assert(a*A-a*B==a*(A-B));

		assert(A*a+B*a==(A+B)*a);
		assert(A*a-B*a==(A-B)*a);
		assert(A/a+B/a==(A+B)/a);
		assert(A/a-B/a==(A-B)/a);

		assert((-A)==-1*A);
		assert(A+(-A)==zero);
		assert(A-(-A)==2*A);

		Matrix matrix = A;
		matrix += B;
		assert(matrix==A+B);

		matrix = A;
		matrix -= B;
		assert(matrix==A-B);

		matrix = A;
		matrix *= B;
		assert(matrix==A*B);

		matrix = A;
		matrix *= a;
		assert(matrix==A*a);

		matrix = A;
		matrix /= a;
		assert(matrix==A/a);

		assert((A+B)*x==A*x+B*x);
		assert((A-B)*x==A*x-B*x);

		assert(x*(A+B)==x*A+x*B);
		assert(x*(A-B)==x*A-x*B);

		assert(x==A.inverse()*(A*x));

		assert(a*(A*x) + b*(B*y) + c*(C*z) == (a*A)*x + (b*B)*y + (c*C)*z);
		assert(a*(A*x) - b*(B*y) - c*(C*z) == (a*A)*x - (b*B)*y - (c*C)*z);

		assert(a*(A*x) + b*(B*y) + c*(C*z) == A*(a*x) + B*(b*y) + C*(c*z));
		assert(a*(x*A) - b*(y*B) - c*(z*C) == (a*x)*A - (b*y)*B - (c*z)*C);

		assert(x*A==A.transpose()*x);
		assert(y*B==B.transpose()*y);
		assert(z*C==C.transpose()*z);
	}

	// test quaternion class
	{
		{
			printf("testing quaternion identity\n");

			Vector a(8,6,5);
			Vector b;
			Quaternion quaternion;
			quaternion.identity();
			matrix = quaternion.matrix();
			matrix.transform(a,b);
			assert(b==a);

			Quaternion p(1,2,3,4);
			assert(p*identity==p);
			assert(identity*p==p);

			// todo: test quaternion zero, what properties does it have ?
		}

		printf("testing quaternion add\n");

		Quaternion a(1,2,3,4);
		Quaternion b(2,3,4,5);
		Quaternion c(3,4,5,6);

		const float s = 5;

		Quaternion q;
		q = a;
		q.add(b);
		assert(q.w==a.w+b.w);
		assert(q.x==a.x+b.x);
		assert(q.y==a.y+b.y);
		assert(q.z==a.z+b.z);

		printf("testing quaternion subtract\n");

		q = a;
		q.subtract(b);
		assert(q.w==a.w-b.w);
		assert(q.x==a.x-b.x);
		assert(q.y==a.y-b.y);
		assert(q.z==a.z-b.z);

		printf("testing quaternion scalar multiply\n");

		q = a;
		q.multiply(s);
		assert(q.w==a.w*s);
		assert(q.x==a.x*s);
		assert(q.y==a.y*s);
		assert(q.z==a.z*s);

		printf("testing quaternion scalar divide\n");

		q = a;
		q.divide(s);
		assert(Float::equal(q.w,a.w/s));
		assert(Float::equal(q.x,a.x/s));
		assert(Float::equal(q.y,a.y/s));
		assert(Float::equal(q.z,a.z/s));

		printf("testing quaternion multiply\n");

		q = Quaternion(0,1,0,0);
		q.multiply(q);
		assert(q==Quaternion(-1,0,0,0));

		q = Quaternion(0,0,1,0);
		q.multiply(q);
		assert(q==Quaternion(-1,0,0,0));

		q = Quaternion(0,0,0,1);
		q.multiply(q);
		assert(q==Quaternion(-1,0,0,0));

		q = Quaternion(1,0,0,0);
		q.multiply(q);
		assert(q==Quaternion(1,0,0,0));

		q = Quaternion(0,1,0,0);
		q.multiply(Quaternion(0,0,1,0));
		assert(q==Quaternion(0,0,0,1));

		q = Quaternion(0,0,1,0);
		q.multiply(Quaternion(0,1,0,0));
		assert(q==Quaternion(0,0,0,-1));

		q = Quaternion(0,0,0,1);
		q.multiply(Quaternion(0,1,0,0));
		assert(q==Quaternion(0,0,1,0));

		q = Quaternion(0,1,0,0);
		q.multiply(Quaternion(0,0,0,1));
		assert(q==Quaternion(0,0,-1,0));

		Quaternion result;
		q = Quaternion(0,1,0,0);
		q.multiply(q,result);
		assert(result==Quaternion(-1,0,0,0));

		q = Quaternion(0,0,1,0);
		q.multiply(q,result);
		assert(result==Quaternion(-1,0,0,0));

		q = Quaternion(0,0,0,1);
		q.multiply(q,result);
		assert(result==Quaternion(-1,0,0,0));

		q = Quaternion(1,0,0,0);
		q.multiply(q,result);
		assert(result==Quaternion(1,0,0,0));

		q = Quaternion(0,1,0,0);
		q.multiply(Quaternion(0,0,1,0),result);
		assert(result==Quaternion(0,0,0,1));

		q = Quaternion(0,0,1,0);
		q.multiply(Quaternion(0,1,0,0),result);
		assert(result==Quaternion(0,0,0,-1));

		q = Quaternion(0,0,0,1);
		q.multiply(Quaternion(0,1,0,0),result);
		assert(result==Quaternion(0,0,1,0));

		q = Quaternion(0,1,0,0);
		q.multiply(Quaternion(0,0,0,1),result);
		assert(result==Quaternion(0,0,-1,0));

		printf("testing quaternion conjugate\n");

		assert(a.conjugate().conjugate()==a);

		assert(Float::equal(a.conjugate().w,a.w));
		assert(Float::equal(a.conjugate().x,-a.x));
		assert(Float::equal(a.conjugate().y,-a.y));
		assert(Float::equal(a.conjugate().z,-a.z));

		printf("testing quaternion norm\n");

		Quaternion p, pq;
		p = a;
		q = b;
		p.multiply(q,pq);
		assert(Float::equal(p.norm()*q.norm(),pq.norm()));

		a.conjugate(result);
		assert(result.norm()==a.norm());

		printf("testing quaternion length\n");

		assert(Float::equal(p.length(),Float::sqrt(p.norm())));

		printf("testing quaternion normalize\n");
		{
			Quaternion n = p;
			n.normalize();
			assert(n.normalized());
			assert(n.conjugate()==n.inverse());

			n = q;
			n.normalize();
			assert(n.normalized());
			assert(n.conjugate()==n.inverse());
		}

		printf("testing quaternion inverse\n");

		assert(a.inverse().inverse()==a);

		q.multiply(q.inverse());
		assert(q==1);

		Quaternion q_inv = q.inverse();
		q_inv.multiply(q);
		assert(q_inv==1);

		printf("testing quaternion operators\n");
		{
			Quaternion p(1,2,3,4);
			Quaternion q(2,3,4,5);
			Quaternion r(3,4,5,6);

			const float a = 5;

			assert(p-p==0);
			assert(q-q==0);
			assert(r-r==0);

			assert(p+p==2*p);
			assert(q+q==2*q);
			assert(r+r==2*r);

			assert(p+q==q+p);
			assert(p-q==-(q-p));

			assert(a*p==p*a);
			assert(a*p+a*q+a*r==a*(p+q+r));
			assert(p*a+q*a+r*a==(p+q+r)*a);

			assert(p/a==p*1.0f/a);
			assert(p/a+q/a+r/a==(p+q+r)/a);

			assert(Quaternion(0,1,0,0)*Quaternion(0,1,0,0)==-1);
			assert(Quaternion(0,0,1,0)*Quaternion(0,0,1,0)==-1);
			assert(Quaternion(0,0,0,1)*Quaternion(0,0,0,1)==-1);
			
			assert(Quaternion(0,1,0,0)*Quaternion(0,0,1,0)==Quaternion(0,0,0,1));
			assert(Quaternion(0,0,1,0)*Quaternion(0,1,0,0)==Quaternion(0,0,0,-1));

			assert(Quaternion(0,0,1,0)*Quaternion(0,0,0,1)==Quaternion(0,1,0,0));
			assert(Quaternion(0,0,0,1)*Quaternion(0,0,1,0)==Quaternion(0,-1,0,0));

			assert(Quaternion(0,0,0,1)*Quaternion(0,1,0,0)==Quaternion(0,0,1,0));
			assert(Quaternion(0,1,0,0)*Quaternion(0,0,0,1)==Quaternion(0,0,-1,0));

			assert((p*q).conjugate()==q.conjugate()*p.conjugate());
			assert((q*p).conjugate()==p.conjugate()*q.conjugate());
			assert((q*r).conjugate()==r.conjugate()*q.conjugate());
			assert((p*q).conjugate()==q.conjugate()*p.conjugate());
			assert((p*r).conjugate()==r.conjugate()*p.conjugate());
			assert((r*p).conjugate()==p.conjugate()*r.conjugate());

			assert(p.norm()*q.norm()==(p*q).norm());

			assert(p*p.inverse()==1);
			assert(q*q.inverse()==1);
			assert(r*r.inverse()==1);

			assert(p.inverse()==p.conjugate()/p.norm());
			assert(q.inverse()==q.conjugate()/q.norm());
			assert(r.inverse()==r.conjugate()/r.norm());

			assert((p*q).inverse()==q.inverse()*p.inverse());
			assert((q*p).inverse()==p.inverse()*q.inverse());
			assert((q*r).inverse()==r.inverse()*q.inverse());
			assert((p*q).inverse()==q.inverse()*p.inverse());
			assert((p*r).inverse()==r.inverse()*p.inverse());
			assert((r*p).inverse()==p.inverse()*r.inverse());

			assert(p.w == (p+p.conjugate())/2);
			assert(q.w == (q+q.conjugate())/2);
			assert(r.w == (r+r.conjugate())/2);

			Quaternion t;
			t = p;
			t += q;
			assert(t==p+q);

			t = p;
			t -= q;
			assert(t==p-q);

			t = p;
			t *= q;
			assert(t==p*q);			// todo: decide on definition of quaternion *= quaternion

			t = p;
			t *= a;
			assert(t==p*a);

			t = p;
			t /= a;
			assert(t==p/a);
		}

		printf("testing quaternion dot\n");

		assert(a.dot(b)==(a*b.conjugate()).w);

		// todo: axis angle <-> quaternion conversion

		// todo: quaternion <-> matrix conversion
		
		// todo: euler <-> quaternion conversion
	}

	// test complex class

	printf("testing complex axioms\n");
	{
		Complex a(1,1);
		Complex b(2,3);
		Complex c(-2,-1);
		Complex d(2,1);

		// 1. commutativity of addition
		
		assert(a+b==b+a);				

		// 2. associativity of addition

		assert((a+b)+c==a+(b+c));

		// 3. existence of zero

		assert(a+0==0+a && 0+a==a);
		assert(a*0==0*a && 0*a==0);

		// 4. existence of negatives
        
		assert(a+(-a)==(-a)+a && (-a)+a==0);

		// 5. commutativity of multiplication

		assert(a*b==b*a);

		// 6. associativity of multiplication

		assert((a*b)*c==a*(b*c));

		// 7. existance of identity

		assert(a*Complex(1,0)==Complex(1,0)*a && Complex(1,0)*a==a);

		// 8. existance of multiplicative inverse

		assert(a*a.inverse() == Complex(1,0));

		// 9. distributivity

		assert(a*(b+c)==a*b+a*c);
	}
		
	printf("testing plane primitive\n");
	{
		Plane plane(Vector(0,0,10), Vector(0,0,1));
		assert(Float::equal(plane.distance(Vector(0,0,10)),0));
		assert(Float::equal(plane.distance(Vector(0,0,0)),-10));
		assert(Float::equal(plane.distance(Vector(0,0,20)),10));
		
		assert(plane.normal==Vector(0,0,1));
	}

	printf("testing sphere primitive\n");
	{
		Sphere sphere(Vector(1,2,3), 1);
		assert(Float::equal(sphere.distance(sphere.center), -sphere.radius));
		assert(Float::equal(sphere.distance(sphere.center + Vector(0,0,1)), 0));
		assert(Float::equal(sphere.distance(sphere.center + Vector(0,2,0)), 1));
		assert(Float::equal(sphere.distance(sphere.center + Vector(-1,0,0)), 0));
		assert(Float::equal(sphere.distance(sphere.center + Vector(-5,0,0)), 4));
	}

	printf("testing ellipsoid primitive\n");
	{
		Quaternion quaternion(1,2,3,4);
		quaternion.normalize();
		Matrix matrix = quaternion.matrix();

		Ellipsoid ellipsoid(Vector(1,2,3), 1, 2, 3, quaternion);

		Vector i(1,0,0);
		Vector j(0,1,0);
		Vector k(0,0,1);
		
		Vector a = ellipsoid.matrix * i;
		Vector b = ellipsoid.matrix * j;
		Vector c = ellipsoid.matrix * k ;

	 	assert(Float::equal(ellipsoid.distance(a),0));
		assert(Float::equal(ellipsoid.distance(b),0));
		assert(Float::equal(ellipsoid.distance(c),0));

		assert(Float::equal(ellipsoid.distance(a+(ellipsoid.center-a)/2), -1.0f/2.0f));
		assert(Float::equal(ellipsoid.distance(b+(ellipsoid.center-b)/2), -2.0f/2.0f));
		assert(Float::equal(ellipsoid.distance(c+(ellipsoid.center-c)/2), -3.0f/2.0f));

		assert(Float::equal(ellipsoid.distance(a-(ellipsoid.center-a)/2), 1.0f/2.0f));
		assert(Float::equal(ellipsoid.distance(b-(ellipsoid.center-b)/2), 2.0f/2.0f));
		assert(Float::equal(ellipsoid.distance(c-(ellipsoid.center-c)/2), 3.0f/2.0f));

		// todo: test ellipsoid normal
	}
}
