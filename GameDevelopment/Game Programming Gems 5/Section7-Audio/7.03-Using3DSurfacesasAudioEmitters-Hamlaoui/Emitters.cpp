#include "Main.h"
#include "Emitters.h"

// Will draw a single emitter and its current audio position.

void Emitter_Render (sEmitter &Emitter)
{
    switch (Emitter.Type)
    {
        case EMITTER_NONE:
            return;

        case EMITTER_POINT:
            glColor3f (1, 0, 0);

            glPushMatrix ();
                glTranslatef (Emitter.Point.x, Emitter.Point.y, Emitter.Point.z);
                glutWireSphere (0.1, 30, 30);
            glPopMatrix ();

            break;

        case EMITTER_SPHERE:
            glColor3f (0, 1, 0);

            glPushMatrix ();
                glTranslatef (Emitter.Sphere.Origin.x, Emitter.Sphere.Origin.y, Emitter.Sphere.Origin.z);
                glutWireSphere (Emitter.Sphere.Radius, 30, 30);
            glPopMatrix ();

            break;

        case EMITTER_BOX:
            glColor3f (0, 0, 1);

            glPushMatrix ();
                glTranslatef (Emitter.Box.Origin.x, Emitter.Box.Origin.y, Emitter.Box.Origin.z);

                // TODO: Proper cube drawing code.
                glutWireCube (4);

                glEnd ();
            glPopMatrix ();

            break;

        case EMITTER_LINE_SEGMENT:

            glPushMatrix ();
                glTranslatef (Emitter.LineSegment.Origin.x, Emitter.LineSegment.Origin.y, Emitter.LineSegment.Origin.z);

                glBegin (GL_LINES);

                    glColor3f (1, 1, 0);
                    glVertex3f (0.0f, 0.0f, 0.0f);
                    glColor3f (1, 0, 1);
                    glVertex3f (Emitter.LineSegment.Direction.x * Emitter.LineSegment.Length,
                                Emitter.LineSegment.Direction.y * Emitter.LineSegment.Length,
                                Emitter.LineSegment.Direction.z * Emitter.LineSegment.Length);
                glEnd ();

            glPopMatrix ();

            break;


        // Drawn 20000 units long. Not quite infinite, but it'll do.
        case EMITTER_LINE_INFINITE:

            glPushMatrix ();
                glTranslatef (Emitter.LineInfinite.Point.x, Emitter.LineInfinite.Point.y, Emitter.LineInfinite.Point.z);

                glBegin (GL_LINES);

                    glColor3f (1, 1, 0);
                    glVertex3f (Emitter.LineInfinite.Direction.x * -10000.0,
                                Emitter.LineInfinite.Direction.y * -10000.0,
                                Emitter.LineInfinite.Direction.z * -10000.0);

                    glColor3f (1, 0, 1);
                    glVertex3f (Emitter.LineInfinite.Direction.x * 10000.0,
                                Emitter.LineInfinite.Direction.y * 10000.0,
                                Emitter.LineInfinite.Direction.z * 10000.0);
                glEnd ();

            glPopMatrix ();

            break;
    }

    glColor3f (1, 1, 1);

    // Draw the audio position.
    glPushMatrix ();
        glTranslatef (Emitter.AudioOrigin.x, Emitter.AudioOrigin.y, Emitter.AudioOrigin.z);
        glutWireSphere (0.1, 30, 30);
    glPopMatrix ();
}

// Updates a single emitter (calculates a new audio position).

void Emitter_Update (sEmitter &Emitter, sVector &Listener)
{
    // Make sure the emitter has something to update.
    if (!Emitter.AudioChannel)
        return;

    switch (Emitter.Type)
    {
        case EMITTER_NONE: return;

        case EMITTER_POINT:         Emitter.Point.GetClosest        (Listener, Emitter.AudioOrigin); break;
        case EMITTER_SPHERE:        Emitter.Sphere.GetClosest       (Listener, Emitter.AudioOrigin); break;
        case EMITTER_BOX:           Emitter.Box.GetClosest          (Listener, Emitter.AudioOrigin); break;
        case EMITTER_LINE_SEGMENT:  Emitter.LineSegment.GetClosest  (Listener, Emitter.AudioOrigin); break;
        case EMITTER_LINE_INFINITE: Emitter.LineInfinite.GetClosest (Listener, Emitter.AudioOrigin); break;
    }

    // Pass the newly calculated audio position to FMOD.
    FSOUND_3D_SetAttributes (Emitter.AudioChannel, &Emitter.AudioOrigin.x, NULL);
}

// Creates a point emitter.

void Emitter_SetPoint (sEmitter &Emitter, float Originx, float Originy, float Originz)
{
    memset (&Emitter, 0, sizeof (sEmitter));

    Emitter.Type = EMITTER_POINT;
    Emitter.Point.Set (Originx, Originy, Originz);
}

// Creates a sphere emitter.

void Emitter_SetSphere (sEmitter &Emitter, float Originx, float Originy, float Originz, float Radius)
{
    memset (&Emitter, 0, sizeof (sEmitter));

    Emitter.Type = EMITTER_SPHERE;
    Emitter.Sphere.Origin.Set (Originx, Originy, Originz);
    Emitter.Sphere.Radius = Radius;
}

// Creates a box emitter.

void Emitter_SetBox (sEmitter &Emitter, float Originx, float Originy, float Originz, float Minx, float Miny, float Minz, float Maxx, float Maxy, float Maxz)
{
    memset (&Emitter, 0, sizeof (sEmitter));

    Emitter.Type = EMITTER_BOX;
    Emitter.Box.Origin.Set (Originx, Originy, Originz);
    Emitter.Box.Min.Set    (Minx,    Miny,    Minz);
    Emitter.Box.Max.Set    (Maxx,    Maxy,    Maxz);
}

// Creates a line segment emitter.

void Emitter_SetLineSegment (sEmitter &Emitter, float Originx, float Originy, float Originz, float Dirx, float Diry, float Dirz, float Length)
{
    memset (&Emitter, 0, sizeof (sEmitter));

    Emitter.Type = EMITTER_LINE_SEGMENT;
    Emitter.LineSegment.Origin.Set    (Originx, Originy, Originz);
    Emitter.LineSegment.Direction.Set (Dirx,    Diry,    Dirz);
    Emitter.LineSegment.Length = Length;

    Emitter.LineSegment.Direction.Normalize ();
}

// Creates an infinite line emitter.

void Emitter_SetLineInfinite (sEmitter &Emitter, float Pointx, float Pointy, float Pointz, float Dirx, float Diry, float Dirz)
{
    memset (&Emitter, 0, sizeof (sEmitter));

    Emitter.Type = EMITTER_LINE_INFINITE;
    Emitter.LineInfinite.Point.Set     (Pointx, Pointy, Pointz);
    Emitter.LineInfinite.Direction.Set (Dirx,    Diry,    Dirz);

    Emitter.LineInfinite.Direction.Normalize ();
}