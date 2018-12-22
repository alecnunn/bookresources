//-----------------------------------------------------------------------------
// File: D3DFont.cs
//
// Desc: Shortcut functions for using DX objects
//
// Copyright (c) Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
using System;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using Direct3D = Microsoft.DirectX.Direct3D;




/// <summary>
/// A generic font class 
/// </summary>
public class GraphicsFont {
	public const int MaxNumfontVertices = 50*6;
  
	// Font rendering flags
	[System.Flags]
		public enum RenderFlags {
		Centered = 0x0001,
		TwoSided = 0x0002, 
		Filtered = 0x0004,
	}
	private System.Drawing.Font systemFont;

	private bool isZEnable = false;
	public bool ZBufferEnable { get { return isZEnable; } set { isZEnable = value; } }
	string ourFontName; // Font properties
	int ourFontHeight;

	private Direct3D.Device device;
	private TextureState textureState0;
	private TextureState textureState1;
	private Sampler samplerState0;
	private RenderStates renderState;
	private Direct3D.Texture fontTexture;
	private Direct3D.VertexBuffer vertexBuffer;
	private CustomVertex.TransformedColoredTextured[] fontVertices = new CustomVertex.TransformedColoredTextured[MaxNumfontVertices];

	private int textureWidth; // Texture dimensions
	private int textureHeight;
	private float textureScale;
	private int spacingPerChar;
	private float[,] textureCoords = new float[128-32,4];

	// Stateblocks for setting and restoring render states
	private StateBlock savedStateBlock;
	private StateBlock drawTextStateBlock;




	/// <summary>
	/// Create a new font object
	/// </summary>
	/// <param name="f">The font to use</param>
	public GraphicsFont(System.Drawing.Font f) {
		ourFontName = f.Name;
		ourFontHeight = (int)f.Size;
		systemFont = f;
	}

    
    
	/// <summary>
	/// Create a new font object
	/// </summary>
	public GraphicsFont(string fontName) : this(fontName, FontStyle.Regular, 12) {
	}


    
    
	/// <summary>
	/// Create a new font object
	/// </summary>
	public GraphicsFont(string fontName, FontStyle style) : this(fontName, style, 12) {
	}




	/// <summary>
	/// Create a new font object
	/// </summary>
	/// <param name="fontName">The name of the font</param>
	/// <param name="style">The style</param>
	/// <param name="size">Size of the font</param>
	public GraphicsFont(string fontName, FontStyle style, int size) {
		ourFontName = fontName;
		ourFontHeight = size;
		systemFont = new System.Drawing.Font(fontName, ourFontHeight, style);
	}




	/// <summary>
	/// Attempt to draw the systemFont alphabet onto the provided texture
	/// graphics.
	/// </summary>
	/// <param name="g"></param>Graphics object on which to draw and measure the letters</param>
	/// <param name="measureOnly">If set, the method will test to see if the alphabet will fit without actually drawing</param>
	public void PaintAlphabet(Graphics g, bool measureOnly) {
		string str;
		float x = 0;
		float y = 0;
		Point p = new Point(0, 0);
		Size size = new Size(0,0);
            
		// Calculate the spacing between characters based on line height
		size = g.MeasureString(" ", systemFont).ToSize();
		x = spacingPerChar = (int) Math.Ceiling(size.Height * 0.3);

		for (char c = (char)32; c < (char)127; c++) {
			str = c.ToString();
			// We need to do some things here to get the right sizes.  The default implemententation of MeasureString
			// will return a resolution independant size.  For our height, this is what we want.  However, for our width, we 
			// want a resolution dependant size.
			Size resSize = g.MeasureString(str, systemFont).ToSize();
			size.Height = resSize.Height + 1;

			// Now the Resolution independent width
			if (c != ' ') { // We need the special case here because a space has a 0 width in GenericTypoGraphic stringformats
				resSize = g.MeasureString(str, systemFont, p, StringFormat.GenericTypographic).ToSize();
				size.Width = resSize.Width;
			}
			else
				size.Width = resSize.Width;

			if ((x + size.Width + spacingPerChar) > textureWidth) {
				x = spacingPerChar;
				y += size.Height;
			}

			// Make sure we have room for the current character
			if ((y + size.Height) > textureHeight)
				throw new System.InvalidOperationException("Texture too small for alphabet");
                
			if (!measureOnly) {
				if (c != ' ') // We need the special case here because a space has a 0 width in GenericTypoGraphic stringformats
					g.DrawString(str, systemFont, Brushes.White, new Point((int)x, (int)y), StringFormat.GenericTypographic);
				else
					g.DrawString(str, systemFont, Brushes.White, new Point((int)x, (int)y));
				textureCoords[c-32,0] = ((float) (x + 0           - spacingPerChar)) / textureWidth;
				textureCoords[c-32,1] = ((float) (y + 0           + 0)) / textureHeight;
				textureCoords[c-32,2] = ((float) (x + size.Width  + spacingPerChar)) / textureWidth;
				textureCoords[c-32,3] = ((float) (y + size.Height + 0)) / textureHeight;
			}

			x += size.Width + (2 * spacingPerChar);
		}

	}




	/// <summary>
	/// Initialize the device objects
	/// </summary>
	/// <param name="dev">The grpahics device used to initialize</param>
	public void InitializeDeviceObjects(Device dev) {
		if (dev != null) {
			// Set up our events
			dev.DeviceReset += new System.EventHandler(this.RestoreDeviceObjects);
		}

		// Keep a local copy of the device
		device = dev;
		textureState0 = device.TextureState[0];
		textureState1 = device.TextureState[1];
		samplerState0 = device.SamplerState[0];
		renderState = device.RenderState;

		// Create a bitmap on which to measure the alphabet
		Bitmap bmp = new Bitmap(1, 1, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
		Graphics g = Graphics.FromImage(bmp);
		g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
		g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
		g.TextContrast = 0;
       
		// Establish the font and texture size
		textureScale  = 1.0f; // Draw fonts into texture without scaling

		// Calculate the dimensions for the smallest power-of-two texture which
		// can hold all the printable characters
		textureWidth = textureHeight = 128;
		for (;;) {
			try {
				// Measure the alphabet
				PaintAlphabet(g, true);
			}
			catch (System.InvalidOperationException) {
				// Scale up the texture size and try again
				textureWidth *= 2;
				textureHeight *= 2;
				continue;
			}

			break;
		}

		// If requested texture is too big, use a smaller texture and smaller font,
		// and scale up when rendering.
		Direct3D.Caps d3dCaps = device.DeviceCaps;

		// If the needed texture is too large for the video card...
		if (textureWidth > d3dCaps.MaxTextureWidth) {
			// Scale the font size down to fit on the largest possible texture
			textureScale = (float)d3dCaps.MaxTextureWidth / (float)textureWidth;
			textureWidth = textureHeight = d3dCaps.MaxTextureWidth;

			for(;;) {
				// Create a new, smaller font
				ourFontHeight = (int) Math.Floor(ourFontHeight * textureScale);      
				systemFont = new System.Drawing.Font(systemFont.Name, ourFontHeight, systemFont.Style);
                
				try {
					// Measure the alphabet
					PaintAlphabet(g, true);
				}
				catch (System.InvalidOperationException) {
					// If that still doesn't fit, scale down again and continue
					textureScale *= 0.9F;
					continue;
				}

				break;
			}
		}

		// Release the bitmap used for measuring and create one for drawing
		bmp.Dispose();
		bmp = new Bitmap(textureWidth, textureHeight, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
		g = Graphics.FromImage(bmp);
		g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
		g.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
		g.TextContrast = 0;

		// Draw the alphabet
		PaintAlphabet(g, false);

		// Create a new texture for the font from the bitmap we just created
		fontTexture = Texture.FromBitmap(device, bmp, 0, Pool.Managed);
		RestoreDeviceObjects(null, null);
	}




	/// <summary>
	/// Restore the font after a device has been reset
	/// </summary>
	public void RestoreDeviceObjects(object sender, EventArgs e) {
		vertexBuffer = new VertexBuffer(typeof(CustomVertex.TransformedColoredTextured), MaxNumfontVertices,
			device, Usage.WriteOnly | Usage.Dynamic, 0, Pool.Default);

		Surface surf = device.GetRenderTarget( 0 );
		bool bSupportsAlphaBlend = Manager.CheckDeviceFormat(device.DeviceCaps.AdapterOrdinal, 
			device.DeviceCaps.DeviceType, device.DisplayMode.Format, 
			Usage.RenderTarget | Usage.QueryPostPixelShaderBlending, ResourceType.Surface, 
			surf.Description.Format );

		// Create the state blocks for rendering text
		for (int which=0; which < 2; which++) {
			device.BeginStateBlock();
			device.SetTexture(0, fontTexture);

			if (isZEnable)
				renderState.ZBufferEnable = true;
			else
				renderState.ZBufferEnable = false;

			if( bSupportsAlphaBlend ) {
				renderState.AlphaBlendEnable = true;
				renderState.SourceBlend = Blend.SourceAlpha;
				renderState.DestinationBlend = Blend.InvSourceAlpha;
			}
			else {
				renderState.AlphaBlendEnable = false;
			}
			renderState.AlphaTestEnable = true;
			renderState.ReferenceAlpha = 0x08;
			renderState.AlphaFunction = Compare.GreaterEqual;
			renderState.FillMode = FillMode.Solid;
			renderState.CullMode = Cull.CounterClockwise;
			renderState.StencilEnable = false;
			renderState.Clipping = true;
			device.ClipPlanes.DisableAll();
			renderState.VertexBlend = VertexBlend.Disable;
			renderState.IndexedVertexBlendEnable = false;
			renderState.FogEnable = false;
			renderState.ColorWriteEnable = ColorWriteEnable.RedGreenBlueAlpha;
			textureState0.ColorOperation = TextureOperation.Modulate;
			textureState0.ColorArgument1 = TextureArgument.TextureColor;
			textureState0.ColorArgument2 = TextureArgument.Diffuse;
			textureState0.AlphaOperation = TextureOperation.Modulate;
			textureState0.AlphaArgument1 = TextureArgument.TextureColor;
			textureState0.AlphaArgument2 = TextureArgument.Diffuse;
			textureState0.TextureCoordinateIndex = 0;
			textureState0.TextureTransform = TextureTransform.Disable; // REVIEW
			textureState1.ColorOperation = TextureOperation.Disable;
			textureState1.AlphaOperation = TextureOperation.Disable;
			samplerState0.MinFilter = TextureFilter.Point;
			samplerState0.MagFilter = TextureFilter.Point;
			samplerState0.MipFilter = TextureFilter.None;

			if (which==0)
				savedStateBlock = device.EndStateBlock();
			else
				drawTextStateBlock = device.EndStateBlock();
		}
	}




	/// <summary>
	/// Draw some text on the screen
	/// </summary>
	public void DrawText(float xpos, float ypos, Color color, string text) {
		DrawText(xpos, ypos, color, text, RenderFlags.Filtered);
	}




	/// <summary>
	/// Draw some text on the screen
	/// </summary>
	public void DrawText(float xpos, float ypos, Color color, string text, RenderFlags flags) {
		if (text == null)
			return;

		// Setup renderstate
		savedStateBlock.Capture();
		drawTextStateBlock.Apply();
		device.SetTexture(0, fontTexture);
		device.VertexFormat = CustomVertex.TransformedColoredTextured.Format;
		device.PixelShader = null;
		device.SetStreamSource(0, vertexBuffer, 0);

		// Set filter states
		if ((flags & RenderFlags.Filtered) != 0) {
			samplerState0.MinFilter = TextureFilter.Linear;
			samplerState0.MagFilter = TextureFilter.Linear;
		}

		// Adjust for character spacing
		xpos -= spacingPerChar;
		float fStartX = xpos;

		// Fill vertex buffer
		int iv = 0;
		int dwNumTriangles = 0;

		foreach (char c in text) {
			if (c == '\n') {
				xpos = fStartX;
				ypos += (textureCoords[0,3]-textureCoords[0,1])*textureHeight;
			}

			if ((c-32) < 0 || (c-32) >= 128-32)
				continue;

			float tx1 = textureCoords[c-32,0];
			float ty1 = textureCoords[c-32,1];
			float tx2 = textureCoords[c-32,2];
			float ty2 = textureCoords[c-32,3];

			float w = (tx2-tx1) *  textureWidth / textureScale;
			float h = (ty2-ty1) * textureHeight / textureScale;

			int intColor = color.ToArgb();
			if (c != ' ') {
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+0-0.5f,ypos+h-0.5f,0.9f,1.0f), intColor, tx1, ty2);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+0-0.5f,ypos+0-0.5f,0.9f,1.0f), intColor, tx1, ty1);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+w-0.5f,ypos+h-0.5f,0.9f,1.0f), intColor, tx2, ty2);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+w-0.5f,ypos+0-0.5f,0.9f,1.0f), intColor, tx2, ty1);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+w-0.5f,ypos+h-0.5f,0.9f,1.0f), intColor, tx2, ty2);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+0-0.5f,ypos+0-0.5f,0.9f,1.0f), intColor, tx1, ty1);
				dwNumTriangles += 2;

				if (dwNumTriangles*3 > (MaxNumfontVertices-6)) {
					// Set the data for the vertexbuffer
					vertexBuffer.SetData(fontVertices, 0, LockFlags.Discard);
					device.DrawPrimitives(PrimitiveType.TriangleList, 0, dwNumTriangles);
					dwNumTriangles = 0;
					iv = 0;
				}
			}

			xpos += w - (2 * spacingPerChar);
		}

		// Set the data for the vertex buffer
		vertexBuffer.SetData(fontVertices, 0, LockFlags.Discard);
		if (dwNumTriangles > 0)
			device.DrawPrimitives(PrimitiveType.TriangleList, 0, dwNumTriangles);

		// Restore the modified renderstates
		savedStateBlock.Apply();
	}




	/// <summary>
	/// Draws scaled 2D text.  Note that x and y are in viewport coordinates
	/// (ranging from -1 to +1).  fXScale and fYScale are the size fraction 
	/// relative to the entire viewport.  For example, a fXScale of 0.25 is
	/// 1/8th of the screen width.  This allows you to output text at a fixed
	/// fraction of the viewport, even if the screen or window size changes.
	/// </summary>
	public void DrawTextScaled(float x, float y, float z, 
		float fXScale, float fYScale, 
		System.Drawing.Color color,
		string text, RenderFlags flags) {
		if (device == null)
			throw new System.ArgumentNullException();

		// Set up renderstate
		savedStateBlock.Capture();
		drawTextStateBlock.Apply();
		device.VertexFormat = CustomVertex.TransformedColoredTextured.Format;
		device.PixelShader = null;
		device.SetStreamSource(0, vertexBuffer, 0);

		// Set filter states
		if ((flags & RenderFlags.Filtered) != 0) {
			samplerState0.MinFilter = TextureFilter.Linear;
			samplerState0.MagFilter = TextureFilter.Linear;
		}

		Viewport vp = device.Viewport;
		float xpos = (x+1.0f)*vp.Width/2;
		float ypos = (y+1.0f)*vp.Height/2;
		float sz = z;
		float rhw = 1.0f;
		float fLineHeight = (textureCoords[0,3] - textureCoords[0,1]) * textureHeight;

		// Adjust for character spacing
		xpos -= spacingPerChar * (fXScale*vp.Height)/fLineHeight;
		float fStartX = xpos;

		// Fill vertex buffer
		int numTriangles = 0;
		int realColor = color.ToArgb();
		int iv = 0;

		foreach (char c in text) {
			if (c == '\n') {
				xpos  = fStartX;
				ypos += fYScale*vp.Height;
			}

			if ((c-32) < 0 || (c-32) >= 128-32)
				continue;

			float tx1 = textureCoords[c-32,0];
			float ty1 = textureCoords[c-32,1];
			float tx2 = textureCoords[c-32,2];
			float ty2 = textureCoords[c-32,3];

			float w = (tx2-tx1)*textureWidth;
			float h = (ty2-ty1)*textureHeight;

			w *= (fXScale*vp.Height)/fLineHeight;
			h *= (fYScale*vp.Height)/fLineHeight;

			if (c != ' ') {
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+0-0.5f,ypos+h-0.5f,sz,rhw), realColor, tx1, ty2);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+0-0.5f,ypos+0-0.5f,sz,rhw), realColor, tx1, ty1);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+w-0.5f,ypos+h-0.5f,sz,rhw), realColor, tx2, ty2);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+w-0.5f,ypos+0-0.5f,sz,rhw), realColor, tx2, ty1);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+w-0.5f,ypos+h-0.5f,sz,rhw), realColor, tx2, ty2);
				fontVertices[iv++] = new CustomVertex.TransformedColoredTextured(new Vector4(xpos+0-0.5f,ypos+0-0.5f,sz,rhw), realColor, tx1, ty1);
				numTriangles += 2;

				if (numTriangles*3 > (MaxNumfontVertices-6)) {
					// Unlock, render, and relock the vertex buffer
					vertexBuffer.SetData(fontVertices, 0, LockFlags.Discard);
					device.DrawPrimitives(PrimitiveType.TriangleList , 0, numTriangles);
					numTriangles = 0;
					iv = 0;
				}
			}

			xpos += w - (2 * spacingPerChar) * (fXScale*vp.Height)/fLineHeight;
		}

		// Unlock and render the vertex buffer
		vertexBuffer.SetData(fontVertices, 0, LockFlags.Discard);
		if (numTriangles > 0)
			device.DrawPrimitives(PrimitiveType.TriangleList , 0, numTriangles);

		// Restore the modified renderstates
		savedStateBlock.Apply();
	}




	/// <summary>
	/// Draws scaled 2D text.  Note that x and y are in viewport coordinates
	/// (ranging from -1 to +1).  fXScale and fYScale are the size fraction 
	/// relative to the entire viewport.  For example, a fXScale of 0.25 is
	/// 1/8th of the screen width.  This allows you to output text at a fixed
	/// fraction of the viewport, even if the screen or window size changes.
	/// </summary>
	public void DrawTextScaled(float x, float y, float z, 
		float fXScale, float fYScale, 
		System.Drawing.Color color,
		string text) {
		this.DrawTextScaled(x,y,z,fXScale, fYScale, color, text, 0);
	}




	/// <summary>
	/// Renders 3D text
	/// </summary>
	public void Render3DText(string text, RenderFlags flags) {
		if (device == null)
			throw new System.ArgumentNullException();

		// Set up renderstate
		savedStateBlock.Capture();
		drawTextStateBlock.Apply();
		device.VertexFormat = CustomVertex.PositionNormalTextured.Format;
		device.PixelShader = null;
		device.SetStreamSource(0, vertexBuffer, 0, VertexInformation.GetFormatSize(CustomVertex.PositionNormalTextured.Format));

		// Set filter states
		if ((flags & RenderFlags.Filtered) != 0) {
			samplerState0.MinFilter = TextureFilter.Linear;
			samplerState0.MagFilter = TextureFilter.Linear;
		}

		// Position for each text element
		float x = 0.0f;
		float y = 0.0f;

		// Center the text block at the origin
		if ((flags & RenderFlags.Centered) != 0) {
			System.Drawing.SizeF sz = GetTextExtent(text);
			x = -(((float)sz.Width)/10.0f)/2.0f;
			y = -(((float)sz.Height)/10.0f)/2.0f;
		}

		// Turn off culling for two-sided text
		if ((flags & RenderFlags.TwoSided) != 0)
			renderState.CullMode = Cull.None;

		// Adjust for character spacing
		x -= spacingPerChar / 10.0f;
		float fStartX = x;

		// Fill vertex buffer
		GraphicsStream strm = vertexBuffer.Lock(0, 0, LockFlags.Discard);
		int numTriangles = 0;

		foreach (char c in text) {
			if (c == '\n') {
				x = fStartX;
				y -= (textureCoords[0,3]-textureCoords[0,1])*textureHeight/10.0f;
			}

			if ((c-32) < 0 || (c-32) >= 128-32)
				continue;

			float tx1 = textureCoords[c-32,0];
			float ty1 = textureCoords[c-32,1];
			float tx2 = textureCoords[c-32,2];
			float ty2 = textureCoords[c-32,3];

			float w = (tx2-tx1) * textureWidth  / (10.0f * textureScale);
			float h = (ty2-ty1) * textureHeight / (10.0f * textureScale);

			if (c != ' ') {
				strm.Write(new CustomVertex.PositionNormalTextured(new Vector3(x+0,y+0,0), new Vector3(0,0,-1), tx1, ty2));
				strm.Write(new CustomVertex.PositionNormalTextured(new Vector3(x+0,y+h,0), new Vector3(0,0,-1), tx1, ty1));
				strm.Write(new CustomVertex.PositionNormalTextured(new Vector3(x+w,y+0,0), new Vector3(0,0,-1), tx2, ty2));
				strm.Write(new CustomVertex.PositionNormalTextured(new Vector3(x+w,y+h,0), new Vector3(0,0,-1), tx2, ty1));
				strm.Write(new CustomVertex.PositionNormalTextured(new Vector3(x+w,y+0,0), new Vector3(0,0,-1), tx2, ty2));
				strm.Write(new CustomVertex.PositionNormalTextured(new Vector3(x+0,y+h,0), new Vector3(0,0,-1), tx1, ty1));
				numTriangles += 2;

				if (numTriangles*3 > (MaxNumfontVertices-6)) {
					// Unlock, render, and relock the vertex buffer
					vertexBuffer.Unlock();
					device.DrawPrimitives(PrimitiveType.TriangleList , 0, numTriangles);
					strm = vertexBuffer.Lock(0, 0, LockFlags.Discard);
					numTriangles = 0;
				}
			}

			x += w - (2 * spacingPerChar) / 10.0f;
		}

		// Unlock and render the vertex buffer
		vertexBuffer.Unlock();
		if (numTriangles > 0)
			device.DrawPrimitives(PrimitiveType.TriangleList , 0, numTriangles);

		// Restore the modified renderstates
		savedStateBlock.Apply();
	}




	/// <summary>
	/// Get the dimensions of a text string
	/// </summary>
	private System.Drawing.SizeF GetTextExtent(string text) {
		if (null == text || text == string.Empty)
			throw new System.ArgumentNullException();

		float fRowWidth  = 0.0f;
		float fRowHeight = (textureCoords[0,3]-textureCoords[0,1])*textureHeight;
		float fWidth     = 0.0f;
		float fHeight    = fRowHeight;

		foreach (char c in text) {
			if (c == '\n') {
				fRowWidth = 0.0f;
				fHeight  += fRowHeight;
			}

			if ((c-32) < 0 || (c-32) >= 128-32)
				continue;

			float tx1 = textureCoords[c-32,0];
			float tx2 = textureCoords[c-32,2];

			fRowWidth += (tx2-tx1)*textureWidth - 2*spacingPerChar;

			if (fRowWidth > fWidth)
				fWidth = fRowWidth;
		}

		return new System.Drawing.SizeF(fWidth, fHeight);
	}




	/// <summary>
	/// Cleanup any resources being used
	/// </summary>
	public void Dispose(object sender, EventArgs e) {
		if (systemFont != null)
			systemFont.Dispose();

		systemFont = null;
	}
}