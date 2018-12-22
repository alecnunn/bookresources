
/////////////////////////////////////////////////////////////////
// demo of BitmapLoops and user interaction
/////////////////////////////////////////////////////////////////

import java.applet.*;
import java.awt.*;


public class UFOControl extends Applet implements Runnable {
  
  Thread animation;
  
  Graphics offscreen;
  Image image;

  static final int NUM_SPRITES = 1;       
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
    sprites = new Sprite[NUM_SPRITES];
    Image backImage;                   // background Image
    Image foreImage[] =  new Image[6]; // 6 foreground Images

    MediaTracker t = new MediaTracker(this);
    backImage = getImage(getCodeBase(),"image/back.gif");
    t.addImage(backImage,0);
    for (int i=0; i<6; i++) {
      foreImage[i] = getImage(getCodeBase(),
			      "image/fore" + i + ".gif");
      t.addImage(foreImage[i],0);
    }

    System.out.println("loading Images");

    // wait for all images to finish loading //
    try {
      t.waitForAll();               
    }  catch (InterruptedException e) {
    }

    // check for errors //
    if (t.isErrorAny()) {
      System.out.println("error");
    }
    else if (t.checkAll()) {
      System.out.println("successfully loaded");
    }
    // initialize the BitmapLoop
    sprites[0] = new BitmapLoop(13,17,backImage,foreImage,this);

  }

  
  // Move UFO depending on Arrow Keys

  public boolean keyDown(Event e,int key) {
    switch (key) {
    case Event.RIGHT:
      ((Moveable)sprites[0]).setVelocity(3,0);
      break;
    case Event.LEFT:
      ((Moveable)sprites[0]).setVelocity(-3,0);
      break;
    case Event.UP:
      ((Moveable)sprites[0]).setVelocity(0,-3);
      break;
    case Event.DOWN:
      ((Moveable)sprites[0]).setVelocity(0,3);
      break;
    default:
      break;
    }
    return true;
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
  

// Moveable interface. Note that the declarations public 
// and abstract are optional. All methods specified in an
// interface are automatically public and abstract!

interface Moveable {
  public abstract void setPosition(int c, int d);
  public abstract void setVelocity(int x, int y);
  public abstract void updatePosition();
}

    
/////////////////////////////////////////////////////////////////
class BitmapLoop extends BitmapSprite implements Moveable{
  protected Image images[];       // sequence of bitmaps
  protected int currentImage;     // the current bitmap
  protected boolean foreground;   // are there foreground images?

  // constructor. Assumes that background image is already
  // loaded. (use MediaTracker)

  public BitmapLoop(int x,int y,Image b,Image f[],Applet a) {
    super(x,y,b,a);
    width = image.getWidth(a); // get size of background
    height = image.getHeight(a);

    images = f;
    currentImage = 0;            
    if (images.length == 0) {
      foreground = false;          // nothing in images[]
    }
    else {
      foreground = true;
    }

  }    


  
  // cycle currentImage if sprite is active, and there
  //   are foreground images
  public void update() {
    if (active && foreground) {
      currentImage = (currentImage + 1) % images.length;
    }
    updatePosition();
  }
    

  public void paint(Graphics g) {
    if (visible) {
      g.drawImage(image,locx,locy,applet);
      if (foreground) {
	g.drawImage(images[currentImage],locx,locy,applet);
      }
    }
  }    

  // implement moveable interface

  
  public void setPosition(int x,int y) {
    locx = x;
    locy = y;
  }
  protected int vx;
  protected int vy;

  public void setVelocity(int x,int y) {
    vx = x;
    vy = y;
  }

  // update position according to velocity
  public void updatePosition() {
    locx += vx;
    locy += vy;
    vx = 0;
    vy = 0;
  }

}
