#include "../Component Library/ExampleConfiguration.hpp"


struct IMove
{
	virtual void GetPosition(float& x, float& y) = 0;
	virtual void SetPosition(float x, float y) = 0;
};


struct IName : ComponentLibrary::Version<0>	//	Invoke optional version support for this interface
{
	virtual const char* const GetName() = 0;
};


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


extern "C" __declspec(dllexport) Factory* CreateFactory()
{
	Factory::Singleton().Support<MyComponent>();


	return &Factory::Singleton();
}