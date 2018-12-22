
/////////////////////////////////////////////////////////////////
// demo of BitmapLoops and user interaction
/////////////////////////////////////////////////////////////////

import java.applet.*;
import java.awt.*;


public class GameManager extends Applet implements Runnable {
  
  Thread animation;
  
  Graphics offscreen;
  Image image;

  static final int REFRESH_RATE = 80; // in ms

  Image ufoImages[] =  new Image[6]; // 6 ufo Images	
  Image gunImage;                    // gun image
  GunManager gm;                   
  UFOManager um;

  int width, height;                   // applet dimensions

  public void init() {
    showStatus("Loading Images -- WAIT!");
    setBackground(Color.black);        // applet background
    width = bounds().width;            // set applet dimensions
    height = bounds().height;
    loadImages();
    um = new UFOManager(width,height,ufoImages,this);
    gm = new GunManager(width,height,gunImage,
			um.getUFO(),
			this);
    um.initialize(gm);                 // initialize gun parameters
    image = createImage(width,height); // make offscreen buffer
    offscreen = image.getGraphics();
 }


  
  public void loadImages() {


    MediaTracker t = new MediaTracker(this);
    gunImage = getImage(getCodeBase(),"image/gun.gif");
    t.addImage(gunImage,0);
    for (int i=0; i<6; i++) {
      ufoImages[i] = getImage(getCodeBase(),
			      "image/ufo" + i + ".gif");
      t.addImage(ufoImages[i],0);
    }

    // wait for all images to finish loading //
    try {
      t.waitForAll();               
    }  catch (InterruptedException e) {
    }

    // check for errors //
    if (t.isErrorAny()) {
      showStatus("Error Loading Images");
    }
    else if (t.checkAll()) {
      showStatus("Images successfully loaded");
    }
    // initialize the BitmapLoop


  }

  public boolean mouseMove(Event e,int x,int y) {		
    gm.moveGun(x);
    return true;
  }
  public boolean mouseDrag(Event e,int x,int y) {		
    gm.moveGun(x);
    return true;
  }
 public boolean mouseDown(Event e,int x,int y) {		
    gm.fireMissile(x);
    return true;
  }



  public void start() {

    showStatus("Starting Game!");
    animation = new Thread(this);
     if (animation != null) {
       animation.start();
     }
  }


  public void updateManagers() {
    gm.update();
    um.update();
  }



  // override update so it doesn't erase screen
  public void update(Graphics g) {
    paint(g);
  }


  //
  public void paint(Graphics g) {

    offscreen.setColor(Color.black);
    offscreen.fillRect(0,0,width,height);  // clear buffer

    gm.paint(offscreen);
    um.paint(offscreen);

    g.drawImage(image,0,0,this);
  }

  public void run() {
    while (true) {
      repaint();
      updateManagers();
      Thread.currentThread().yield();
      try {
	Thread.sleep (REFRESH_RATE);
      } catch (Exception exc) { };
    }
  }



  public void stop() {

    showStatus("Game Stopped");
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

  public BitmapSprite(Image i,Applet a) {
    locx = 0;
    locy = 0;
    image = i;
    applet = a;
    if (image != null) {
      width = image.getWidth(a); // get size of background
      height = image.getHeight(a);
    }
    restore();
  }


  public BitmapSprite(int x,int y,Image i,Applet a) {
    locx = x;
    locy = y;
    image = i;
    applet = a;
    if (image != null) {
      width = image.getWidth(a); // get size of background
      height = image.getHeight(a);
    }
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
  public abstract void setPosition(int x, int y);
  public abstract void setVelocity(int x, int y);
  public abstract void updatePosition();
}

    
/////////////////////////////////////////////////////////////////
class BitmapLoop extends BitmapSprite implements Moveable{
  protected Image images[];       // sequence of bitmaps
  protected int currentImage;     // the current bitmap
  protected boolean foreground;   // are there foreground images?
  protected boolean background;   // is there background image?
  // constructor. Assumes that background image is already
  // loaded. (use MediaTracker)

  public BitmapLoop(int x,int y,Image b,Image f[],Applet a) {
    super(x,y,b,a);
    if (image != null) {           // if there's a background image
      background = true;
    }
    else {
      background = false;
    }

    images = f;
    currentImage = 0;            
    if (images == null || images.length == 0) {
      foreground = false;          // nothing in images[]
    }
    else {
      foreground = true;
      if (!background) {               // if no background
	width = images[0].getWidth(a); // get size of images[0]
	height = images[0].getHeight(a);	
      }
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
      if (background) {
	g.drawImage(image,locx,locy,applet);
      }
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

  }

}
