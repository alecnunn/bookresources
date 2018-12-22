// Magic Software, Inc.
// http://www.magic-software.com
// http://www.wild-magic.com
// Copyright (c) 2004.  All Rights Reserved
//
// The Wild Magic Library (WM3) source code is supplied under the terms of
// the license agreement http://www.wild-magic.com/License/WildMagic3.pdf and
// may not be copied or disclosed except in accordance with the terms of that
// agreement.

#ifndef WEXPORTSETTINGS_H
#define WEXPORTSETTINGS_H

// Class WExportSettings encapsulates the exporter options as specified by
// the user from the exporter dialog's user interface. The options specified
// in the constructor represent defaults, which will be overridden if a valid
// exporter configuration file exists.

#include <wtypes.h>

class WExportSettings
{
public:
    WExportSettings ();

    // object settings
    BOOL IncludeObjects;
    BOOL IncludeCameras;
    BOOL IncludeLights;
    BOOL IncludeMeshes;

    // mesh settings
    BOOL IncludeMaterials;
    BOOL IncludeNormals;
    BOOL IncludeTexCoords;
    BOOL IncludeVertexColors;
    BOOL GenerateMaps;

    // modifier settings
    BOOL IncludeModifiers;
    BOOL IncludeSkins;

    // animation settings
    BOOL IncludeCurrentFrame;
    BOOL IncludeKeyFrames;
    BOOL IncludeAllFrames;
    BOOL UseLocalTime;
    int StartFrame;
    int EndFrame;
};

#endif
