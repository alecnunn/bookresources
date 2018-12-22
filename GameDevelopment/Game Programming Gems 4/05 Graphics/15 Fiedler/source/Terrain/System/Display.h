// System Display class

#pragma once

#include "Vertex.h"
#include "Mathematics/Color.h"
#include "Mathematics/Matrix.h"

#include <vector>


namespace System
{
    using namespace Mathematics;

    /// Manages rendering to windowed or fullscreen display.
    /// This object also acts as a factory for Light, Material, Texture, Mesh and Timer objects.

	class Display
	{
	public:

        /// virtual destructor.

		virtual ~Display() {};

		/// open fullscreen display mode or window of specified dimensions.

		virtual void open(const char title[], int width, int height, bool windowed = false) = 0;

        /// close display mode or window.

		virtual void close() = 0;

        /// test if display is valid.
        /// being valid means that the display has been opened and ready for rendering.

		virtual bool valid() const = 0;

        /// begin rendering.
        /// you must call this before calling any rendering methods in display.
        /// you should only call it once per frame.

		virtual void begin() = 0;

        /// end rendering.
        /// call this once you have finished calling display rendering methods for this frame.
        /// only call this method once per frame.

		virtual void end() = 0;

        /// clear the display to the given color, depth value and stencil value.

		virtual void clear(const Color &color = Color(0,0,0,0), float depth = 1.0f, unsigned int stencil = 0) = 0;

        /// update the display.
        /// this makes your graphics rendered this frame visible on the display.

		virtual void update() = 0;

        /// get title of display window.
        
        virtual const char* title() const = 0;

        /// get width of display mode or window.

        virtual int width() const = 0;

        /// get height of display mode or window.

        virtual int height() const = 0;

        /// test if display is in a window or fullscreen.

        virtual bool windowed() const = 0;

        /// set world transform.

		virtual void world(const Matrix &matrix) = 0;

        /// set view transform.

		virtual void view(const Matrix &matrix) = 0;

        /// set projection transform.
        /// note: the projection matrix should conform to d3d specs.

		virtual void projection(const Matrix &matrix) = 0;

        /// get world transform

		virtual Matrix world() const = 0;

        /// get view transform

		virtual Matrix view() const = 0;

        /// get projection transform

		virtual Matrix projection() const = 0;

		/// get the maximum number of lights supported by this display.

		virtual int numberOfLights() const = 0;
		
        /// get the maximum number of texture stages supported by this display.
        
        virtual int numberOfTextureStages() const = 0;

		/// create a texture object loading data from the given filename.

		virtual class Texture* createTexture(const char name[]) = 0;

        /// create a mesh object.
        
        virtual class Mesh* createMesh(const std::vector<Vertex> &vertex, const std::vector<int> &index) = 0;

        /// create a light object.
        /// you can create as many light objects as  you like, however you can only
        /// select up to numberOfLights simultaneous lights with this display.

		virtual class Light* createLight() = 0;

        /// create a material object.

		virtual class Material* createMaterial() = 0;

        /// create timer object.

        virtual class Timer* createTimer() = 0;

		/// select texture object into stage.
        /// valid stages are [0, numberOfTextureStages-1].

		virtual void selectTexture(class Texture *texture, int stage = 0) = 0;

        /// select light object into slot.
        /// valid slots are [0, numberOfLights-1].
        /// note: each call to selectLight MUST be deselected by calling selectMaterial(0)!

		virtual void selectLight(class Light *light, int index = 0) = 0;
        
        /// select material.
        /// note: each call to selectMaterial MUST be deselected by calling selectMaterial(0)!

		virtual void selectMaterial(class Material *material) = 0;

        /// select mesh.
        /// once a mesh is selected, call renderMesh to render it.
        /// you can select a mesh once, then render it multiple times.
        /// this is more efficient than reselecting the mesh each time you want to render it.
        /// note: each call to selectMesh MUST be deselected by calling selectMesh(0)!

		virtual void selectMesh(class Mesh *mesh) = 0;

        /// blend state

        struct Blend
        {
            /// blend mode

            enum Mode
            {
                NONE,           ///< no blending
                MULTIPLY,       ///< color = source * dest
                ADD,            ///< color = source + dest
                ALPHA           ///< color = alpha*source + (1-alpha)dest.  (todo: verify 1-a etc.)
            };
        };

        /// set blend mode.

        virtual void blendMode(Blend::Mode mode = Blend::NONE) = 0;

        /// cull state

        struct Cull
        {
            /// cull mode

            enum Mode
            {
                NONE,               ///< no culling
                CLOCKWISE,          ///< cull clockwise faces
                COUNTERCLOCKWISE,   ///< cull counter clockwise faces
            };
        };

        virtual void cullMode(Cull::Mode mode = Cull::COUNTERCLOCKWISE) = 0;

        /// set depth test

        virtual void depthTest(bool enabled) = 0;

		/// render mesh.
        /// renders the mesh object previously selected with selectMesh.

		virtual void renderMesh() = 0;

        /// begin a quad stream.

        virtual void beginQuadStream() = 0;

        /// render quad. requires open quad stream.

        virtual void renderQuad(const Vector vertex[]) = 0;

        /// end quad stream.

        virtual void endQuadStream() = 0;

        // begin a line stream

        virtual void beginLineStream() = 0;

        /// render line. requires open line stream.

        virtual void renderLine(const Vector &a, const Vector &b) = 0;

        /// end line stream.

        virtual void endLineStream() = 0;

		/// add a keyboard listener.

		virtual void addKeyboardListener(class KeyboardListener &listener) = 0;

        /// add a mouse listener.

		virtual void addMouseListener(class MouseListener &listener) = 0;

        /// add a window listener.

		virtual void addWindowListener(class WindowListener &listener) = 0;

        /// remove keyboard listener.

		virtual void removeKeyboardListener(class KeyboardListener &listener) = 0;

        /// remove mouse listener.

		virtual void removeMouseListener(class MouseListener &listener) = 0;

        /// remove window listener.

		virtual void removeWindowListener(class WindowListener &listener) = 0;

        /// load heightfield data from image.

        virtual bool loadHeightfield(const char filename[], std::vector<float> &data, int &width, int &height) = 0;
    };
}
