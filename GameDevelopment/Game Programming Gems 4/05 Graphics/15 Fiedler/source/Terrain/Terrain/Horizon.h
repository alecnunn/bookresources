// Horizon class

#pragma once

#include "System/Display.h"
#include "System/Texture.h"
#include "System/Material.h"
#include "Mathematics/Float.h"


/// Horizon class.
/// Implementation of the horizon buffer as described in the article.

class Horizon
{
public:

    /// default constructor

    Horizon()
    {
        width = 0;
        height = 0;
    }

    /// initialize horizon for a screen width x height resolution.

    void initialize(int width, int height)
    {
        this->width = width;
        this->height = height;

        buffer.resize(width);

        clear();
    }

    /// clear the horizon to a height of zero.

    void clear()
    {
        memset(&buffer[0], 0, buffer.size()*sizeof(float));
    }

    /// enumeration for horizon testing

    enum TestResult
    {
        ABOVE,      ///< above the horizon at some point
        BELOW       ///< below the horizon at all points
    };

    /// test if a line is above or below the horizon

    TestResult test( float x1, float y1,
                     float x2, float y2 )
    {
        if (x1>x2)
        {
            float t = x1;
            x1 = x2;
            x2 = t;

            t = y1;
            y1 = y2;
            y2 = t;
        }

        if (x2<0 || x1>width-1)
            return BELOW;

        float dx = x2 - x1;
        float dy = y2 - y1;

        const float dydx = dy / dx;

        if (x1<0)
        {
            // clip left

            y1 += -x1 * dydx;
            x1 = 0;
        }

        const float right = (float) width - 1;

        if (x2>right)
        {
            // clip right

            y2 -= (right-x2) * dy;
            x2 = right;
        }

        const int ix1 = Float::integerCeiling(x1-0.5f);
        const int ix2 = Float::integerFloor(x2-0.5f);

        float y = y1;

        for (int x=ix1; x<=ix2; x++)
        {
            assert(x>=0);
            assert(x<(int)buffer.size());

            if (y>buffer[x])
                return ABOVE;

            y += dydx;
        }

        return BELOW;
    }

    /// insert a line into the horizon
                 
    void insert( float x1, float y1,
                 float x2, float y2 )
    {
        if (x1>x2)
        {
            float t = x1;
            x1 = x2;
            x2 = t;

            t = y1;
            y1 = y2;
            y2 = t;
        }

        if (x2<0 || x1>=width-1)
            return;

        float dx = x2 - x1;
        float dy = y2 - y1;

        const float dydx = dy / dx;

        if (x1<0)
        {
            // clip left

            y1 += -x1 * dydx;
            x1 = 0;
        }

        const float right = (float) width - 1;

        if (x2>right)
        {
            // clip right

            y2 -= (right-x2) * dy;
            x2 = right;
        }

        const int ix1 = Float::integerCeiling(x1-0.5f);
        const int ix2 = Float::integerFloor(x2-0.5f);

        float y = y1;

        for (int x=ix1; x<=ix2; x++)
        {
            assert(x>=0);
            assert(x<(int)buffer.size());

            if (buffer[x]<y)
                buffer[x] = y;

            y += dydx;
        }
    }

    void render(System::Display *display)
    {
        // save display matrices

        Matrix view = display->view();
        Matrix world = display->world();
        Matrix projection = display->projection();

        // setup orthographic projection

        Matrix matrix;
        matrix.orthographic((float) display->width(), (float) display->height(), 0, 1);

        Matrix identity;
        identity.identity();

        display->view(identity);
        display->world(identity);
        display->projection(matrix);

        // render horizon line

        System::Material *material = display->createMaterial();
        material->ambient(Color(1,1,1));
        material->diffuse(Color(1,1,1));
        material->specular(Color(0,0,0));
        material->power(0);

        display->selectMaterial(material);

        display->beginLineStream();

        for (unsigned int x=1; x<buffer.size(); x++)
        {
            const int x1 = x - 1;
            const int x2 = x;

            const float y1 = buffer[x1];
            const float y2 = buffer[x2];

            Vector a((float)x1, y1, 0);
            Vector b((float)x2, y2, 0);

            display->renderLine(a,b);
        }

        display->endLineStream();

        display->selectMaterial(0);

        material->release();

        // restore display matrices

        display->view(view);
        display->world(world);
        display->projection(projection);
    }

private:

    int width;
    int height;
    std::vector<float> buffer;
};