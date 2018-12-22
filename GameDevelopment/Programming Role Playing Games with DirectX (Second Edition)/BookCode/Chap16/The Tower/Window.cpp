#include "Core_Global.h"
#include "Window.h"

cWindow::cWindow()
{
  // Clear class data
  m_Graphics = NULL;
  m_Font     = NULL;
  m_Text     = NULL;
  m_TextColor = 0xFFFFFFFF;
}

cWindow::~cWindow()
{
  Free();  // Free class data
}

BOOL cWindow::Create(cGraphics *Graphics, cFont *Font)
{
  Free();  // Free previous class data

  // Error checking
  if((m_Graphics = Graphics) == NULL || (m_Font = Font) == NULL)
    return FALSE;

  // Create new vertex buffer
  m_WindowVB.Create(m_Graphics, 11, WINDOWFVF, sizeof(sVertex));

  return TRUE; // Return success
}

BOOL cWindow::Free()
{
  m_WindowVB.Free();  // Free vertex buffer

  delete [] m_Text;   // Delete text buffer
  m_Text = NULL;

  return TRUE;
}

BOOL cWindow::SetText(char *Text, D3DCOLOR TextColor)
{
  // Delete previous text
  delete [] m_Text;
  m_Text = NULL;

  m_Text = strdup(Text);       // Copy text string
  m_TextColor = TextColor;     // Store text color

  return TRUE;
}

BOOL cWindow::Move(long XPos, long YPos,                      \
                   long Width, long Height,                   \
                   long TargetX, long TargetY,                \
                   D3DCOLOR BackColor)
{
  sVertex Verts[11];
  long i;

  // Save the coordinates and calculate height if needed
  m_XPos  = XPos;
  m_YPos  = YPos;
  m_Width = Width;
  if(!(m_Height = Height)) {
    RECT Rect;
    Rect.left   = XPos;
    Rect.top    = 0;
    Rect.right  = XPos + Width - 12;
    Rect.bottom = 1;

    m_Height = m_Font->GetFontCOM()->DrawText(m_Text, -1,     \
                &Rect, DT_CALCRECT | DT_WORDBREAK,            \
                0xFFFFFFFF) + 12;
  }

  // Clear the vertex data
  for(i=0;i<11;i++) {
    Verts[i].z = 0.0f;
    Verts[i].rhw = 1.0f;
    Verts[i].Diffuse = 0xFFFFFFFF;
  }

  // Setup the white outline
  Verts[0].x = (float)m_XPos;
  Verts[0].y = (float)m_YPos;
  Verts[1].x = (float)(m_XPos+m_Width);
  Verts[1].y = (float)m_YPos;
  Verts[2].x = (float)m_XPos;
  Verts[2].y = (float)(m_YPos+m_Height);
  Verts[3].x = (float)(m_XPos+m_Width);
  Verts[3].y = (float)(m_YPos+m_Height);

  // Setup the text window
  Verts[4].x = (float)m_XPos+2.0f;
  Verts[4].y = (float)m_YPos+2.0f;
  Verts[4].Diffuse = BackColor;
  Verts[5].x = (float)(m_XPos+m_Width)-2.0f;
  Verts[5].y = (float)m_YPos+2.0f;
  Verts[5].Diffuse = BackColor;
  Verts[6].x = (float)m_XPos+2.0f;
  Verts[6].y = (float)(m_YPos+m_Height)-2.0f;
  Verts[6].Diffuse = BackColor;
  Verts[7].x = (float)(m_XPos+m_Width)-2.0f;
  Verts[7].y = (float)(m_YPos+m_Height)-2.0f;
  Verts[7].Diffuse = BackColor;

  // Setup the target pointer
  if(TargetX != -1 && TargetY != -1) {
    m_DrawTarget = TRUE;

    if(TargetY < m_YPos) {
      Verts[8].x  = (float)TargetX;
      Verts[8].y  = (float)TargetY;
      Verts[9].x  = (float)(m_XPos+m_Width/2+10);
      Verts[9].y  = (float)m_YPos;
      Verts[10].x = (float)(m_XPos+m_Width/2-10);
      Verts[10].y = (float)m_YPos;
    } else {
      Verts[8].x  = (float)(m_XPos+m_Width/2-10);
      Verts[8].y  = (float)(m_YPos+m_Height);
      Verts[9].x  = (float)(m_XPos+m_Width/2+10);
      Verts[9].y  = (float)(m_YPos+m_Height);
      Verts[10].x = (float)TargetX;
      Verts[10].y = (float)TargetY;
    }
  } else {
    m_DrawTarget = FALSE;
  }

  m_WindowVB.Set(0,11,&Verts);  // Set the vertices
  
  return TRUE;
}

long cWindow::GetHeight()
{
  return m_Height;
}

BOOL cWindow::Render(char *Text, D3DCOLOR Color)
{
  // Error checking
  if(m_Graphics == NULL || m_Font == NULL)
    return FALSE;

  // Set rendering states
  m_Graphics->SetTexture(0, NULL);
  m_Graphics->EnableZBuffer(FALSE);

  // Draw window
  m_WindowVB.Render(0,2,D3DPT_TRIANGLESTRIP);
  m_WindowVB.Render(4,2,D3DPT_TRIANGLESTRIP);

  // Draw target pointer if needed
  if(m_DrawTarget)
    m_WindowVB.Render(8,1,D3DPT_TRIANGLELIST);

  // Only draw text if height > 12
  if(m_Height > 12) {
    // Draw the text
    if(Text == NULL)
      m_Font->Print(m_Text, m_XPos+6, m_YPos+6,               \
                            m_Width-12,m_Height-12,           \
                            m_TextColor, DT_WORDBREAK);
    else
      m_Font->Print(Text, m_XPos+6, m_YPos+6,                 \
                          m_Width-12,m_Height-12,             \
                          Color, DT_WORDBREAK);
  }
  
  return TRUE;
}
