#ifndef _COMPONENTLIBRARY_REFERENCECOUNT_HPP
#define _COMPONENTLIBRARY_REFERENCECOUNT_HPP


#include <assert.h>


namespace ComponentLibrary
{
	class ReferenceCount
	{
		protected:

			ReferenceCount()
				:	references(1)
			{}


		public:

			virtual ~ReferenceCount()
			{}


			void Reference()
			{
				references++;
			}


			void Release()
			{
				assert(references && "Reference count has already reached zero before release");

				if(--references == 0)
					delete this;
			}


		private:

			unsigned references;
	};
}


#endif