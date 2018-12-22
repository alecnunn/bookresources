#include <stdio.h>


#include "../Component Library/ExampleConfiguration.hpp"


struct IMove
{
	virtual void GetPosition(float& x, float& y) = 0;
	virtual void SetPosition(float x, float y) = 0;
};


struct IName : ComponentLibrary::Version<0>
{
	virtual const char* const GetName() = 0;
};


class MyComponent;	//	Prototype for component we wish to use - note that it is dynamically linked


void main()
{
#ifndef _DEBUG
	DLLFactory factory("../../DLLComponent/Release/DLLComponent.dll");
#else
	DLLFactory factory("../../DLLComponent/Debug/DLLComponent.dll");
#endif
	Component* component = factory.Create<MyComponent>();


	if(!component)
		printf("Error creating component\n");
	else
	{
		//	Obtain the interfaces it supports and use them

		IName* name = component->QueryForInterface<IName>();


		if(name)
			printf("%s\n", name->GetName());
		else
			printf("Error obtaining interface IName\n");


		IMove* move = component->QueryForInterface<IMove>();


		if(move)
			move->SetPosition(5.0f, 4.2f);
		else
			printf("Error obtaining interface IMove\n");


		//	Release the component when we are done with it
		component->Release();
	}
}