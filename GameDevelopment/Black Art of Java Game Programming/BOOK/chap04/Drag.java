import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class Drag extends Applet {
  Font courierFont;
  String testString = "Drag the Rectangle!";
  DragRect r = new DragRect(0,0,107,103,Color.red);

  public void init() {
    courierFont = new Font("Courier",Font.BOLD+Font.ITALIC,14);
  }

  public void paint(Graphics g) {
    g.setFont(courierFont);

    // center the string
    FontMetrics m = g.getFontMetrics();
    int stringWidth = m.stringWidth(testString);
    int width = (bounds().width - stringWidth )/2;
    int height = bounds().height / 2;

    // draw the string
    g.setColor(Color.green);
    g.drawString(testString,width,height);
    r.paint(g);
  }

  // if user clicks in the rectangle, make rectangle draggable

  int oldx,oldy;    // stores old mouse location
  public boolean mouseDown(Event e,int x,int y) {
    if (r.inside(x,y)) {
      oldx = x;
      oldy = y;
      r.setDraggable(true);  
    }
    return true;
  }


  // if mouseUp, rectangle is no longer draggable
  public boolean mouseUp(Event e,int x,int y) {
    r.setDraggable(false);  
    return true;
  }

  // translate the rectangle by the difference between
  //   the new mouse position and the old one

  public boolean mouseDrag(Event e,int x,int y) {
    if (r.isDraggable()) {
      r.translate(x-oldx,y-oldy);  // move rectangle
      oldx = x;                    // store old mouse position
      oldy = y;
      repaint();                   // redraw screen
    }
    return true;
  }

}



/////////////////////////////////////////////////////////////////
class RectSprite extends Sprite2D {

  protected int width, height;    // dimensions of rectangle

  
  public RectSprite(int x,int y,int w,int h,Color c) {
    locx = x;
    locy = y;
    width = w;
    height = h;
    color = c;
    fill = false;                 // default: don't fill
    restore();                    // restore the sprite
  }

  // provide implementation of abstract methods:

  public void update() {
   
    // does nothing

  }


  // check if sprite's visible before painting
  public void paint(Graphics g) {
    if (visible) {
      g.setColor(color);

      if (fill) {
	g.fillRect(locx,locy,width,height);
      }

      else {
	g.drawRect(locx,locy,width,height);
      }

    }

  }

}
  
// Moveable interface. Note that the declarations public 
// and abstract are optional. All methods specified in an
// interface are automatically public and abstract!

interface Moveable {
  public abstract void setPosition(int c, int d);
  public abstract void setVelocity(int x, int y);
  public abstract void updatePosition();
}


/////////////////////////////////////////////////////////////////
class DragRect extends RectSprite {

  protected boolean draggable;    // is rectangle draggable?

  public DragRect(int x,int y,int w,int h,Color c) {
    super(x,y,w,h,c);
    fill = true;
    draggable = false; // initially not draggable
  }

  // check if (x,y) is inside rectangle
  public boolean inside(int x,int y) {
    return (locx <= x && locy <= y &&
	    (locx + width >= x) && 
	    (locy + height >= y));
  }

  // modify draggable
  public void setDraggable(boolean b) {
    draggable = b;
  }

  public boolean isDraggable() {
    return draggable;
  }


  public void translate(int x,int y) {
    locx += x;
    locy += y;
      
  }
}





