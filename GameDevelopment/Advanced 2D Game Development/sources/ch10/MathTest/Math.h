#include "Vector3.h"
//#include "Advanced2D.h"

#pragma once

//namespace Advanced2D {

    const double PI = 3.1415926535;
    const double PI_over_180 = PI / 180.0f;
    const double PI_under_180 = 180.0f / PI;
    
    class Math {
    public:
        double toDegrees(double radian);
        double toRadians(double degree);
        double wrapAngleDegs(double degs);
        double wrapAngleRads(double rads);
        double LinearVelocityX(double angle);
        double LinearVelocityY(double angle);
        Vector3 LinearVelocity(double angle);
        double AngleToTarget(double x1,double y1,double x2,double y2);
        double AngleToTarget(Vector3& source,Vector3& target);
        double Distance( double x1,double y1,double x2,double y2 );
        double Distance( Vector3& v, Vector3& vec2  );
        double Length(Vector3& vec);
        double Length(double x,double y,double z);
        double DotProduct(double x1,double y1,double z1,double x2,double y2,double z2);
        double DotProduct( Vector3& vec1, Vector3& vec2 );
        Vector3 CrossProduct( double x1,double y1,double z1,double x2,double y2,double z2);
        Vector3 CrossProduct( Vector3& vec1, Vector3& vec2 );
        Vector3 Normal(double x,double y,double z);
        Vector3 Normal(Vector3& vec);
    };
    
//};
