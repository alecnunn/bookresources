#ifndef GPG5_HAMLAOUI_Emitters
#define GPG5_HAMLAOUI_Emitters

// The emitter shapes supported.

enum eEmitterType
{
    EMITTER_NONE = 0,

    EMITTER_POINT,
    EMITTER_SPHERE,
    EMITTER_BOX,
    EMITTER_LINE_SEGMENT,
    EMITTER_LINE_INFINITE
};

// Defines a single emitter.

struct sEmitter
{
    // eEmitterType
    int Type;

    // The position of the audio.
    sVector AudioOrigin;

    // The channel of the audio.
    int AudioChannel;

    // Storage for the shape components.
    union
    {
        sVector       Point;
        sSphere       Sphere;
        sBox          Box;
        sLineSegment  LineSegment;
        sLineInfinite LineInfinite;
    };
};

void Emitter_Render (sEmitter &Emitter);
void Emitter_Update (sEmitter &Emitter, sVector &Listener);

void Emitter_SetPoint        (sEmitter &Emitter, float Originx, float Originy, float Originz);
void Emitter_SetSphere       (sEmitter &Emitter, float Originx, float Originy, float Originz, float Radius);
void Emitter_SetBox          (sEmitter &Emitter, float Originx, float Originy, float Originz, float Minx, float Miny, float Minz, float Maxx, float Maxy, float Maxz);
void Emitter_SetLineSegment  (sEmitter &Emitter, float Originx, float Originy, float Originz, float Dirx, float Diry, float Dirz, float Length);
void Emitter_SetLineInfinite (sEmitter &Emitter, float Pointx,  float Pointy,  float Pointz,  float Dirx, float Diry, float Dirz);

#endif  // GPG5_HAMLAOUI_Emitters