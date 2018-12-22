
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

#ifndef __GPGBoneParser_h__
#define __GPGBoneParser_h__



/******************************************************************************
class GPGBoneParser: public GPGParser

******************************************************************************/
class GPGBoneParser: public GPGParser
	{
	public:
				GPGBoneParser(void);
virtual			~GPGBoneParser(void)		{ };

virtual	void	OutputHeaderBlock(void)		{ };

virtual	void	InputBoneData(GPGCharacter &character,bool justweights)
					{
					TransferBoneData(character,justweights);
					};
virtual	void	OutputBoneData(GPGCharacter &character)
					{
//					OutputHeaderBlock();

					SetLayoutMode(true);
					TransferBoneData(character,false);

					SetLayoutMode(false);
					GetParseFormat()->ResolveLayout(this);
					TransferBoneData(character,false);
					};

virtual	GPGRESULT	LoadBoneData(GPGCharacter &character,const char *filename,
															bool justweights)
					{
					GPGLOG("LoadBoneData() \"%s\"\n",filename);
					if(!filename || !filename[0])
						{
						GPGLOG("No bone filename for read");
						return GPG_E_INVALID_HANDLE;
						}
					if(!OpenInputFile(filename))
						{
						GPGLOG("Error opening bone file \"%s\" for read\n",
																	filename);
						return GPG_E_INVALID_FILE;
						}
					if(!justweights)
						character.Reset();
					InputBoneData(character,justweights);
					CloseInputFile();
					if(GetAbortCode()!=GPG_OK)
						GPGLOG("Read \"%s\" aborted\n",filename);

					GPGLOG(" done\n");
					return GetAbortCode();
					};
virtual	GPGRESULT	SaveBoneData(GPGCharacter &character,const char *filename)
					{
					GPGLOG("SaveBoneData() \"%s\"\n",filename);
					if(!filename || !filename[0])
						{
						GPGLOG("No bone filename for write");
						return GPG_E_INVALID_HANDLE;
						}
					if(!OpenOutputFile(filename))
						{
						GPGLOG("Error opening bone file \"%s\" for write\n",
																	filename);
						return GPG_E_INVALID_FILE;
						}
					OutputBoneData(character);
					CloseOutputFile();
					if(GetAbortCode()!=GPG_OK)
						GPGLOG("Read \"%s\" aborted\n",filename);

					GPGLOG(" done\n");
					return GetAbortCode();
					};

/*****************************************************************************/

virtual	void	TransferBoneData(GPGCharacter &character,bool justweights);

virtual	void	TransferCharacter(GPGParseEntry &entry);

virtual	void	TransferBoneNode(GPGParseEntry &entry);
static	BOOL	OutputBoneNode(GPGCoreNode &corenode,GPGVoidStar state);

virtual	void	TransferEffection(GPGParseEntry &entry);

virtual	void	InputBoneName(GPGParseEntry &entry);
virtual	void	TransferBoneName(GPGParseEntry &entry)
					{
					if(m_parsermode==INPUT)
						InputBoneName(entry);
/*
					else
						OutputBoneName(entry);
*/
					};

virtual	void	TransferVertexWeights(GPGParseEntry &entry);
virtual	void	TransferVertexWeight(GPGParseEntry &entry);

// virtual	void	TransferCoreNode(GPGParseEntry &entry);

virtual	void	TransferJoint(GPGParseEntry &entry);
virtual	void	TransferConstraints(GPGParseEntry &entry);

	protected:

		GPGParseBoneMapList	&GetBoneMapList(void)	{ return m_bonemaplist; };

	private:

		GPGParseBoneMapList	m_bonemaplist;
		GPGParseTable		m_vw_parsetable;
		char				progresstext[128];
	};



#endif /* __GPGBoneParser_h__ */
