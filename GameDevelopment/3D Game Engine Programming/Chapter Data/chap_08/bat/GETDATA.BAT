REM --- Kopieren der Renderer DLL ---
xcopy ..\support\zfxRenderDLL\ZFXD3D\include\zfx.h			include\*.* /Y /Q
xcopy ..\support\zfxRenderDLL\include\zfxRenderer.h			include\*.* /Y /Q
xcopy ..\support\zfxRenderDLL\include\zfxRenderDevice.h		include\*.* /Y /Q
xcopy ..\support\zfxRenderDLL\release\*.lib					lib\*.* /Y /Q
xcopy ..\support\zfxRenderDLL\release\*.dll					release\*.* /Y /Q
xcopy ..\support\zfxRenderDLL\ZFXD3D\release\*.lib			lib\*.* /Y /Q
xcopy ..\support\zfxRenderDLL\ZFXD3D\release\*.dll			release\*.* /Y /Q
REM --- Kopiern der Mathe Lib ---
xcopy ..\support\zfxMatheLib\include\zfx3d.h				include\*.* /Y /Q
xcopy ..\support\zfxMatheLib\release\*.lib					lib\*.* /Y /Q
