

This example shows the use of fog in a program with vertex and pixel shaders.  Note that the standard D3D fog calculation happens after the pixel shader program.

The gray screen depth based fog is output to oFog.x for the default fog processing.  For this to work properly, it is necessary to set:
    SetRenderState( D3DRS_FOGENABLE,     true );
    SetRenderState( D3DRS_FOGVERTEXMODE, D3DFOG_NONE );  
    SetRenderState( D3DRS_FOGTABLEMODE,  D3DFOG_NONE );  
    SetRenderState( D3DRS_FOGCOLOR,      0x00aaaaaa  );
You must set D3DFOG_NONE even though this is the supposed 'default'!

The other fog value is derived from the landscape height (modelspace x coordinate) and written to oD1, the output specular color.  The pixel shader adds this color to that of the landscape for the height fog effect.

Hit F1 or 'H' while running to see a list of user interface keys.

Help : F1 - Help
W    - Wireframe toggle

Home - Reset view & terrain

. >	- Increase height fog
, <	- Decrease height fog

R	- Increase vertical tesselation
F	- Decrease  ""
G	- Increase horizontal tesselation
D	- Decrease  ""

W	- Wireframe
Arrows  - Move camera
Home	- Reset camera & landscape
PgUP	- Move camera up
PgDown	- Move camera down



-Greg J.
NVIDIA Developer Relations







