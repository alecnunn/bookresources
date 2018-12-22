import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class DragSize extends Applet {
  Font courierFont;
  String testString = "Drag and Resize the Rectangle!";
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

    // paint the rectangle
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

  // Resize rectangle:
  // if Right arrow key, grow the rectangle
  // if Left arrow key, shrink the rectangle

  public boolean keyDown(Event e,int key) {
    switch (key) {
    case Event.RIGHT:
      r.grow();
      repaint();
      break;
    case Event.LEFT:
      r.shrink();
      repaint();
      break;
    default:
      break;
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

  // increase size of rectangle. Note there is no 
  //    maximum size!
  public void grow() {
    width++;
    height++;
  }

  // shrink the rectangle
  public void shrink() {
    if (width >  0) {
      width--;
    }
    if (height > 0) {
      height--;
    }
  }

}

/////////////////////////////////////////////////////////////////
class TextSprite extends Sprite {
  protected String string;
  protected Font font;
  protected int locx;
  protected int locy;
  protected Color color;

  public TextSprite(int x,int y,String s,Font f,Color c) {
    string = s;
    font = f;
    locx = x;
    locy = y;
    color = c;
  }

  public void update() {
  }

  public void paint(Graphics g) {
    g.setFont(font);
    g.setColor(color);
    g.drawString(string,locx,locy);
  }
}
