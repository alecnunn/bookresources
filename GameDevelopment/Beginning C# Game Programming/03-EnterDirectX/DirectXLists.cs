using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using Microsoft.DirectX.Direct3D;

namespace EnterDirectX {
	/// <summary>
	/// Summary description for DirectXLists.
	/// </summary>
	public class DirectXLists {
		private static int FrameRate, LastFrameRate, LastTick;

		private DirectXLists() {}

		public static int CalcFrameRate() {
			// Frame rate calculation
			if(System.Environment.TickCount - LastTick >= 1000) {
				LastFrameRate = FrameRate;
				FrameRate = 0;
				LastTick = System.Environment.TickCount;
			}
			FrameRate++;
			return LastFrameRate;
		}

		public static string DisplayModeName(Format modeFormat) {
			return modeFormat.ToString();
		}

		public static void ListGeneralCaps(Caps devCaps, ListBox listCaps) {
			listCaps.Items.Add(" -----  General Caps ------------------------");
			if(devCaps.MaxActiveLights == -1) {
				listCaps.Items.Add("Maximum Active Lights: Unlimited");
			}
			else {
				listCaps.Items.Add("Maximum Active Lights: " + devCaps.MaxActiveLights);
			}
			if(devCaps.MaxPointSize == 1) {
				listCaps.Items.Add("Device does not support point size control");
			}
			else {
				listCaps.Items.Add("Maximum point primitive size: " + devCaps.MaxPointSize);
			}
			listCaps.Items.Add("Maximum Primitives in each DrawPrimitives call: " + devCaps.MaxPrimitiveCount);
			listCaps.Items.Add("Maximum textures simultaneously bound: " + devCaps.MaxSimultaneousTextures);
			listCaps.Items.Add("Maximum Texture aspect ratio: " + devCaps.MaxTextureAspectRatio);
			listCaps.Items.Add("Maximum Texture size: " + devCaps.MaxTextureWidth + "x" + devCaps.MaxTextureHeight);
			listCaps.Items.Add("Maximum matrixes blending: " + devCaps.MaxVertexBlendMatrices);
			listCaps.Items.Add("Maximum vertex shaders registers: " + devCaps.MaxVertexShaderConst);
		}

		public static void ListTextureCaps(TextureCaps textureCaps, ListBox listCaps) {
			listCaps.Items.Add(" -----  Texture Caps ------------------------");
			if(textureCaps.SupportsPerspective) {
				listCaps.Items.Add("Perspective correction texturing is supported. ");
			}
			if(textureCaps.SupportsPower2) {
				listCaps.Items.Add("All textures must have widths and heights specified as powers of 2. "
					+ "This requirement does not apply to either cube textures or volume textures. ");
			}
			if(textureCaps.SupportsAlpha) {
				listCaps.Items.Add("Alpha in texture pixels is supported. ");
			}
			if(textureCaps.SupportsSquareOnly) {
				listCaps.Items.Add("All textures must be square. ");
			}
			if(textureCaps.SupportsTextureRepeatNotScaledBySize) {
				listCaps.Items.Add("Texture indices are not scaled by the texture size prior to interpolation. ");
			}
			if(textureCaps.SupportsAlphaPalette) {
				listCaps.Items.Add("Device can draw alpha from texture palettes. ");
			}
			if(textureCaps.SupportsNonPower2Conditional) {
				listCaps.Items.Add("Conditionally supports the use of textures with dimensions that are not powers of 2.. ");
			}
			if(textureCaps.SupportsProjected) {
				listCaps.Items.Add("Supports the projected texture transformation flag. ");
			}
			if(textureCaps.SupportsCubeMap) {
				listCaps.Items.Add("Supports cube textures. ");
			}
			if(textureCaps.SupportsVolumeMap) {
				listCaps.Items.Add("Device supports volume textures. ");
			}
			if(textureCaps.SupportsMipMap) {
				listCaps.Items.Add("Device supports mipmapped textures. ");
			}
			if(textureCaps.SupportsMipVolumeMap) {
				listCaps.Items.Add("Device supports mipmapped textures. ");
			}
			if(textureCaps.SupportsMipCubeMap) {
				listCaps.Items.Add("Device supports mipmapped cube textures. ");
			}
			if(textureCaps.SupportsCubeMapPower2) {
				listCaps.Items.Add("Device requires that cube texture maps have dimensions specified as powers of 2. ");
			}
			if(textureCaps.SupportsVolumeMapPower2) {
				listCaps.Items.Add("Device requires that volume texture maps have dimensions specified as powers of 2. ");
			}
		}

		public static void ListRasterCaps(RasterCaps rasterCaps, ListBox listCaps) {
			listCaps.Items.Add(" -----  Rasterizer Caps ------------------------");
			if(rasterCaps.SupportsDither) {
				listCaps.Items.Add("The device can dither to improve color resolution.");
			}


			if(rasterCaps.SupportsZBufferTest) {
				listCaps.Items.Add("The device can perform z-test operations.");
			}

			if(rasterCaps.SupportsFogVertex) {
				listCaps.Items.Add("The device calculates the fog value during the lighting operation using the D3DTLVERTEX. ");
			}

			if(rasterCaps.SupportsFogTable) {
				listCaps.Items.Add("The device calculates the fog value by referring to a lookup table.");
			}

			if(rasterCaps.SupportsMipMapLevelOfDetailBias) {
				listCaps.Items.Add("The device supports level-of-detail (LOD) bias adjustments. ");
			}

			if(rasterCaps.SupportsDepthBias) {
				listCaps.Items.Add("The device supports depth bias values.");
			}

			if(rasterCaps.SupportsZBufferLessHsr) {
				listCaps.Items.Add("The device can perform hidden-surface removal (HSR) without requiring the allocation of a depth-buffer");
			}

			if(rasterCaps.SupportsFogRange) {
				listCaps.Items.Add("The device supports range-based fog.");
			}

			if(rasterCaps.SupportsAnisotropy) {
				listCaps.Items.Add("The device supports anisotropic filtering.");
			}

			if(rasterCaps.SupportsWBuffer) {
				listCaps.Items.Add("The device supports depth buffering using w. ");
			}

			if(rasterCaps.SupportsWFog) {
				listCaps.Items.Add("The device supports w-based fog. ");
			}

			if(rasterCaps.SupportsZFog) {
				listCaps.Items.Add("The device supports z-based fog. ");
			}

			if(rasterCaps.SupportsColorPerspective) {
				listCaps.Items.Add("The device iterates colors perspective correct.");
			}
		}

		public static void ListDriverCaps(DriverCaps driverCaps, ListBox listCaps) {
			listCaps.Items.Add(" -----  Driver Caps ------------------------");
			if(driverCaps.SupportsDynamicTextures) {
				listCaps.Items.Add("The driver support Dynamic textures");
			}

			if(driverCaps.CanCalibrateGamma) {
				listCaps.Items.Add("The driver can automatically adjust the gamma ramp");
			}

			if(driverCaps.SupportsFullscreenGamma) {
				listCaps.Items.Add("The driver supports dynamic gamma ramp adjustment in full-screen mode. ");
			}
		}

		public static void ListDevCaps(DeviceCaps devCaps, ListBox listCaps) {
			listCaps.Items.Add(" -----  Device Caps ------------------------");

			if(devCaps.SupportsExecuteSystemMemory) {
				listCaps.Items.Add("Device can use execute buffers from system memory.");
			}
			if(devCaps.SupportsExecuteVideoMemory) {
				listCaps.Items.Add("Device can use execute buffers from video memory. ");
			}
			if(devCaps.SupportsTransformedVertexSystemMemory) {
				listCaps.Items.Add("Device can use buffers from system memory for transformed and lit vertices. ");
			}
			if(devCaps.SupportsTransformedVertexVideoMemory) {
				listCaps.Items.Add("Device can use buffers from video memory for transformed and lit vertices. ");
			}
			if(devCaps.SupportsTextureSystemMemory) {
				listCaps.Items.Add("Device can retrieve textures from system memory. ");
			}
			if(devCaps.SupportsTextureVideoMemory) {
				listCaps.Items.Add("Device can retrieve textures from device memory.");
			}
			if(devCaps.SupportsDrawPrimitivesTransformedVertex) {
				listCaps.Items.Add("Device exports a DrawPrimitives-aware hardware abstraction layer (HAL).");
			}
			if(devCaps.CanRenderAfterFlip) {
				listCaps.Items.Add("Device can queue rendering commands after a page flip.");
			}
			if(devCaps.SupportsTextureNonLocalVideoMemory) {
				listCaps.Items.Add("Device can retrieve textures from nonlocal video memory. ");
			}
			if(devCaps.SupportsSeparateTextureMemories) {
				listCaps.Items.Add("Device is texturing from separate memory pools. ");
			}
			if(devCaps.SupportsHardwareTransformAndLight) {
				listCaps.Items.Add("Device can support transformation and lighting in hardware. ");
			}
			if(devCaps.CanDrawSystemToNonLocal) {
				listCaps.Items.Add("Device supports blits from system-memory textures to nonlocal video-memory textures. ");
			}
			if(devCaps.SupportsHardwareRasterization) {
				listCaps.Items.Add("Device has hardware acceleration for scene rasterization. ");
			}
			if(devCaps.SupportsPureDevice) {
				listCaps.Items.Add("Device can support rasterization, transform, lighting, and shading in hardware. ");
			}
			if(devCaps.SupportsQuinticRtPatches) {
				listCaps.Items.Add("Device supports quintic béziers and B-splines.");
			}
			if(devCaps.SupportsRtPatches) {
				listCaps.Items.Add("Device supports high-order surfaces. ");
			}
			if(devCaps.SupportsRtPatchHandleZero) {
				listCaps.Items.Add("High-order surfaces can be drawn efficiently using a handle value of 0. ");
			}
			if(devCaps.SupportsNPatches) {
				listCaps.Items.Add("Device supports N patches. ");
			}
		}
	}
}
