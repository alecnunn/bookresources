#ifndef _DX9KIT_D3DCHECK_HPP
#define _DX9KIT_D3DCHECK_HPP


#include <windows.h>


struct D3DCheck
{
	D3DCheck& operator=(HRESULT error)
	{
		if(error != D3D_OK)
		{
			#ifdef _DEBUG
				_asm int 3;
			#endif
		}


		return *this;
	}
};


#endif