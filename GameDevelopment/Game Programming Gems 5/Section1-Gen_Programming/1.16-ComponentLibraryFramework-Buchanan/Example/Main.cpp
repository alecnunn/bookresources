#include <stdio.h>


#include <string>


#include "../Component Library/ExampleConfiguration.hpp"


//	We now define some interfaces that our component will support


struct IMove
{
	virtual void GetPosition(float& x, float& y) = 0;
	virtual void SetPosition(float x, float y) = 0;
};


struct IName : ComponentLibrary::Version<0>	//	Invoke optional version support for this interface
{
	virtual const char* const GetName() = 0;
};


/*
	We will implement one of our interfaces via an embed-able helper
	class rather than use inheritence.
*/


struct IMove_Helper : IMove
{
	float x, y;


	void GetPosition(float& x, float& y)
	{
		x = this->x;
		y = this->y;
	}


	void SetPosition(float x, float y)
	{
		this->x = x;
		this->y = y;
	}
};


//	Now we can define the actual component itself


class MyComponent : public Component,
					public IName
{
	public:

		MyComponent()
		{
			//	Expose all the interfaces we support
			ExposeInterface<IMove>(&moveHelper);
			ExposeInterface<IName>(this);


			name = "Diebus Dan";
		}


		const char* const GetName()
			//	Implementation of IName::GetName()
		{
			return name.c_str();
		}


		MyComponent* Clone()
			//	Implementation of Component::Clone()
		{
			MyComponent* component = new MyComponent();


			component->moveHelper = moveHelper;
			component->name = name;


			return component;
		}


	private:

		IMove_Helper moveHelper;	//	Implementation of IMove
		std::string name;
};


void main()
{
	//	Use the factory singleton and add support for our component to it
	Factory::Singleton().Support<MyComponent>();


	//	Create our component using the factory singleton
	Component* component = Factory::Singleton().Create<MyComponent>();


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