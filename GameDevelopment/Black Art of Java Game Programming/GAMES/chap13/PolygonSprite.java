

import java.awt.*;

/////////////////////////////////////////////////////////////////
// 
// PolygonSprite class: encapsulates polygon, position
//                      it appears, visibility
//
/////////////////////////////////////////////////////////////////

class PolygonSprite extends Sprite2D {

  protected Polygon p;

  // methods:

/////////////////////////////////////////////////////////////////
// constructor: take absolute coords
/////////////////////////////////////////////////////////////////

  public PolygonSprite(int x[], int y[], int n, Color c) {
    p = new Polygon(x,y,n);
    color = c;
    visible = true;
    fill = false;                              // don't fill polygon
    locx = locy = 0;
  }

/////////////////////////////////////////////////////////////////
// constructor: take center and offset vectors
/////////////////////////////////////////////////////////////////

  public PolygonSprite(int tx[], int ty[], int n, 
		       int centerx, int centery, Color c) {
    int x[], y[];
    x = new int[n];
    y = new int[n];
    for (int i=0; i<n; i++) {                  // compute abs coords
      x[i] = centerx + tx[i];
      y[i] = centery + ty[i];
    }
    p = new Polygon(x,y,n);
    locx = centerx;
    locy = centery;
    color = c;
    visible = true;
    fill = false;                               // don't fill polygon
  }

/////////////////////////////////////////////////////////////////
// add point to the polygon
/////////////////////////////////////////////////////////////////

  public void addPoint(int x, int y) {
    p.addPoint(x,y);
  }
  
/////////////////////////////////////////////////////////////////
// paint polygon based on variables from Sprite and Sprite2D
/////////////////////////////////////////////////////////////////

  public void paint(Graphics g) {
    if (visible) {                              
      g.setColor(color);
      if (fill)
	g.fillPolygon(p);

      else
	g.drawPolygon(p);
    }
  }

/////////////////////////////////////////////////////////////////
// no update operation
/////////////////////////////////////////////////////////////////

  public void update() {
  }
  

}




