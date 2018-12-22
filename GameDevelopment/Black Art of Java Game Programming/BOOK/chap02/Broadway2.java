import java.applet.*;
import java.awt.*;

// double buffered and clipped

public class Broadway2 extends Applet implements Runnable {
  
  Thread animation;
  int locx,locy;          // location of rectangle
  int width, height;      // dimensions of rectangle
  static final byte UP = 0; // direction of motion
  static final byte DOWN = 1;
  static final byte LEFT = 2;
  static final byte RIGHT = 3;

  Graphics offscreen;
  Image image;
  
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
    image = createImage(300,300);
    offscreen = image.getGraphics();
  }


  public void start() {

    System.out.println(">> start <<");
    animation = new Thread(this);
     if (animation != null) {
       animation.start();
     }
  }

  public void update(Graphics g) {
    g.clipRect(70,90,130,110);
    paint(g);
  }

  public void paint(Graphics g) {
//    System.out.println(">> paint <<");

    offscreen.setColor(Color.black);
    offscreen.fillRect(0,0,300,300);  // clear buffer
    offscreen.setColor(Color.yellow);
    offscreen.fillRect(0,0,90,90);
    offscreen.fillRect(250,0,40,190);
    offscreen.fillRect(80,110,100,20);

    offscreen.setColor(Color.blue);
    offscreen.fillRect(80,200,220,90);
    offscreen.fillRect(100,10,90,80);

    offscreen.setColor(Color.lightGray);
    offscreen.fillRect(locx,locy,width,height);

    offscreen.setColor(Color.red);
    offscreen.fillRect(200,0,45,45);
    offscreen.fillRect(0,100,70,200);
   
    offscreen.setColor(Color.magenta);
    offscreen.fillRect(200,55,60,135);
    g.drawImage(image,0,0,this);
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










































































