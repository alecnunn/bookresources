
/******************************************************************************
IMPORTANT: READ BEFORE DOWNLOADING, COPYING, INSTALLING OR USING.

By downloading, copying, installing or using the software you agree to this
license. If you do not agree to this license, do not download, install,
copy or use the software.

Intel Open Source License for "Improved Bones Deformation" and
"Constrained Inverse Kinematics" sample code

Copyright (c) 2002, Intel Corporation.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
* Redistributions of source code must retain the above copyright notice, this
	list of conditions and the following disclaimer.
* Redistributions in binary form must reproduce the above copyright notice,
	this list of conditions and the following disclaimer in the documentation
	and/or other materials provided with the distribution.
* Neither the name of Intel Corporation nor the names of its contributors may
	be used to endorse or promote products derived from this software without
	specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ''AS IS''
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE INTEL CORPORATION OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
EXPORT LAWS: THIS LICENSE ADDS NO RESTRICTIONS TO THE EXPORT LAWS OF YOUR
JURISDICTION. It is each licensee's responsibility to comply with any export
regulations applicable in each licensee's jurisdiction.
******************************************************************************/

#ifndef __GPGTransformStack_h__
#define __GPGTransformStack_h__



/******************************************************************************
class GPGTransformStack

	implemented as never shrinking, only expanding
	we could fix that later if it becomes a problem

	TODO inherit from existing GPGArray to take care of resizing

******************************************************************************/
class GPGTransformStack
	{
	public:
						GPGTransformStack(int initial_size=16)
							{
							m_current=0;
							m_filled=0;
							m_allocated=0;
							m_stack=NULL;
							Resize(initial_size);
							};

						~GPGTransformStack(void)
							{
							if(m_stack)
								GPGDeallocate(m_stack);
							};

		GPGTransform	&GetCurrent(void)			{ return m_stack[m_current]; };
		GPGTransform	&GetCurrentConst(void) const
													{ return m_stack[m_current]; };

		int				GetLevel(void) const		{ return m_current; };

		void			Push(void);
		bool			Pop(void);

	private:
		GPGTransform	&GetTransform(int level);
		void			Resize(long needed);

		GPGTransform	*m_stack;
		int				m_current;
		int				m_filled;
		int				m_allocated;
	};

GPGINLINE void GPGTransformStack::Push(void)
	{
	// NOTE: new transform's constructor not run

	m_current++;
	Resize(m_current+1);

	// NOTE: this copies everything that is valid
	// watch out for efficiency here
	m_stack[m_current]=m_stack[m_current-1];
	}

GPGINLINE bool GPGTransformStack::Pop(void)
	{
	if(m_current<1)
		return false;

	m_current--;
	return true;
	}

GPGINLINE GPGTransform &GPGTransformStack::GetTransform(int level)
	{
	// if nothing there, return bottom-most
	if(level<0 || level>m_filled-1)
		level=0;

	return m_stack[level];
	}

GPGINLINE void GPGTransformStack::Resize(long needed)
	{
	if(m_allocated<needed)
		{
		m_allocated*=2;

		if(m_allocated<needed)
			m_allocated=needed;

		m_stack=(GPGTransform *)GPGReallocate(m_stack,
					m_allocated*sizeof(GPGTransform));
		}

	if(m_filled<needed)
		m_filled=needed;
	}



#endif /* __GPGTransformStack_h__ */
