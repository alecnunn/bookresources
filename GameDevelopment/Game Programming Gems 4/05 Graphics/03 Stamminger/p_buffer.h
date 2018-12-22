//
// Perspective Shadow Maps
// accompanying example code for Game Programming Gems 4
// (w)(c) 2003, Carsten Dachsbacher (dachsbacher@cs.fau.de), Marc Stamminger (stamminger@cs.fau.de)
//
#ifndef _P_BUFFER__H
#define _P_BUFFER__H

class CPBuffer
{
    private:    
        HPBUFFERARB hPBuffer;
        HDC         hDC;
        HGLRC       hRC;

        int         sizeX, sizeY;

        GLuint      textureID;

        int         exists;
        GLenum      bound;

    public:
        CPBuffer( int _x, int _y, HDC hDC );
        ~CPBuffer();

        int     bind( GLenum drawBuffer = WGL_FRONT_LEFT_ARB );
        int     release();
        int     makeCurrent();

        GLuint  getTexID() { return textureID; };
        HGLRC   getHRC()   { return hRC; };

        int     exist()    { return exists; };
};

#endif







