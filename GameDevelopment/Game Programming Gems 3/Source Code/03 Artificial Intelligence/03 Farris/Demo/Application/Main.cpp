// Non-Local Includes
#include "..\Global.h"
#include "..\LightBulb\LightBulb.h"
#include "..\LightBulb\FlashingLightBulb.h"
#include "..\LightBulb\RealisticLightBulb.h"

// Main
int main(int argc, char* argv[])
{
	cout << "Starting FSM Demo Application..." << endl;
	// Select Class
	cout << "Select Class: " << endl;
	cout << '\t' << "1. CLightBulb" << endl;
	cout << '\t' << "2. CFlashingLightBulb" << endl;
	cout << '\t' << "3. CRealisticLightBulb" << endl;
	cout << '\t' << "4. Exit" << endl;
	cout << "Class: ";
	int iClass=0;
	cin >> iClass;
	// Allocate Class
	CLightBulb *pLight=0;
	switch (iClass)
	{
	case 1:
		cout << "Creating Instance of CLightBulb..." << endl;
		pLight=new CLightBulb;
		break;
	case 2: 
		cout << "Creating Instance of CFlashingLightBulb..." << endl;
		pLight=new CFlashingLightBulb;
		break;
	case 3:
		cout << "Creating Instance of CRealisticLightBulb..." << endl;
		pLight=new CRealisticLightBulb;
		break;
	default:
		return 0;
		break;
	}
	// Display Instructions
	cout << endl;
	cout << "Instructions: " << endl;
	cout << "SPACE :  Toggle Switch Input Events" << endl;
	cout << "ESC   :  Exit " << endl;
	cout << endl;
	// Check Light
	if (pLight)
	{
		// Variables
		bool bSwitch=false;
		// Loop
		cout << "Starting Update Loop..." << endl;
		while (true)
		{
			// Update Light
			pLight->Update();
			// Check Input
			if (::GetAsyncKeyState(VK_SPACE)!=0)
			{
				// Toggle Switch
				bSwitch=!bSwitch;
				if (bSwitch)
					pLight->SwitchOnEvent();
				else
					pLight->SwitchOffEvent();
			}
			else if (::GetAsyncKeyState(VK_ESCAPE)!=0)
				break;
			// Sleep
			Sleep(100);
		}
		// Delete Light
		cout << "Destroying Instance..." << endl;
		delete pLight;
		pLight=0;
	}
	cout << "Exiting FSM Demo Application..." << endl;
	return 0;
}

