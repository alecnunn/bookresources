/*
*/

struct SVector {
    float fX;
    float fY;
    float fZ;
};

float GetObstructionLevel(SVector &svListener, 
                          SVector &svSource, 
                          SVector &svMin, 
                          SVector &svMax);
