
// Sprite class
//
// Sprites are objects which can be displayed
//

import java.awt.*;

/////////////////////////////////////////////////////////////////
abstract class Sprite {
  protected boolean visible;           // is sprite visible
  protected boolean active;            // is sprite updatable

  // abstract methods:
  abstract void paint (Graphics g);
  abstract void update();

  // accessor methods:
  public boolean isVisible() {
    return visible;
  }

  public void setVisible(boolean b) {
    visible = b;
  }

  public boolean isActive() {
    return active;
  }

  public void setActive(boolean b) {
    active = b;
  }

  // suspend the sprite
  public void suspend() {
    setVisible(false);
    setActive(false);
  }

  // restore the sprite
  public void restore() {
    setVisible(true);
    setActive(true);
  }

}


abstract class Sprite2D extends Sprite {

  protected int locx;   
  protected int locy;

  Color color;
  boolean fill; 

  public boolean getFill() {
    return fill;
  }

  public void setFill(boolean b) {
    fill = b;
  }

  public void setColor(Color c) {
    color = c;
  }

  public Color getColor() {
    return color;
  }


}

