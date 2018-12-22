/*********************************************************************
 * SimHelpers.h
 * Authored by Kris Hauser 2002-2003
 *
 * Simulation.C was getting too big, so I put these functions here.
 * Most of them have to do with getting the right entries in the
 * constraint equation Ax >= b
 *
 * Copyright 2003, Regents of the University of California 
 *
 *********************************************************************/

/**************************************************************************
 * Copyright (c) 2003, University of California at Berkeley
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * 
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 * 
 *     * Redistributions in binary form must reproduce the above
 *       copyright notice, this list of conditions and the following
 *       disclaimer in the documentation and/or other materials provided
 *       with the distribution.
 * 
 *     * Neither the name of the University of California nor the names
 *       of its contributors may be used to endorse or promote products
 *       derived from this software without specific prior written
 *       permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************/




///GETTING CONSTRAINT RESPONSE

//ACCEL RESPONSE

//response of p wrt q
float_t NormalNormalAResponse(const CConstraint* p, const CConstraint* q) {
  float_t resp = 0.0;
  //for those on the same sides use positive force dof
  if(q->a == p->a) {
    resp += p->a->GetNormalForceResponse(p->p1, q->p1, p->fn.normal);
  }
  //for those on the opposite sides use negative force dof
  if(q->b == p->a) {
    resp -= p->a->GetNormalForceResponse(p->p1, q->p2, p->fn.normal);
  }
  if(p->b) {
    //for those on the same sides use positive force dof
    if(q->a == p->b) {
      resp -= p->b->GetNormalForceResponse(p->p2, q->p1, p->fn.normal);
    }
    //for those on the opposite sides use negative force dof
    if(q->b == p->b) {
      resp += p->b->GetNormalForceResponse(p->p2, q->p2, p->fn.normal);
    }
  }
  return resp;
}

Vector3 NormalFreeAResponse(const CConstraint* p, const CConstraint* q) {
  Vector3 r;
  Matrix3 m;
  if(p->a == q->a) {
    p->a->GetForceResponse(p->p1, q->p1, m);
    r = p->fn.normal*m;
  }
  if(p->a == q->b) {
    p->a->GetForceResponse(p->p1, q->p2, m);
    r -= p->fn.normal*m;
  }
  if(p->b) {
    if(p->b == q->a) {
      p->b->GetForceResponse(p->p2, q->p1, m);
      r -= p->fn.normal*m;
    }
    if(p->b == q->b) {
      p->b->GetForceResponse(p->p2, q->p2, m);
      r += p->fn.normal*m;
    }
  }
  return r;
}


Vector3 FreeNormalAResponse(const CConstraint* p, const CConstraint* q) {
  Vector3 r;
  Matrix3 m;
  if(p->a == q->a) {
    p->a->GetForceResponse(p->p1, q->p1, m);
    r = m*p->fn.normal;
  }
  if(p->a == q->b) {
    p->a->GetForceResponse(p->p1, q->p2, m);
    r -= m*p->fn.normal;
  }
  if(p->b) {
    if(p->b == q->a) {
      p->b->GetForceResponse(p->p2, q->p1, m);
      r -= m*p->fn.normal;
    }
    if(p->b == q->b) {
      p->b->GetForceResponse(p->p2, q->p2, m);
      r += m*p->fn.normal;
    }
  }
  return r;
}

void FreeFreeAResponse(const CConstraint* c, const CConstraint* d, Matrix3& out) {
  Matrix3 m;
  out = 0.0;
  if(c->a == d->a) {
    c->a->GetForceResponse(c->p1, d->p1, m);
    out += m;
  }
  if(c->a == d->b) {
    c->a->GetForceResponse(c->p1, d->p2, m);
    out -= m;
  }
  if(c->b) {
    if(c->b == d->a) {
      c->b->GetForceResponse(c->p2, d->p1, m);
      out -= m;
    }
    if(c->b == d->b) {
      c->b->GetForceResponse(c->p2, d->p2, m);
      out += m;
    }
  }
}

void ConstraintAResponse(const CConstraint* c, const CConstraint* d, Matrix3& out) {
  if(c->IsNormal()) {
    if(d->IsNormal()) {
      out(0,0) = NormalNormalAResponse(c,d);
    }
    else {
      Vector3 r = NormalFreeAResponse(c,d);
      out(0,0) = r[0];
      out(0,1) = r[1];
      out(0,2) = r[2];
    }
  }
  else {
    if(d->IsNormal()) {
      Vector3 r = FreeNormalAResponse(c,d);
      out(0,0) = r[0];
      out(1,0) = r[1];
      out(2,0) = r[2];
    }
    else {
      FreeFreeAResponse(c,d, out);
    }
  }
}


//VELOCITY RESPONSE

//response of p wrt q
float_t NormalNormalVResponse(const CConstraint* p, const CConstraint* q) {
  float_t resp = 0.0;
  //for those on the same sides use positive force dof
  if(q->a == p->a) {
    resp += p->a->GetNormalImpulseResponse(p->p1, q->p1, p->fn.normal);
  }
  //for those on the opposite sides use negative force dof
  if(q->b == p->a) {
    resp -= p->a->GetNormalImpulseResponse(p->p1, q->p2, p->fn.normal);
  }
  if(p->b) {
    //for those on the same sides use positive force dof
    if(q->a == p->b) {
      resp -= p->b->GetNormalImpulseResponse(p->p2, q->p1, p->fn.normal);
    }
    //for those on the opposite sides use negative force dof
    if(q->b == p->b) {
      resp += p->b->GetNormalImpulseResponse(p->p2, q->p2, p->fn.normal);
    }
  }
  return resp;
}

Vector3 NormalFreeVResponse(const CConstraint* p, const CConstraint* q) {
  Vector3 r;
  Matrix3 m;
  if(p->a == q->a) {
    p->a->GetImpulseResponse(p->p1, q->p1, m);
    r = p->fn.normal*m;
  }
  if(p->a == q->b) {
    p->a->GetImpulseResponse(p->p1, q->p2, m);
    r -= p->fn.normal*m;
  }
  if(p->b) {
    if(p->b == q->a) {
      p->b->GetImpulseResponse(p->p2, q->p1, m);
      r -= p->fn.normal*m;
    }
    if(p->b == q->b) {
      p->b->GetImpulseResponse(p->p2, q->p2, m);
      r += p->fn.normal*m;
    }
  }
  return r;
}


Vector3 FreeNormalVResponse(const CConstraint* p, const CConstraint* q) {
  Vector3 r;
  Matrix3 m;
  if(p->a == q->a) {
    p->a->GetImpulseResponse(p->p1, q->p1, m);
    r = m*p->fn.normal;
  }
  if(p->a == q->b) {
    p->a->GetImpulseResponse(p->p1, q->p2, m);
    r -= m*p->fn.normal;
  }
  if(p->b) {
    if(p->b == q->a) {
      p->b->GetImpulseResponse(p->p2, q->p1, m);
      r -= m*p->fn.normal;
    }
    if(p->b == q->b) {
      p->b->GetImpulseResponse(p->p2, q->p2, m);
      r += m*p->fn.normal;
    }
  }
  return r;
}

void FreeFreeVResponse(const CConstraint* c, const CConstraint* d, Matrix3& out) {
  Matrix3 m;
  out = 0.0;
  if(c->a == d->a) {
    c->a->GetImpulseResponse(c->p1, d->p1, m);
    out += m;
  }
  if(c->a == d->b) {
    c->a->GetImpulseResponse(c->p1, d->p2, m);
    out -= m;
  }
  if(c->b) {
    if(c->b == d->a) {
      c->b->GetImpulseResponse(c->p2, d->p1, m);
      out -= m;
    }
    if(c->b == d->b) {
      c->b->GetImpulseResponse(c->p2, d->p2, m);
      out += m;
    }
  }
}

void ConstraintVResponse(const CConstraint* c, const CConstraint* d, Matrix3& out) {
  if(c->IsNormal()) {
    if(d->IsNormal()) {
      out(0,0) = NormalNormalVResponse(c,d);
    }
    else {
      Vector3 r = NormalFreeVResponse(c,d);
      out(0,0) = r[0];
      out(0,1) = r[1];
      out(0,2) = r[2];
    }
  }
  else {
    if(d->IsNormal()) {
      Vector3 r = FreeNormalVResponse(c,d);
      out(0,0) = r[0];
      out(1,0) = r[1];
      out(2,0) = r[2];
    }
    else {
      FreeFreeVResponse(c,d, out);
    }
  }
}


//POSITION RESPONSE

void ConstraintPResponse(const CConstraint* c, const CConstraint* d, Matrix3& out) {
  //TODO: check: is there any difference?
  ConstraintVResponse(c,d,out);
}



//GETTING CONSTRAINT ERROR

void ConstraintVError(const CConstraint* p, Vector3& out) {
  Vector3 vel;
  p->a->GetBodyPointVelocity_World(p->p1, vel);
  out = vel;
  if(p->b) {
    p->b->GetBodyPointVelocity_World(p->p2, vel);
    out -= vel;
  }

  if(p->IsNormal()) {
    out[0] = dot(p->fn.normal, out);
  }
}

void ConstraintAError(const CConstraint* p, Vector3& out) {
  Vector3 vel;
  p->a->GetBodyPointAccel_World(p->p1, vel);
  out = vel;
  if(p->b) {
    p->b->GetBodyPointAccel_World(p->p2, vel);
    out -= vel;
  }

  if(p->IsNormal()) {
    out[0] = dot(p->fn.normal, out);
  }
}

void ConstraintPError(const CConstraint* p, Vector3& out) {
  Vector3 pos;
  p->a->GetBodyPointPosition_World(p->p1, pos);
  out = pos;
  if(p->b) {
    p->b->GetBodyPointPosition_World(p->p2, pos);
    out = out - pos - p->p;
  }
  else {
    out -= p->p;
  }

  if(p->IsNormal()) {
    out[0] = dot(p->fn.normal, out);
  }
}

inline float_t PenetrationVError(const CConstraint* p) {
  assert(p->IsPenetration() && p->IsNormal());
  Vector3 v;
  ConstraintVError(p,v); 
  return v[0];
}

inline float_t PenetrationAError(const CConstraint* p) {
  assert(p->IsPenetration() && p->IsNormal());
  Vector3 v;
  ConstraintAError(p,v); 
  return v[0];
}

inline float_t PenetrationPError(const CConstraint* p) {
  assert(p->IsPenetration() && p->IsNormal());
  Vector3 v;
  ConstraintPError(p,v); 
  return v[0];
}



/***************************
FrictionForce()
Gives the friction force given the tangential velocity, load, and 
a hacky scale factor for the maximum force to apply (for static friction)
***************************/
Vector3 FrictionForce(const CConstraint* p, const Vector3& vtangential, float_t load, float_t max_scale = 1.0) {
  Vector3 vt = vtangential;
  Real vt_mag = vt.length();

  if(vt_mag > EPSILON) {
    //find delta v, restrict it so vt+delta v > 0
    //approx: f cannot exceed A^-1*v or fn*kFriction
    
    vt *= Inv(vt_mag);
    
    //calculate a maximum friction to apply (so you don't force a point backwards)
    float_t resp = p->a->GetNormalForceResponse(p->p1, p->p1, vt);
    if(p->b)
      resp += p->b->GetNormalForceResponse(p->p2, p->p2, vt);
    float_t maximum;
    if(resp > EPSILON)
      maximum = vt_mag/resp*max_scale;
    else
      maximum = FLT_MAX;

    float_t f = load*kFriction;
    //clip f to maximum
    if(maximum < f)
      f = maximum;

    //no negative friction
    if(f > 0.0)
      return -f*vt;
  }
  return Vector3(Zero);
}

/***************************
FrictionImpulse()
Gives the friction impulse given the tangential velocity, load, and 
a hacky scale factor for the maximum impulse to apply (for static friction)
***************************/
Vector3 FrictionImpulse(const CConstraint* p, const Vector3& vtangential, float_t load, float_t max_scale = 1.0) {
  Vector3 vt = vtangential;
  Real vt_mag = vt.length();

  if(vt_mag > EPSILON) {
    //find delta v, restrict it so vt+delta v > 0
    //approx: f cannot exceed A^-1*v or fn*kFriction
    
    vt *= Inv(vt_mag);
    
    //calculate a maximum friction to apply (so you don't force a point backwards)
    float_t resp = p->a->GetNormalImpulseResponse(p->p1, p->p1, vt);
    if(p->b)
      resp += p->b->GetNormalImpulseResponse(p->p2, p->p2, vt);
    float_t maximum;
    if(resp > EPSILON)
      maximum = vt_mag/resp*max_scale;
    else
      maximum = FLT_MAX;

    float_t f = load*kFriction;
    //clip f to maximum
    if(maximum < f)
      f = maximum;

    //no negative friction
    if(f > 0.0)
      return -f*vt;
  }
  return Vector3(Zero);
}


/***************************
ConstraintForce()
Gives the force vector to apply, given the result x from the constraint
solver, plus the hacky friction scale thing.
***************************/
Vector3 ConstraintForce(const CConstraint* p, float_t* x, float scale) {
  if(p->IsPenetration()) {
    Vector3 n;
    Vector3 v,vt;
    float_t vn;

    n = p->fn.normal;

#ifndef NO_FRICTION
    p->a->GetBodyPointVelocity_World(p->p1, v);
    if(p->b) {
      p->b->GetBodyPointVelocity_World(p->p2, vt);
      v = v+vt;
    }
    
    vn = dot(v,n);
    vt = v-vn*n;
    
    return x[0]*n + FrictionForce(p, vt, x[0], scale); 
#else
    return x[0]*n;
#endif
  }
  else if(p->IsNormal()) {
    return x[0]*p->fn.normal;
  }
  else {
    return Vector3(x[0], x[1], x[2]);
  }
}

/***************************
ConstraintImpulse()
Gives the impulse vector to apply, given the result x from the constraint
solver, plus the hacky friction scale thing.
***************************/
Vector3 ConstraintImpulse(const CConstraint* p, float_t* x, float scale) {
  if(p->IsPenetration()) {
    Vector3 n;
    Vector3 v,vt;
    float_t vn;

    n = p->fn.normal;

#ifndef NO_FRICTION
    p->a->GetBodyPointVelocity_World(p->p1, v);
    if(p->b) {
      p->b->GetBodyPointVelocity_World(p->p2, vt);
      v = v+vt;
    }
    
    vn = dot(v,n);
    vt = v-vn*n;
    
    return x[0]*n + FrictionImpulse(p, vt, x[0], scale); 
#else
    return x[0]*n;
#endif
  }
  else if(p->IsNormal()) {
    return x[0]*p->fn.normal;
  }
  else {
    return Vector3(x[0], x[1], x[2]);
  }
}

/***************************
ConstraintShift()
Gives the shift vector to apply, given the result x from the constraint
solver
***************************/
Vector3 ConstraintShift(const CConstraint* p, float_t* x) {
  if(p->IsPenetration()) {
    return x[0]*p->fn.normal;
  }
  else if(p->IsNormal()) {
    return x[0]*p->fn.normal;
  }
  else {
    return Vector3(x[0], x[1], x[2]);
  }
}




//SETTING UP CONSTRAINTS (into a matrix/vector)

inline void SetSubMatrix(matrix_type A, int i, int j, const Matrix3& m, int x, int y) {
  int p,q;
  for(p=0; p<x; p++)
    for(q=0; q<y; q++)
      A[i+p][j+q] = m(p,q);
}

inline void SetAConstraints(matrix_type A, int i, int j, CConstraint* c, CConstraint* d) {
  assert(c->constraint_type & CONSTRAINT_ACCEL);
  Matrix3 m;
  ConstraintAResponse(c,d,m);
  SetSubMatrix(A,i,j,m,c->NumDofs(),d->NumDofs());
}

void SetVConstraints(matrix_type A, int i, int j, CConstraint* c, CConstraint* d) {
  assert(c->constraint_type & CONSTRAINT_VELOCITY);
  Matrix3 m;
  ConstraintVResponse(c,d,m);
  SetSubMatrix(A,i,j,m,c->NumDofs(),d->NumDofs());
}

void SetPConstraints(matrix_type A, int i, int j, CConstraint* c, CConstraint* d) {
  assert(c->constraint_type & CONSTRAINT_POSITION);
  Matrix3 m;
  ConstraintPResponse(c,d,m);
  SetSubMatrix(A,i,j,m,c->NumDofs(),d->NumDofs());
}


void SetAConstraints(vector_type b, int i, CConstraint* c) {
  Vector3 v;
  int p;
  int x;
  x = c->NumDofs();

  assert(c->constraint_type & CONSTRAINT_ACCEL);
  ConstraintAError(c,v);

  for(p=0; p<x; p++)
    b[i+p] = -v[p];
}

void SetVConstraints(vector_type b, int i, CConstraint* c) {
  Vector3 v;
  int p;
  int x;
  x = c->NumDofs();

  assert(c->constraint_type & CONSTRAINT_VELOCITY);
  ConstraintVError(c,v);

  for(p=0; p<x; p++)
    b[i+p] = -v[p];
}

void SetPConstraints(vector_type b, int i, CConstraint* c) {
  Vector3 v;
  int p;
  int x;
  x = c->NumDofs();

  assert(c->constraint_type & CONSTRAINT_POSITION);
  ConstraintPError(c,v);

  for(p=0; p<x; p++)
    b[i+p] = -v[p];
}








void TestConstraints(CBody* b)
{
  CConstraint c,d;
  Vector3 a,f;
  Matrix3 m;

  //REGULAR
  c.constraint_type = CONSTRAINT_ACCEL;
  c.a = b;
  c.b = NULL;
  c.p1.vert[0] = 0;
  c.p1.coords[0] = 1;
  c.p1.n = 1;

  d.constraint_type = CONSTRAINT_ACCEL;
  d.a = b;
  d.b = NULL;
  d.p1.vert[0] = 5;
  d.p1.coords[0] = 1;
  d.p1.n = 1;

  ConstraintAError(&c, a);

  ConstraintAResponse(&c, &c, m);
  cout << "A response: " << endl << m << endl;

  f.set(100,0,0);
  b->ApplyForce_World(c.p1, f);

  if(b->Deformable) {
    b->deformState->Refresh();
  }

  cout << "Old a: " << a << endl;
  cout << "Predicted a, force 100 applied: " << a + m*f << endl;
  ConstraintAError(&c, a);
  cout << "New a, force 100 applied: " << a << endl;



  //NORMAL
  c.constraint_type = CONSTRAINT_ACCEL | CONSTRAINT_NORMAL;
  c.fn.normal.set(0,1,0);

  ConstraintAError(&c, a);
  ConstraintAResponse(&c, &c, m);
  cout << "A response: " << endl << m(0,0) << endl;

  f.set(0,50,0);
  b->ApplyForce_World(c.p1, f);

  if(b->Deformable) {
    b->deformState->Refresh();
  }

  cout << "Old a: " << a[0] << endl;
  cout << "Predicted a, force 50 applied: " << a[0] + m(0,0)*f[1] << endl;
  ConstraintAError(&c, a);
  cout << "New a, force 50 applied: " << a[0] << endl;


  //OPPOSITE
  c.constraint_type = CONSTRAINT_ACCEL;
  ConstraintAError(&c, a);

  ConstraintAResponse(&c, &d, m);
  cout << "A response: " << endl << m << endl;

  f.set(100,0,0);
  b->ApplyForce_World(d.p1,f);

  if(b->Deformable) {
    b->deformState->Refresh();
  }

  cout << "Old a: " << a << endl;
  cout << "Predicted a, force 100 applied: " << a + m*f << endl;
  ConstraintAError(&c, a);
  cout << "New a, force 100 applied: " << a << endl;

}










//assume m entries are in ascending order
void drop_vector_entries(vector_type y, const vector_type x, int n, int entries [], int m) {
  if(m == 0)
    return;
  int k=0;
  int i;
  for(i=0; i<n; i++) {
    if(k < m && i == entries[k]) {
      k++;
    }
    else {
      y[i-k] = x[i];
    }
  }
}

inline void drop_vector_entries(vector_type x, int n, int entries [], int m) {
  drop_vector_entries(x,x,n,entries,m);
}



//x is square, entries ascending
void drop_matrix_entries(matrix_type y, const matrix_type x, int n, int entries [], int m) {
  if(m == 0)
    return;
  int k=0;
  int i;
  for(i=0; i<n; i++) {
    if(k < m && i == entries[k]) {
      k++;
    }
    else {
      drop_vector_entries(y[i-k],x[i], n,entries,m);
    }
  }
}

inline void drop_matrix_entries(matrix_type x,  int n, int entries [], int m) {
  drop_matrix_entries(x,x,n,entries,m);
}

void test_drops(const vector_type vold, const vector_type vnew, int drops[], int m, int n) {
  int o; //old counter
  int q; //new counter
  int k; //drop counter;
  k = 0;
  q = 0;
  for(o=0; o<n; o++) {
    if(k < m && o == drops[k]) {
      k++;
    }
    else {
      if(vnew[q] != vold[o]) {
	printf("ERROR in drop, %d %d!!!\n", q, o);
	printf("Drops: ");
	for(int i=0; i<m; i++) {
	  printf("%d ", drops[i]);
	}
	printf("\n");
	vector_print(vold, n);
	vector_print(vnew, n-m);
	exit(-1);
      }
      q++;
    }
  }
}
void test_drops(const matrix_type mold, const matrix_type mnew, int drops[], int m, int n) {
  int o; //old counter
  int q; //new counter
  int k; //drop counter;
  k = 0;
  q = 0;
  for(o=0; o<n; o++) {
    if(k < m && o == drops[k]) {
      k++;
    }
    else {
      //printf("row %d, %d\n", q, o);
      test_drops(mold[o], mnew[q], drops, m, n);
      q++;
    }
  }
}

template <class type>
type* list_to_array(const mylist<type>& l) {
  if(l.empty())
    return NULL;
  type* array = new type[l.size()];
  int i=0;
  type* x = NULL;
  while((x = l.next(x))) {
    array[i] = *x;
    i++;
  }
  return array;
}



void drop_test() {
  matrix_type m = matrix_create(4,4);
  m[0][0] = 1; m[0][1] = 2; m[0][2] = 3; m[0][3] = 4;
  m[1][0] = 5; m[1][1] = 6; m[1][2] = 7; m[1][3] = 8;
  m[2][0] = 9; m[2][1] = 10; m[2][2] = 11; m[2][3] = 12;
  m[3][0] = 13; m[3][1] = 14; m[3][2] = 15; m[3][3] = 16;
  int drops [] = { 1, 3 };
  drop_matrix_entries(m,4,drops,2);
  matrix_print(m,2,2);

  vector_type v = vector_create(10);
  for(int i=0; i<10; i++)
    v[i] = i;
  int vdrops [] = {2, 3, 4, 9 };
  drop_vector_entries(v,10,vdrops,4);
  vector_print(v, 6);
  exit(0);
}
