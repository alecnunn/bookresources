
class RNS
{
public:
  void Init(){ nodeCount = 0; }
  void AddNode(const Vector3 &pos);
  void BuildSpline();
  Vector3 GetPosition(float time);

  struct splineData
  {
    Vector3 position;
    Vector3 velocity;
    float distance;
  } node[100];
  float maxDistance;
  int nodeCount;
protected:
  Vector3 GetStartVelocity(int index);
  Vector3 GetEndVelocity(int index);
};

class SNS : public RNS
{
public:
  void BuildSpline(){ RNS::BuildSpline(); Smooth(); Smooth(); Smooth(); }
  void Smooth();
};

class TNS : public SNS
{
public:
  void AddNode(const Vector3 &pos, float timePeriod);
  void BuildSpline(){ RNS::BuildSpline(); Smooth(); Smooth(); Smooth(); }
  void Smooth(){ SNS::Smooth(); Constrain(); }
  void Constrain(); 
};