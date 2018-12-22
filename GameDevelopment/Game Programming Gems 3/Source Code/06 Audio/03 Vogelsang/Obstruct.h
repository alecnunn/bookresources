/*
*/

struct SVector {							   //Standard 3D Vector
    float fX;
    float fY;
    float fZ;
};

float GetObstructionLevel(SVector &svListener, //Listener position
                          SVector &svSource,   //Source position
                          SVector &svMin,      //Min. coord. of bounding box
                          SVector &svMax);	   //Max. coord. of bounding box
