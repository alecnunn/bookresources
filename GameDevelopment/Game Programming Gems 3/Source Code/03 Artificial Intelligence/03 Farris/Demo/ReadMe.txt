Demo ReadMe

Description:
Demo implements a console application with an update loop for creating and testing classes
using the CFSM classes.  

Files:
Application				Main Demo code
Bin					Binary directory (All .exe & .dll files are copied here)
FSM					The CFSM classes
Lib					Library directory (All .lib files are copied here)
LightBulb				Light Bulb example classes
Global.h				
Demo.dsw				Demo Workspace
Demo.dsp				Demo Project

Release Notes:7

To run the demo, use the following instructions:

1.  Open Demo.dsw in Visual Studio
2.  Set Demo as the Active Project
3.  Hit CTRL-F5 to rebuild the demo and execute.
4.  Follow the on-screen instructions.

The demo application uses the CFSM classes described in the Gem (look in the FSM directory)
in three example classes, CLightBulb, CFlashingLightBulb, and CRealisticLightBulb.  Each
of the classes illustrates the use of the CFSM class as well as extending the FSM's behavior
through the use of inheritance, function overloading, and new states.

Classes				States				Input Events
CLightBulb				On, Off				Switch On, Switch Off
CFlashingLightBulb		On, On Dim, Off			Switch On, Switch Off
CRealisticLightBulb		On, Off, Burned Out		Switch On, Switch Off

Keyboard Controls:
SPACE					Toggles Switch On/Switch Off events
ESC					Exit Demo
