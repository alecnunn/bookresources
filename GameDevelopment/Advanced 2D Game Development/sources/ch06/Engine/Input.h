#include "Advanced2d.h"

#pragma once

namespace Advanced2D {

    class Input {

    private:
        HWND window; 
        IDirectInput8 *di; 
        IDirectInputDevice8 *keyboard;
        char keyState[256]; 
        IDirectInputDevice8 *mouse; 
        DIMOUSESTATE mouseState; 
        POINT position; 

    public:
        Input( HWND window );
        virtual ~Input();
        void Update();
        bool GetMouseButton( char button );

        char GetKeyState(int key) { return keyState[key]; }
        long GetPosX() { return position.x; }
        long GetPosY() { return position.y; }
        long GetDeltaX() { return mouseState.lX; }
        long GetDeltaY() { return mouseState.lY; }
        long GetDeltaWheel() { return mouseState.lZ; }

    };

};

