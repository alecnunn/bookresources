// Terrain Application

#pragma warning( disable : 4127 )		// conditional expression is constant
#pragma warning( disable : 4100 )		// unreferenced formal parameter
#pragma warning( disable : 4702 )		// unreachable code (for stl ...)

#include "Terrain.h"
#include "Heightfield.h"
#include "Quadtree.h"
#include "QuadtreeNode.h"
#include "QuadtreeLevel.h"

#include "System/System.h"
#include "System/Display.h"
#include "System/KeyboardListener.h"
#include "System/MouseListener.h"
#include "System/WindowListener.h"
#include "System/Key.h"
#include "System/Mouse.h"
#include "System/Error.h"
#include "System/Light.h"
#include "System/Timer.h"

using namespace System;
using namespace Mathematics;


class Application : public KeyboardListener, public MouseListener, public WindowListener
{
public:

    Application()
    {
        // defaults

        x = y = 0;
        error = 4;
        errorDragging = false;
        cameraDragging = false;
        quit = false;

        showNodes = false;
        showHorizon = true;
        showVisualization = true;
        showTerrain = true;

        tileHideCount = 0;

        display = 0;

        // setup camera

        cameraZoomDefault = 60 * 4;
        cameraZoom = cameraZoomDefault;
        cameraZoomTarget = cameraZoom;
        cameraZoomSpeed = 0.002f * 2;
        cameraZoomTightness = 0.1f;
        cameraZoomMinimum = 20 * 4;
        cameraZoomMaximum = 100 * 4;
        cameraZoomAngleMultiplier = Float::pi/2;

        cameraRotateDefault = 0;
        cameraRotate = cameraRotateDefault;
        cameraRotateTarget = cameraRotate;
        cameraRotateSpeed = 0.005f;
        cameraRotateTightness = 0.1f;

        cameraFov = Float::pi / 4;
    }

    void run()
    {
        try
        {
            // open display and register listeners

            System::log("creating display");

            display = System::createDisplay();

            display->open("Terrain", 800, 600, true);
                        
            display->addKeyboardListener(*this);
            display->addMouseListener(*this);
            display->addWindowListener(*this);

            // load heightfield image

            System::log("loading heightfield image");

            int width, height;
            std::vector<float> heights;
            if (!display->loadHeightfield("heightfield.png", heights, width, height))
                throw Error("failed to load heightfield image");

            // create heightfield

            System::log("creating heightfield");

            Heightfield heightfield;
            heightfield.initialize(&heights[0], width, height, Vector(0,0,0), 1, 50);

            // initialize and build quadtree

            quadtree.initialize(display, heightfield, 1);

            buildQuadtree();

            // initialize horizon

            horizon.initialize(display->width(), display->height());

            // setup perspective

            Matrix projectionMatrix;
            projectionMatrix.perspective(cameraFov, display->width()/float(display->height()), 1, 1000);
            display->projection(projectionMatrix);

            // setup lights

            System::Light *light = display->createLight();
            light->position(Vector(0,0,1000));
            light->direction(Vector(-2,-2,-1));
            light->diffuse(Color(1,1,1));
            light->specular(Color(1,1,1));

            // create terrain

            System::log("creating terrain...");

            terrain.initialize(display, heightfield, 16);

            // main loop

            System::log("main loop...");

            quit = false;

            while (!quit)
            {
                Float::doublePrecision();

                updateCamera();

                display->begin();

                display->clear();

                // calculate frustum

                const int width = display->width();
                const int height = display->height();

                Frustum frustum;
                frustum.width = (float) width;
                frustum.height = (float) height;
                frustum.matrix = display->projection() * display->view() * display->world();
                frustum.eye = (display->view() * display->world()).inverse() * Vector(0,0,0);
                frustum.base = frustum.matrix.inverse() * Vector(0, -1500, 0);
                frustum.direction = Vector(0,0,0) - frustum.eye;
                frustum.direction.normalize();
                frustum.k = width / (2 * Float::tan(cameraFov/2));

                // render terrain with horizon culling

                display->selectLight(light, 0);

                std::vector<int> tiles;
                quadtree.visibleTerrainTiles(horizon, frustum, tiles, terrain.getTileSize(), terrain.getResolution(), pow(error,2));
                
                if (showTerrain)
                    terrain.renderTiles(tiles, tileHideCount);

                display->selectLight(0, 0);

                if (showHorizon)
                {
                    // render horizon

                    horizon.render(display);
                }

                if (showVisualization)
                {
                    // top-down visualization of rendered tiles

                    const float size = (float) width / 8.0f;
                    const float edge = size * 0.1f;

                    terrain.visualizeTiles(tiles, tileHideCount, (float)width - size - edge, edge, (float)width - edge, size + edge);
                }

                if (showNodes)
                {
                    // render quadtree nodes

                    quadtree.render(frustum, pow(error,2));
                }

                display->end();

                display->update();
            }

            terrain.free();

            delete display;
        }
        catch (Error &error)
        {
            error.report();
            exit(1);
        }
    }

    void buildQuadtree(Quadtree::BuildType type=Quadtree::TOPDOWN)
    {
        Float::doublePrecision();

        if (type==Quadtree::TOPDOWN)
            System::log("top-down build...");
        else
            System::log("bottom-up build...");

        System::Timer *timer = display->createTimer();

        quadtree.rebuild(type);
        
        System::logf("%f seconds", timer->time());

        timer->release();
    }

    virtual void onKeyDown(const Key &key)
    {
        switch (key.code())
        {
            case Key::ESCAPE:
                quit = true;
                break;

            case Key::SPACE:
                buildQuadtree(Quadtree::TOPDOWN);
                break;

            case Key::ENTER:
                buildQuadtree(Quadtree::BOTTOMUP);
                break;

            case Key::F1:
                showNodes = !showNodes;
                break;

            case Key::F2:
                showHorizon = !showHorizon;
                break;

            case Key::F3:
                showVisualization = !showVisualization;
                break;

            case Key::F4:
                showTerrain = !showTerrain;
                break;

            case Key::UP:
                tileHideCount--;
                if (tileHideCount<0)
                    tileHideCount = 0;
                break;

            case Key::DOWN:
            {
                tileHideCount++;
                const int size = terrain.getResolution() * terrain.getResolution();
                if (tileHideCount>size)
                    tileHideCount = size;
            }
            break;
        }
    }

    virtual void onKeyUp(const Key &key)
    {
    }

    virtual void onMouseDown(const Mouse &mouse, int x, int y)
    {
        this->x = x;
        this->y = y;

        if (mouse.button()==Mouse::LEFT)
            cameraDragging = true;
        else
            errorDragging = true;
            
    }

	virtual void onMouseMove(const Mouse &mouse, int x, int y)
    {
        const int dx = x - this->x;
        const int dy = y - this->y;

        if (errorDragging)
        {
            // error slider left/right drag across screen

            error += dx * 0.005f;

            if (error<0)
                error = 0;
            else if (error>100)
                error = 10;
        }
        else if (cameraDragging)
        {
            // rotate zoom camera

            if (dy<0) cameraZoomTarget += dy * ((cameraZoomTarget)*cameraZoomSpeed);
			else cameraZoomTarget += dy * (cameraZoomSpeed) * cameraZoomMaximum * 1.5f;

			cameraRotateTarget -= dx * cameraRotateSpeed;
        }

        this->x = x;
        this->y = y;
    }

	virtual void onMouseUp(const Mouse &mouse, int x, int y)
    {
        errorDragging = false;
        cameraDragging = false;
    }

	virtual void onActivate(bool active)
    {
    }

    virtual bool onClose()
    {
        return true;
    }

protected:

    void updateCamera()
    {
        // clamp zoom in bounds and interpolate zoom and rotation towards targets

		if (cameraZoomTarget<cameraZoomMinimum) cameraZoomTarget = cameraZoomMinimum;
		if (cameraZoomTarget>cameraZoomMaximum) cameraZoomTarget = cameraZoomMaximum;

		float zoomDelta = cameraZoomTarget - cameraZoom;
		cameraZoom += zoomDelta * cameraZoomTightness;

		float rotateDelta = cameraRotateTarget - cameraRotate;
		cameraRotate += rotateDelta * cameraRotateTightness;

        // build view matrix for camera

		float normalizedZoom = cameraZoom;
		normalizedZoom -= cameraZoomMinimum;
		normalizedZoom /= cameraZoomMaximum - cameraZoomMinimum;
		
        const float zoomAngle = (1.0f-normalizedZoom) * cameraZoomAngleMultiplier;

        Vector cameraTarget(0,0,0);
        Vector cameraUp(0,1,0);

        Matrix view;
        view.lookat(Vector(0,0,cameraZoom), cameraTarget, cameraUp);
               
        Matrix rotationX;
        rotationX.rotate(-zoomAngle, Vector(1,0,0));
        view *= rotationX;

        Matrix rotationZ;
        rotationZ.rotate(cameraRotate, Vector(0,0,1));
        view *= rotationZ;

        display->view(view);
    }

private:

    int x,y;
    float error;
    bool errorDragging;
    bool cameraDragging;
    bool quit;

    bool showNodes;
    bool showHorizon;
    bool showVisualization;
    bool showTerrain;

    int tileHideCount;

    Display *display;
    Terrain terrain;
    Quadtree quadtree;
    Horizon horizon;

    float cameraZoom;
    float cameraZoomDefault;
    float cameraZoomTarget;
    float cameraZoomSpeed;
    float cameraZoomTightness;
    float cameraZoomMinimum;
    float cameraZoomMaximum;
    float cameraZoomAngleMultiplier;

    float cameraRotate;
    float cameraRotateDefault;
    float cameraRotateTarget;
    float cameraRotateSpeed;
    float cameraRotateTightness;

    float cameraFov;
};


void main()
{
    Application application;
    application.run();
}
