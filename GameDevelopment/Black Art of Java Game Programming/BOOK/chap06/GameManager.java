
import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class GameManager extends Applet 
implements Runnable {

  // animation variables
  static final int REFRESH_RATE = 80; // in ms
  Thread animation;
  Graphics offscreen;
  Image image;

  // game variables used when playing
  static final int UFO_VALUE = 130;   // 130 points
  static final int MAX_LANDED = 5;    // at most 5 aliens 
                                      //   can land

  static final int MAX_LEVEL = 9;     //
  static final int MAX_ENERGY = 113;  //

  private int score;
  private int numLanded;              // num of aliens landed

  Image ufoImages[] =  new Image[6];    // 6 ufo Images	
  Image attackImages[] =  new Image[6]; // 6 attack Images	
  Image explodeImages[] =  new Image[4];// 4 explode Images	
  Image gunImage;                       // gun image
  GunManager gm;                      // refers to gun manager
  UFOManager um;                      // refers to ufo manager

  // state of game
  private boolean playing;            // if game playing
  private int screen;                 // which screen to show
  static final int INTRO = 0;         // intro screen
  static final int GAME_OVER = 1;     // game over screen
  
  AudioClip expsound;                 // explosion sound

  // fonts
  Font smallFont = new Font("Helvetica",Font.BOLD,12);
  Font mediumFont = new Font("Helvetica",Font.BOLD,14);
  Font bigFont = new Font("Helvetica",Font.BOLD,18);

  FontMetrics fm;                      // use to center string

  int width, height;                   // applet dimensions

  // strings
  String scoreString = "Score: ";
  String ufoLandedString = "UFOs Landed: ";
  String gameOverString =     "  GAME OVER  ";
  String clickString = "Shift-Click to Continue";
  String alienLandingString = "Alien Landing";
  int stringWidth;
  String introString[] = new String[8];
  

  public void init() {
    showStatus("Loading Images -- WAIT!");
    setBackground(Color.black);        // applet background
    width = bounds().width;            // set applet dimensions
    height = bounds().height;
    loadImages();

    try {
      expsound = getAudioClip(getCodeBase(),"Explosion.au");
    }
    catch (Exception e) { }
    um = new UFOManager(2,MAX_LEVEL,width,height,ufoImages,
			attackImages,explodeImages,
			this,expsound);
    gm = new GunManager(MAX_ENERGY,5,width,height,gunImage,
			um.getUFO(),
			this);
    um.initialize(gm);              // initialize gun parameters

    playing = false;                   // not playing
    screen = INTRO;                    // show intro screen

    image = createImage(width,height); // make offscreen buffer
    offscreen = image.getGraphics();
    offscreen.setFont(bigFont);        // font for intro
    fm = offscreen.getFontMetrics(bigFont); // font metrics
    stringWidth = fm.stringWidth(alienLandingString);

    introString[0] = "You are Humanity's last hope!";
    introString[1] = "Destroy the green Alien Landers";
    introString[2] = "by using the Mouse to Move";
    introString[3] = "your Missile Launcher. Click";
    introString[4] = "to Fire Missile. If 5 Aliens";
    introString[5] = "Land, or Energy Runs Out,";
    introString[6] = "Humans will be Eaten Alive!";
    introString[7] = "Click to start Game";

 }
 
  // load all images used in game
  public void loadImages() {

    MediaTracker t = new MediaTracker(this);
    gunImage = getImage(getCodeBase(),"image/gun.gif");
    t.addImage(gunImage,0);
    for (int i=0; i<ufoImages.length; i++) {
      ufoImages[i] = getImage(getCodeBase(),
			      "image/ufo" + i + ".gif");
      t.addImage(ufoImages[i],0);
      attackImages[i] = getImage(getCodeBase(),
			      "image/attack" + i + ".gif");
      t.addImage(attackImages[i],0);
    }
    for (int i=0; i<explodeImages.length; i++) {
      explodeImages[i] = getImage(getCodeBase(),
			      "image/explode" + i + ".gif");
      t.addImage(explodeImages[i],0);
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

  }

  // initialize params for new game
  public void newGame() {

    score = 0;                         // no score
    numLanded = 0;                     // no aliens landed
    gm.newGame();                      // call newGame in
    um.newGame();                      //   manager classes
    offscreen.setFont(smallFont);      // set font in game
  }


  // handle mouse events //

  public boolean mouseMove(Event e,int x,int y) {		
    if (playing) {
      gm.moveGun(x);
    }
    return true;
  }
  public boolean mouseDrag(Event e,int x,int y) {		
    if (playing) {
      gm.moveGun(x);
    }
    return true;
  }

  public boolean mouseDown(Event e,int x,int y) {		
    if (playing) {
      gm.fireMissile(x);
    }
    else if (screen == INTRO) {        // start game for mouse
                                       //   down on intro screen
      playing = true;                   
      newGame();
    }
    else if (screen == GAME_OVER) {    // else go back to intro
      if (e.shiftDown()) {             //   if shift-click
	screen = INTRO;
      }
    }

    return true;
  }


  // start the Video Game Loop
  public void start() {
    showStatus("Starting Game!");
    animation = new Thread(this);
     if (animation != null) {
       animation.start();
     }
  }

  // update managers. only update gun if playing
  public void updateManagers() {
    if (playing) {
      gm.update();
    }
    um.update();
  }



  // override update so it doesn't erase screen
  public void update(Graphics g) {
    paint(g);
  }

  // paint the applet depending on mode of game
  public void paint(Graphics g) {
    if (playing) {
      offscreen.setColor(Color.black);
      offscreen.fillRect(0,0,width,height);  // clear buffer

      // draw status info
      offscreen.setColor(Color.cyan);
      offscreen.drawString(scoreString+score,width - 113,13);
      offscreen.drawString(ufoLandedString+numLanded,
			   width - 113,27);
      
      // tell UFOManager and GunManager to paint
      um.paint(offscreen);
      gm.paint(offscreen);                  

  
      g.drawImage(image,0,0,this);
    }
    else if (screen == INTRO) {
      offscreen.setColor(Color.black);
      offscreen.fillRect(0,0,width,height);  // clear buffer

      offscreen.setFont(smallFont);
      offscreen.setColor(Color.cyan);
      offscreen.drawString(scoreString+score,width - 113,13);
      offscreen.drawString(ufoLandedString+numLanded,
			   width - 113,27);
      um.paint(offscreen);

      offscreen.setFont(bigFont);

      offscreen.setColor(Color.green);
      offscreen.drawString(alienLandingString,
		   (width-stringWidth)/2,
		   height/6);

      offscreen.setColor(Color.magenta);
      offscreen.setFont(mediumFont);

      // draw instructions
      for (int i=0; i<introString.length-1; i++) {
	offscreen.drawString(introString[i],13,(3+i)*height/12);
      }
      offscreen.setColor(Color.green);
      offscreen.drawString(introString[7],
		   (width-stringWidth)/2,
		   height*11/12);

      g.drawImage(image,0,0,this);
      
    }

    else if (screen == GAME_OVER) {
      offscreen.setColor(Color.black);
			 
      offscreen.fillRect(0,0,width,height);  // clear buffer

      // draw status info
      offscreen.setFont(smallFont);
      offscreen.setColor(Color.cyan);
      offscreen.drawString(scoreString+score,width - 113,13);
      offscreen.drawString(ufoLandedString+numLanded,
			   width - 113,27);

      um.paint(offscreen);
      gm.paint(offscreen);                  

      offscreen.setFont(bigFont);

      offscreen.setColor(Color.red);
      offscreen.drawString(gameOverString,
		   (width-stringWidth)/2,
		   height/2);
      offscreen.setFont(mediumFont);
      offscreen.setColor(Color.green);
      offscreen.drawString(clickString,
		   (width-stringWidth-17)/2,
		   height*11/12);
      
      g.drawImage(image,0,0,this);


    }
  }

  // the Video Game Loop
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


  // stop animation
  public void stop() {

    showStatus("Game Stopped");
    if (animation != null) {
      animation.stop();
      animation = null;
    }
  }

  // increase score
  public void incrementScore() {
    score += UFO_VALUE;
  }

  // count number of ufo's landed
  public void alienLanded() {
    numLanded++;
    if (numLanded == MAX_LANDED) {
      gameOver();
    }
  }

  // handle game over
  public void gameOver() {
    if (playing) {
      playing = false;
      screen = GAME_OVER;
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
