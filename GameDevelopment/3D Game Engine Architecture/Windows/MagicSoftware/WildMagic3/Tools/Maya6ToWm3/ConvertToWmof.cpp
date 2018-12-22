// Magic Software, Inc.
// http://www.magic-software.com
// Copyright (c) 2000-2003.  All Rights Reserved
//
// Source code from Magic Software is supplied under the terms of a license
// agreement and may not be copied or disclosed except in accordance with the
// terms of that agreement.  The various license agreements may be found at
// the Magic Software web site.  This file is subject to the license
//
// 3D GAME ENGINE SOURCE CODE
// http://www.magic-software.com/License/3DGameEngine.pdf

#include "Exporter.h"
#include "ConvertToWmof.h"

#include <maya/MGlobal.h>
#include <maya/MFnPlugin.h>

// This file contains the calls to the Maya Plugin-related API.
// No conversion is done here.

#define VERSION "1.01"
#define AUTHOR "Magic Software, Inc."
// Note: Due to a bug in Maya6.0 that makes it use the class name as the
// file type whenever you do an exportAll, the name of the file exporter
// should be exactly the same name as this class.
// #define NAME "WildMagic Object File"
#define NAME "ConvertToWmof"

// The .mel script for the options pane
#define EXPORT_OPTIONS_SCRIPT "ConvertToWmofOptions"

// Default options for that script
// See ConvertToWmofOptions.mel for descriptions of these options
#define EXPORT_DEFAULT_OPTIONS "objFile=1;textFile=0;"\
    "quiet=0;expCameras=1;expInvisible=0;expLights=1;"\
    "expTextures=1;expBump=1;expKeys=1;expSkin=1;expBind=0;"
//----------------------------------------------------------------------------
MStatus initializePlugin (MObject kObj)
{
	MStatus kStatus;
	MFnPlugin kPlugin(kObj, AUTHOR, VERSION, "Any");

	kStatus = kPlugin.registerFileTranslator(NAME, "",
        ConvertToWmof::creator, EXPORT_OPTIONS_SCRIPT, EXPORT_DEFAULT_OPTIONS);
	if (!kStatus) {
		kStatus.perror("registerCommand");
		return kStatus;
	}

	return kStatus;
}
//----------------------------------------------------------------------------
MStatus uninitializePlugin (MObject kObj)
{
	MStatus kStatus;
	MFnPlugin kPlugin(kObj, AUTHOR, VERSION, "Any");

	kStatus = kPlugin.deregisterFileTranslator( NAME );
	if (!kStatus) {
		kStatus.perror("deregisterFileTranslator");
	}

	return kStatus;
}
//----------------------------------------------------------------------------
ConvertToWmof::ConvertToWmof()
{
    // empty
}
//----------------------------------------------------------------------------
ConvertToWmof::~ConvertToWmof()
{
    // empty
}
//----------------------------------------------------------------------------
MStatus ConvertToWmof::doIt (const MArgList& rkArgs)
{
    return MS::kSuccess;
}
//----------------------------------------------------------------------------
void* ConvertToWmof::creator()
{
    return new ConvertToWmof;
}
//----------------------------------------------------------------------------
bool ConvertToWmof::haveReadMethod() const
{
	return false;
}
//----------------------------------------------------------------------------
bool ConvertToWmof::haveWriteMethod() const
{
	return true;
}
//----------------------------------------------------------------------------
MString ConvertToWmof::defaultExtension() const
{
	return MString("wmof");
}
//----------------------------------------------------------------------------
MString ConvertToWmof::filter() const
{
    return MString("*.wmof");
}
//----------------------------------------------------------------------------
MPxFileTranslator::MFileKind ConvertToWmof::identifyFile(
    const MFileObject& fileName,const char* buffer,short size) const
{
	const char* acName = fileName.name().asChar();
	int iNameLength = (int)strlen(acName);

	if ((iNameLength > 5) && !strcasecmp(acName+iNameLength-5, ".wmof")) {
		return kIsMyFileType;
	}

	return kNotMyFileType;
}
//----------------------------------------------------------------------------
MStatus ConvertToWmof::writer(const MFileObject& file,
    const MString& kOptions,FileAccessMode kMode)
{
    Exporter kExporter;

    // Read options string, if it exists.  It comes in the format
    // option1=value1;option2=value2; ... etc
	if (kOptions.length() > 0)
    {
		MStringArray kOptionList;
		MStringArray kThisOption;
		kOptions.split(';', kOptionList);

		int iNumOptions = (int)kOptionList.length();
		for (int i = 0; i < iNumOptions; i++)
        {
			kThisOption.clear();
			kOptionList[i].split('=', kThisOption);
			if (kThisOption.length() < 2 || !kThisOption[1].isInt())
            {
				continue;
			}

            bool bValue = (kThisOption[1].asInt() ? true : false);

            if (kThisOption[0] == "objFile")
                kExporter.m_abFlags[Exporter::FLAG_OBJFILE] = bValue;
            else if (kThisOption[0] == "textFile")
                kExporter.m_abFlags[Exporter::FLAG_TEXTFILE] = bValue;
            else if (kThisOption[0] == "quiet")
                kExporter.m_abFlags[Exporter::FLAG_QUIET] = bValue;
            else if (kThisOption[0] == "collapse")
                kExporter.m_abFlags[Exporter::FLAG_COLLAPSE] = bValue;
            else if (kThisOption[0] == "expCameras")
                kExporter.m_abFlags[Exporter::FLAG_CAMERAS] = bValue;
            else if (kThisOption[0] == "expInvisible")
                kExporter.m_abFlags[Exporter::FLAG_INVISIBLE] = bValue;
            else if (kThisOption[0] == "expLights")
                kExporter.m_abFlags[Exporter::FLAG_LIGHTS] = bValue;
            else if (kThisOption[0] == "expTextures")
                kExporter.m_abFlags[Exporter::FLAG_TEXTURES] = bValue;
            else if (kThisOption[0] == "expBump")
                kExporter.m_abFlags[Exporter::FLAG_BUMP] = bValue;
            else if (kThisOption[0] == "expKeys")
                kExporter.m_abFlags[Exporter::FLAG_KEYS] = bValue;
            else if (kThisOption[0] == "expSkin")
                kExporter.m_abFlags[Exporter::FLAG_SKIN] = bValue;
            else if (kThisOption[0] == "expBind")
                kExporter.m_abFlags[Exporter::FLAG_BIND] = bValue;
            else if (kThisOption[0] == "expCalcNorm")
                kExporter.m_abFlags[Exporter::FLAG_CALC_NORM] = bValue;
            else
                MGlobal::displayError("Unknown flag: " + kOptionList[i]);
		}
	}

    MGlobal::displayInfo("");

    kExporter.DoExport(file.fullName().asChar());

    if ( kExporter.Successful() )
    {
        MGlobal::displayInfo("Conversion was successful.");
        return MStatus::kSuccess;
    }
    else
    {
        MGlobal::displayInfo("Conversion had errors; see script editor.");
        // If Maya gets back a failure status, it won't save the file.
        // It's possible the user doesn't want to/can't fix the errors,
        // and so we will just return success anyway.
        // return MStatus::kFailure;
        return MStatus::kSuccess;
    }
}
//----------------------------------------------------------------------------