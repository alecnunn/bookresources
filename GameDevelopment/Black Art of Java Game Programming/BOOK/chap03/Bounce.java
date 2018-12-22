import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class Bounce extends Applet implements Runnable {
  
  Thread animation;
  
  Graphics offscreen;
  Image image;


  static final int NUM_SPRITES = 3;       
  static final int REFRESH_RATE = 80; // in ms

  Sprite sprites[];                    // sprite array
  int width, height;                   // applet dimensions

  public void init() {
    System.out.println(">> init <<");
    setBackground(Color.black);        // applet background
    width = bounds().width;            // set applet dimensions
    height = bounds().height;
    initSprites();
    image = createImage(width,height); // make offscreen buffer
    offscreen = image.getGraphics();
 }

  public void initSprites() {

    sprites = new Sprite[NUM_SPRITES]; // init sprite array
    
    // define sprite for border
    sprites[0] = new RectSprite(0,0,width-1,height-1,Color.green);

    sprites[1] = new BouncingRect(0,0,30,30,Color.yellow,
				  width-1,height-1);

    sprites[2] = new BouncingRect(17,17,13,13,Color.red,
				  width-1,height-1);

    ((Moveable)sprites[1]).setVelocity(4,3);
    ((Moveable)sprites[2]).setVelocity(1,2);
    ((Sprite2D)sprites[2]).setFill(true);  // fill this sprite

  }

  public void start() {

    System.out.println(">> start <<");
    animation = new Thread(this);
     if (animation != null) {
       animation.start();
     }
  }

  // CALL EACH SPRITE'S update() METHOD
  // DYNAMIC METHOD BINDING OCCURS HERE!
  public void updateSprites() {
    for (int i=0; i<sprites.length; i++) {
      sprites[i].update();          // call each sprite's
                                    //    update() method
    }
  }



  // override update so it doesn't erase screen
  public void update(Graphics g) {
    paint(g);
  }


  //
  public void paint(Graphics g) {

    offscreen.setColor(Color.black);
    offscreen.fillRect(0,0,width,height);  // clear buffer

    for (int i=0; i<sprites.length; i++) {
      sprites[i].paint(offscreen);     // paint each rectangle
    }

    g.drawImage(image,0,0,this);
  }

  public void run() {
    while (true) {
      repaint();
      updateSprites();
      try {
	Thread.sleep (REFRESH_RATE);
      } catch (Exception exc) { };
    }
  }



  public void stop() {

    System.out.println(">> stop <<");
    if (animation != null) {
      animation.stop();
      animation = null;
    }
  }

}

/////////////////////////////////////////////////////////////////
class BitmapSprite extends Sprite {
  protected int locx;   
  protected int locy;

  // image dimensions
  protected int width,height;

  protected Image image;                   // the bitmap
  protected Applet applet;                 // the parent applet

  public BitmapSprite(int x,int y,Image i,Applet a) {
    locx = x;
    locy = y;
    image = i;
    applet = a;
    restore();
  }

  public void setSize(int w,int h) {
    width = w;
    height = h;
  }
  
  public void update() {
    
    // do nothing

  }

  public void paint(Graphics g) {
    if (visible) {
      g.drawImage(image,locx,locy,applet);
    }
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
class BouncingRect extends RectSprite implements Moveable {
  
  // the coords at which
  //  the rectangle bounces  
  protected int max_width;
  protected int max_height;    

  // sprite velocity. used to implement Moveable interface
  protected int vx;
  protected int vy;

  public BouncingRect(int x,int y,int w,int h,Color c,
		      int max_w,int max_h) {
    super(x,y,w,h,c);
    max_width = max_w;
    max_height = max_h;
  }

  public void setPosition(int x,int y) {
    locx = x;
    locy = y;
  }

  public void setVelocity(int x,int y) {
    vx = x;
    vy = y;
  }

  // update position according to velocity
  public void updatePosition() {
    locx += vx;
    locy += vy;
  }

  // move and bounce rectangle if it hits borders
  public void update() {

    
    // flip x velocity if it hits left or right bound
    if ((locx + width > max_width) ||
	locx < 0) {
      vx = -vx;
      }

    // flip y velocity if it hits top or bottom bound
    if ((locy + height > max_height) ||
	locy < 0) {
      vy = -vy;
      }
    updatePosition();
  }

}

/////////////////////////////////////////////////////////////////
class BouncingBitmap extends BitmapSprite implements Moveable {
  
  // the coords at which
  //  the bitmap bounces  
  protected int max_width;
  protected int max_height;    

  // sprite velocity. used to implement Moveable interface
  protected int vx;
  protected int vy;

  public BouncingBitmap(int x,int y,Image i,Applet a, 
		      int max_w,int max_h) {
    super(x,y,i,a);
    max_width = max_w;
    max_height = max_h;
  }

  public void setPosition(int x,int y) {
    locx = x;
    locy = y;
  }

  public void setVelocity(int x,int y) {
    vx = x;
    vy = y;
  }

  // update position according to velocity
  public void updatePosition() {
    locx += vx;
    locy += vy;
  }

  // move and bounce rectangle if it hits borders
  public void update() {

    
    // flip x velocity if it hits left or right bound
    if ((locx + width > max_width) ||
	locx < 0) {
      vx = -vx;
      }

    // flip y velocity if it hits top or bottom bound
    if ((locy + height > max_height) ||
	locy < 0) {
      vy = -vy;
      }
    updatePosition();
  }

}
