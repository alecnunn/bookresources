
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

#ifndef __GPGMotionParser_h__
#define __GPGMotionParser_h__



/******************************************************************************
class GPGMotionParser: public GPGParser

******************************************************************************/
class GPGMotionParser: public GPGParser
	{
	public:

				GPGMotionParser(void);
virtual			~GPGMotionParser(void)		{ };

virtual void	OutputHeaderBlock(void)		{ };

virtual	void	InputMotionData(GPGMotion &motion)
					{
					TransferMotionData(motion);
					};
virtual	void	OutputMotionData(GPGMotion &motion)
					{
//					OutputHeaderBlock();

					SetLayoutMode(true);
					TransferMotionData(motion);

					SetLayoutMode(false);
					GetParseFormat()->ResolveLayout(this);
					TransferMotionData(motion);
					};

virtual	GPGRESULT	LoadMotionData(GPGMotion &motion,const char *filename)
					{
//					GPGLOG("LoadMotionData() \"%s\"\n",filename);
					if(!filename || !filename[0])
						{
						GPGLOG("No motion filename for read\n");
						return GPG_E_INVALID_HANDLE;
						}
					if(!OpenInputFile(filename))
						{
						GPGLOG("Error opening motion \"%s\" for read\n",
																	filename);
						return GPG_E_INVALID_FILE;
						}
					InputMotionData(motion);
					CloseInputFile();
					if(GetAbortCode()!=GPG_OK)
						GPGLOG("Read \"%s\" aborted\n",filename);

					GPGLOG(" done\n");
					return GetAbortCode();
					};
virtual	GPGRESULT	SaveMotionData(GPGMotion &motion,const char *filename)
					{
					GPGLOG("SaveMotionData() \"%s\"\n",filename);
					if(!filename || !filename[0])
						{
						GPGLOG("No motion filename for write\n");
						return GPG_E_INVALID_HANDLE;
						}
					if(!OpenOutputFile(filename))
						{
						GPGLOG("Error opening motion \"%s\" for write\n",
																	filename);
						return GPG_E_INVALID_FILE;
						}
					OutputMotionData(motion);
					CloseOutputFile();
					if(GetAbortCode()!=GPG_OK)
						GPGLOG("Write \"%s\" aborted\n",filename);

					GPGLOG(" done\n");
					return GetAbortCode();
					};

/*****************************************************************************/

virtual	void	TransferMotionData(GPGMotion &motion);

virtual	void	TransferMotion(GPGParseEntry &entry);
virtual	void	TransferTrack(GPGParseEntry &entry);
virtual	void	TransferFrame(GPGParseEntry &entry);

	private:
		GPGParseTable	m_frame_parsetable;
		char			progresstext[128];

	};



#endif /* __GPGMotionParser_h__ */
