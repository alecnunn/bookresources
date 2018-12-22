// File: ZFXRenderDevice.h
#ifndef ZFXINPUTINTERFACE_H
#define ZFXINPUTINTERFACE_H


// I N C L U D E S /////////////////////////////////////////////////

#include <windows.h>
#include <stdio.h>
#include "ZFX.h"


// D E F I N E S ///////////////////////////////////////////////////



// S T R U C T S ///////////////////////////////////////////////////

/**
 * 
 */
class ZFXInputDevice {
   protected:
      HWND       m_hWndMain;          // application main window
      HINSTANCE  m_hDLL;              // dll module handle
      bool       m_bRunning;          // after succesful initialization
      FILE      *m_pLog;              // log file
      
      
   public:
      ZFXInputDevice(void) {};
      virtual ~ZFXInputDevice(void) {};
      
      
      // INIT/RELEASE STUFF:
      // ===================
      
      virtual HRESULT Init(HWND, RECT, bool)=0;
      
      // release API specific stuff
      virtual void Release(void)=0;
      
      // is initialized?
      virtual bool IsRunning(void)=0;

      // joystick available?
      virtual bool HasJoystick(char *pJoyName)=0;

      // poll input devices
      virtual HRESULT Update(void)=0;

      // will work for mouse and joystick only
      virtual HRESULT GetPosition(ZFXINPUTDEV, POINT*)=0;

      // for joysticks only (intensity range from -1 to +1)
      virtual HRESULT GetJoyDeflection(float*, float*)=0;

      // will work for keyboard, mouse and joystick
      virtual bool IsPressed(ZFXINPUTDEV, UINT)=0;

      // will work for keyboard, mouse and joystick
      virtual bool IsReleased(ZFXINPUTDEV, UINT)=0;

   }; // class

typedef class ZFXInputDevice *LPZFXINPUTDEVICE;
/*----------------------------------------------------------------*/


extern "C" {
   HRESULT CreateInputDevice(HINSTANCE hDLL, ZFXInputDevice **pInterface);
   typedef HRESULT (*CREATEINPUTDEVICE)(HINSTANCE hDLL, ZFXInputDevice **pInterface);
   
   HRESULT ReleaseInputDevice(ZFXInputDevice **pInterface);
   typedef HRESULT (*RELEASEINPUTDEVICE)(ZFXInputDevice **pInterface);
   }
/*----------------------------------------------------------------*/


// keyboard scan codes from dinput.h
#define ZVK_ESCAPE          0x01
#define ZVK_TAB             0x0F
#define ZVK_SPACE           0x39
#define ZVK_RETURN          0x1C
#define ZVK_BACK            0x0E
#define ZVK_CAPITAL         0x3A

#define ZVK_MINUS           0x0C
#define ZVK_EQUALS          0x0D
#define ZVK_LBRACKET        0x1A
#define ZVK_RBRACKET        0x1B
#define ZVK_SEMICOLON       0x27
#define ZVK_APOSTROPHE      0x28
#define ZVK_GRAVE           0x29
#define ZVK_BACKSLASH       0x2B
#define ZVK_COMMA           0x33
#define ZVK_PERIOD          0x34
#define ZVK_SLASH           0x35

// CHARACTER KEYS
#define ZVK_A               0x1E
#define ZVK_S               0x1F
#define ZVK_D               0x20
#define ZVK_F               0x21
#define ZVK_G               0x22
#define ZVK_H               0x23
#define ZVK_J               0x24
#define ZVK_K               0x25
#define ZVK_L               0x26
#define ZVK_Z               0x2C
#define ZVK_X               0x2D
#define ZVK_C               0x2E
#define ZVK_V               0x2F
#define ZVK_B               0x30
#define ZVK_N               0x31
#define ZVK_M               0x32
#define ZVK_Q               0x10
#define ZVK_W               0x11
#define ZVK_E               0x12
#define ZVK_R               0x13
#define ZVK_T               0x14
#define ZVK_Y               0x15
#define ZVK_U               0x16
#define ZVK_I               0x17
#define ZVK_O               0x18
#define ZVK_P               0x19

// MAIN NUM KEYS
#define ZVK_1               0x02
#define ZVK_2               0x03
#define ZVK_3               0x04
#define ZVK_4               0x05
#define ZVK_5               0x06
#define ZVK_6               0x07
#define ZVK_7               0x08
#define ZVK_8               0x09
#define ZVK_9               0x0A
#define ZVK_0               0x0B

// FUNCTION KEYS
#define ZVK_F1              0x3B
#define ZVK_F2              0x3C
#define ZVK_F3              0x3D
#define ZVK_F4              0x3E
#define ZVK_F5              0x3F
#define ZVK_F6              0x40
#define ZVK_F7              0x41
#define ZVK_F8              0x42
#define ZVK_F9              0x43
#define ZVK_F10             0x44
#define ZVK_F11             0x57
#define ZVK_F12             0x58

// ON NUMPAD
#define ZVK_NPPLUS          0x4E
#define ZVK_NPMINUS         0x4A
#define ZVK_NPDECIMAL       0x53
#define ZVK_NPCOMMA         0xB3
#define ZVK_NPDIVIDE        0xB5
#define ZVK_NPMULTIPLY      0x37
#define ZVK_NPENTER         0x9C
#define ZVK_NUMLOCK         0x45
#define ZVK_NP1             0x4F
#define ZVK_NP2             0x50
#define ZVK_NP3             0x51
#define ZVK_NP4             0x4B
#define ZVK_NP5             0x4C
#define ZVK_NP6             0x4D
#define ZVK_NP7             0x47
#define ZVK_NP8             0x48
#define ZVK_NP9             0x49
#define ZVK_NP0             0x52

// CTRL, ALT, SHFT and WINDWS
#define ZVK_RSHIFT          0x36
#define ZVK_LSHIFT          0x2A
#define ZVK_RCTRL           0x9D
#define ZVK_LCTRL           0x1D
#define ZVK_RALT            0xB8
#define ZVK_LALT            0x38
#define ZVK_LWIN            0xDB
#define ZVK_RWIN            0xDC

// ON ARROW KEYPAD
#define ZVK_UP              0xC8
#define ZVK_DOWN            0xD0
#define ZVK_LEFT            0xCB
#define ZVK_RIGHT           0xCD
#define ZVK_INSERT          0xD2
#define ZVK_DELETE          0xD3
#define ZVK_HOME            0xC7
#define ZVK_END             0xCF
#define ZVK_PGDOWN          0xD1
#define ZVK_PGUP            0xC9

#define ZVK_PAUSE           0xC5    /* Pause */
#define ZVK_SCROLL          0x46    /* Scroll Lock */


#endif