import java.applet.*;
import java.awt.*;

public class Broadway extends Applet implements Runnable {
  
  Thread animation;
  int locx,locy;          // location of rectangle
  int width, height;      // dimensions of rectangle
  static final byte UP = 0; // direction of motion
  static final byte DOWN = 1;
  static final byte LEFT = 2;
  static final byte RIGHT = 3;
  
  byte state;              // state the rect is in

  static final int REFRESH_RATE = 100;        // in ms

  public void init() {

    System.out.println(">> init <<");
    setBackground(Color.black);
    locx = 80;
    locy = 100;
    width = 110;
    height = 90;
    state = UP;
  }


  public void start() {

    System.out.println(">> start <<");
    animation = new Thread(this);
     if (animation != null) {
       animation.start();
     }
  }

  public void paint(Graphics g) {
    System.out.println(">> paint <<");

    g.setColor(Color.yellow);
    g.fillRect(0,0,90,90);
    g.fillRect(250,0,40,190);
    g.fillRect(80,110,100,20); // hidden rectangle

    g.setColor(Color.blue);
    g.fillRect(80,200,220,90);
    g.fillRect(100,10,90,80);

    g.setColor(Color.lightGray);
    g.fillRect(locx,locy,width,height);

    g.setColor(Color.red);
    g.fillRect(200,0,45,45);
    g.fillRect(0,100,70,200);
   
    g.setColor(Color.magenta);
    g.fillRect(200,55,60,135);
  }


  void updateRectangle() {
    switch (state) {
    case DOWN:
      locy += 2; 
      if (locy >= 110) {
	state = UP;
      }
      break;
    case UP:
      locy -= 2;
      if (locy <= 90) {
	state = RIGHT;
      }
      break;
    case RIGHT:
      locx += 2;
      if (locx >= 90) {
	state = LEFT;
      }
      break;
    case LEFT:
      locx -= 2;   
      if (locx <= 70) {
	state = DOWN;
      }
      break;
      
    }


  }

  public void run() {
    while (true) {
      repaint();
      updateRectangle();
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



