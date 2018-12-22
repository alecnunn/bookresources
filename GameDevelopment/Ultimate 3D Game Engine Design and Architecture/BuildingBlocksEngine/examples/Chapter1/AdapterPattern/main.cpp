/*
   Adapter Class Demo - Chapter 1
   Ultimate Game Engine Design and Architecture
   Created by Allen Sherrod
*/


#include<iostream>


class DogInterface
{
   public:
      DogInterface()
      {
      }

      virtual ~DogInterface()
      {
      }

      virtual void Bark()
      {
         std::cout << "Woof I'm a dog!" << std::endl;
      }
};


class CatInterface
{
   public:
      CatInterface()
      {
      }

      virtual ~CatInterface()
      {
      }

      virtual void Meow()
      {
         std::cout << "Meow I'm a cat!" << std::endl;
      }
};


class DogObjectAdapter : public CatInterface
{
   public:
      DogObjectAdapter(DogInterface d)
      {
         m_dog = d;
      }

      virtual ~DogObjectAdapter()
      {
      }

      void Meow()
      {
         m_dog.Bark();
      }

   private:
      DogInterface m_dog;
};


class DogClassAdapter : public CatInterface, DogInterface
{
   public:
      DogClassAdapter()
      {
      }

      virtual ~DogClassAdapter()
      {
      }
};


void CatTalk(CatInterface &cat)
{
   cat.Meow();
}


int main(int args, char argc[])
{
   CatInterface cat;
   DogInterface dog;

   DogObjectAdapter dogObjectAdapter(dog);
   DogClassAdapter dogClassAdapter;

   CatTalk(cat);
   CatTalk(dogObjectAdapter);
   CatTalk(dogClassAdapter);

   return 1;
}