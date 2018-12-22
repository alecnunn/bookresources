import java.applet.*;
import java.awt.*;


public class Woogie extends Applet implements Runnable {
  
  Thread animation;
  
  Graphics offscreen;
  Image image;

  static final int NUM_RECTS = 9;        // in ms
  static final int REFRESH_RATE = 100;        // in ms

  DancingRect r[];

  public void init() {
    System.out.println(">> init <<");
    setBackground(Color.black);
    initRectangles();
    image = createImage(bounds().width,bounds().height);
    offscreen = image.getGraphics();
  }

  public void initRectangles() {

    // allocate dancing rectangles

    r = new DancingRect[NUM_RECTS];
    r[0] = new DancingRect(0,0,90,90,Color.yellow);
    r[1] = new BoogieRect(250,0,40,190,Color.yellow);
    r[2] = new WaltzRect(200,55,60,135,Color.yellow);
    r[3] = new BoogieRect(80,200,220,90,Color.blue);
    r[4] = new WaltzRect(100,10,90,80,Color.blue);
    r[5] = new BoogieRect(80,100,110,90,Color.lightGray);
    r[6] = new WaltzRect(200,0,45,45,Color.red);
    r[7] = new WaltzRect(0,100,70,200,Color.red);
    r[8] = new BoogieRect(200,55,60,135,Color.magenta);

  }

  public void start() {

    System.out.println(">> start <<");
    animation = new Thread(this);
     if (animation != null) {
       animation.start();
     }
  }

  // update each rectangle's position.
  // DYNAMIC METHOD BINDING OCCURS HERE!
  public void updateRectangles() {
    for (int i=0; i<NUM_RECTS; i++) {
      r[i].danceStep();      // each rectangles dance step

    }
  }



  // override update so it doesn't erase screen
  public void update(Graphics g) {
    paint(g);
  }

  public void paint(Graphics g) {

    offscreen.setColor(Color.black);
    offscreen.fillRect(0,0,300,300);  // clear buffer

    for (int i=0; i<NUM_RECTS; i++) {
      r[i].paint(offscreen);          // paint each rectangle
    }

    g.drawImage(image,0,0,this);
  }

  public void run() {
    while (true) {
      repaint();
      updateRectangles();
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
/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

class DancingRect {
  
  int locx, locy;      // (locx,locyy) are coordinates of upper 
                       //    left corner of rectangle
  int width, height;   // width and height of rectangle
  Color myColor;         // color of rectangle


/////////////////////////////////////////////////////////////////

  public DancingRect(int x,int y,int w,int h,Color c) {
    locx = x;
    locy = y;
    width = w;
    height = h;
    myColor = c;
  }

  public void danceStep() {
   
    // does nothing

  }

  public void paint(Graphics g) {
    g.setColor(myColor);
    g.fillRect(locx,locy,width,height);
  }

}


/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////

class BoogieRect extends DancingRect {
  
  // BoogieRect inherits all instance variables and
  // methods from DancingRect

  static final byte UP = 0;   // direction of motion
  static final byte DOWN = 1;
  static final byte LEFT = 2;
  static final byte RIGHT = 3;

  byte state;                 // state of rectangle

  int max_x;                  // max x value
  int min_x;                  // min x value
  int max_y;                  // max y value
  int min_y;                  // min y value

/////////////////////////////////////////////////////////////////

  public BoogieRect(int x,int y,int w,int h,Color c) {
    super(x,y,w,h,c);         // call superclass constructor
    
    max_x = locx + 13;
    min_x = locx - 13;
    max_y = locy + 13;
    min_y = locy - 13;
 
 }


  // override danceStep()
  public void danceStep() {
    switch (state) {
    case DOWN:
      locy += 2; 
      if (locy >= max_y) {
	state = UP;
      }
      break;
    case UP:
      locy -= 2;
      if (locy <= min_y) {
	state = RIGHT;
      }
      break;
    case RIGHT:
      locx += 2;
      if (locx >= max_x) {
	state = LEFT;
      }
      break;
    case LEFT:
      locx -= 2;   
      if (locx <= min_x) {
	state = DOWN;
      }
      break;
      
    }


  }

}

/////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////
// WaltzRect also inherits from DancingRect

class WaltzRect extends DancingRect {
  byte state;
  static final byte SE = 0; // going southeast
  static final byte NE = 1; // going northeast
  static final byte W = 2;  // going west

  int bottom_x;            // the x coordinate of
                        //    bottom pt of the waltz
  int right_x;          // the x coordinate of
                        //    right pt of the waltz
  int left_x;           // the x coordinate of
                        //    left pt of the waltz

/////////////////////////////////////////////////////////////////

 public WaltzRect(int x,int y,int w,int h,Color c) {
    super(x,y,w,h,c);         // call superclass constructor
    bottom_x = locx + 17;
    right_x = bottom_x + 17;
    left_x = locx;
  }


  // override danceStep()
  public void danceStep() {
    switch (state) {
    case SE:
      locx++;
      locy++; 
      if (locx == bottom_x) {
	state = NE;
      }
      break;
    case NE:
      locx++;
      locy--;
      if (locx == right_x) {
	state = W;
      }
      break;
    case W:
      locx-- ;
      if (locx == left_x) {
	state = SE;
      }
      break;
    }

  }

}






























































