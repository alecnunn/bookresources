#ifndef _WINDOW_H_
#define _WINDOW_H_

class cWindow
{
  private:
    typedef struct sVertex {  // Custom vertex 
      float    x, y, z;  // Coordinates in screen space
      float    rhw;      // RHW value
      D3DCOLOR Diffuse;  // Diffuse color
    } sVertex;
    #define WINDOWFVF (D3DFVF_XYZRHW | D3DFVF_DIFFUSE)

    cGraphics     *m_Graphics;  // Parent cGraphics object
    cFont         *m_Font;      // Font object
    cVertexBuffer  m_WindowVB;  // Vertex buffer for window

    char          *m_Text;        // Text to display
    D3DCOLOR       m_TextColor;   // Color to draw text 

    long           m_XPos, m_YPos;     // Window coordinates
    long           m_Width, m_Height;  // Window dimensions

    BOOL           m_DrawTarget;  // Flag to draw bubble pointer

  public:
    cWindow();   // Constructor
    ~cWindow();  // Destructor

    // Functions to create/free a text window
    BOOL Create(cGraphics *Graphics, cFont *Font);
    BOOL Free();

   // Set the text and window coordinates/dimensions/colors
    BOOL SetText(char *Text, D3DCOLOR TextColor = 0xFFFFFFFF);

    // Move the window
    BOOL Move(long XPos, long YPos, long Width,long Height=0, \
              long TargetX = -1, long TargetY = -1,           \
              D3DCOLOR BackColor = D3DCOLOR_RGBA(0,64,128,255));

    long GetHeight();  // Get window height after set

    // Render window and text to display
    BOOL Render(char *Text = NULL, D3DCOLOR Color = 0xFFFFFFFF);
};

#endif
