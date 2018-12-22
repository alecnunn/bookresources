import java.applet.*;
import java.awt.*;
import java.util.*;

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

  // customizable parameters
  int startLevel;
  int energyDec;
  boolean sound;


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
  String alienLandingString = "Alien Landing";
  int stringWidth;
  String introString[] = new String[7];
  
  GameFrame f;

  public void init() {
    setBackground(Color.black);        // applet background
    width = 240;
    height = 270;
    resize(width,height);
    startLevel = 2;
    energyDec = 5;
    sound = false;

    f = new GameFrame(this,width,height);

    showStatus("Loading Images -- WAIT!");
    loadImages();

    try {
      expsound = getAudioClip(getCodeBase(),"Explosion.au");
    }
    catch (Exception e) { }
    um = new UFOManager(startLevel,MAX_LEVEL,width,height,ufoImages,
			attackImages,explodeImages,
			this,expsound);
    gm = new GunManager(MAX_ENERGY,energyDec,width,height,gunImage,
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

    System.out.println("loading Images");

    // wait for all images to finish loading //
    try {
      t.waitForAll();               
    }  catch (InterruptedException e) {
      return;
    }

    // check for errors //
    if (t.isErrorAny()) {
      System.out.println("error in loading");
    }
    else if (t.checkAll()) {
      System.out.println("Images successfully loaded");
    }

  }

  // initialize params for new game
  public void newGame() {
    playing = true;                    // playing game
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
  
    else if (screen == GAME_OVER) {    // else go back to intro
      if (e.shiftDown()) {             //   if shift-click
	screen = INTRO;
      }
    }

    return true;
  }
    



  // start the Video Game Loop
  public void start() {
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
      for (int i=0; i<introString.length; i++) {
	offscreen.drawString(introString[i],13,(3+i)*height/12);
      }
      offscreen.setColor(Color.green);
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
      g.drawImage(image,0,0,this);


    }
  }

  // stop animation
  public void stop() {

    System.out.println(">> stop <<");
    if (animation != null) {
      animation.stop();
      animation = null;
    }
  }

  // free up system resources
  public void destroy() {
    offscreen.dispose();
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
      f.gameOver();     // restore menu items
    }
  }

  // CUSTOMIZE MANAGERS!
  public void setOptions(int startLevel,
		    int energyDec,boolean sound) {

    if (startLevel >= 1 && startLevel < MAX_LEVEL) {
      this.startLevel = startLevel;
      um.setStartLevel(startLevel);   // set startLevel
    }
    if (energyDec >= 0 && energyDec <= MAX_ENERGY) {
      this.energyDec = energyDec;
      gm.setEnergyDec(energyDec);     // set energy lost
    }
    this.sound = sound;
    um.setSound(sound);             // set sound
    
  }  

}

class GameFrame extends Frame {
  protected Panel p;
  protected MenuItem newItem,abortItem;
  protected GameManager gm;
  protected int width,height;
  protected OptionsDialog d;
  
  public GameFrame(Applet app,int width,int height) {

    super("Alien Landing");

    this.width = width;
    this.height = height;
    gm = (GameManager)app;

    resize(width+13,height+65);
    setResizable(false);
    MenuBar menubar = new MenuBar();
    Menu m1 = new Menu("Game");
    newItem = new MenuItem("New Game");
    m1.add(newItem);
    abortItem = new MenuItem("Abort Game");
    abortItem.disable();
    m1.add(abortItem);
    m1.add(new MenuItem("Exit"));
    Menu m2 = new Menu("Options");
    m2.add(new MenuItem("Change Options..."));   
    menubar.add(m1);
    menubar.add(m2);
    setMenuBar(menubar);
    p = new Panel();
    p.setLayout(new FlowLayout(FlowLayout.CENTER));
    p.add(app);
    setLayout(new BorderLayout());
    add("Center",p);

    setCursor(Frame.CROSSHAIR_CURSOR);
  
    show();
    
  }

  public void gameOver() {
    abortItem.disable();
    newItem.enable();
  }	


  // handle actions
  public boolean action(Event e,Object o) {
    if (e.target instanceof MenuItem) {
      String s = (String)o;
      if (e.target == newItem) {
	gm.newGame();
	newItem.disable();
	abortItem.enable();
      }
      else if (e.target == abortItem) {
	gm.gameOver();
      }

      else if (s.equals("Exit")) {
	hide();
	gm.stop();
	gm.destroy();
	dispose();
      }
      else if (s.equals("Change Options...")) {
	d = new OptionsDialog(this,gm);
	
      }

      return true;
    }
    else return false;
  }

}


class OptionsDialog extends Dialog {
  Label l[] = new Label[3];
  TextField t[] = new TextField[2];
  Button b[] = new Button[2];
  CheckboxGroup cg = new CheckboxGroup();
  Checkbox c[] = new Checkbox[2];
  Panel p = new Panel();
  GameManager gm;

  public OptionsDialog(Frame parent,GameManager gm) {
    super(parent,"Alien Landing Options",true);
    this.gm = gm;
    setLayout(new GridLayout(4,2,13,13));
    l[0] = new Label("Starting Level",Label.LEFT);
    l[1] = new Label("Energy Decrement",Label.LEFT);
    l[2] = new Label("Sound",Label.LEFT);
    t[0] = new TextField(String.valueOf(gm.startLevel),3);
    t[1] = new TextField(String.valueOf(gm.energyDec),3);
    c[0] = new Checkbox("On",cg,gm.sound);
    c[1] = new Checkbox("Off",cg,!gm.sound);
    p.setLayout(new FlowLayout(FlowLayout.CENTER,3,3));
    p.add(c[0]);
    p.add(c[1]);
    b[0] = new Button("OK");
    b[1] = new Button("Cancel");
    add(l[0]);
    add(t[0]);
    add(l[1]);
    add(t[1]);
    add(l[2]);
    add(p);
    add(b[0]);
    add(b[1]);

    pack();
    show();
  }


  // handle actions
  public boolean action(Event e,Object o) {
    if (e.target instanceof Button) {
      String str = (String)o;
       // if user presses OK
      if (str.equals(b[0].getLabel())) {
	parseDialog();
      }
      // else user pressed cancel, so
      // don't do anything

      hide();
      dispose();
      return true;
    }
    else return false;
  }

  protected void parseDialog() {
    int start = -1,energy = -1;
    boolean sound;
    try {
      start = Integer.parseInt(t[0].getText());
    }
    catch (Exception exc) {
    }

    try {
      energy = Integer.parseInt(t[1].getText());
    }
    catch (Exception exc) {
    }

    sound = c[0].getState();
    gm.setOptions(start,energy,sound);
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
