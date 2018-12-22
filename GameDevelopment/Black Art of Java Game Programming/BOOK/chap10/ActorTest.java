

import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class ActorTest extends Applet {
  
  Actor actors[] = new Actor[4];

  public void init() {
    int width = size().width;
    int height = size().height;
    setBackground(Color.black);	
    // define 4 Actors:
    // this changes color every 50 ms
    actors[0] = new CycleColorActor(this,0,0,
			       90,150,0,50);
    // this changes color every 100 ms
    actors[1] = new CycleColorActor(this,width-73,0,
			       90,150,0,100);
    // this changes color every 200 ms
    actors[2] = new CycleColorActor(this,0,height-73,
			       90,150,0,200);
    // this changes color every 400 ms
    actors[3] = new CycleColorActor(this,width-73,height-73,
			       90,150,0,400);

  }




  // override update so it doesn't erase screen
  public void update(Graphics g) {
    paint(g);
  }

  public void paint(Graphics g) {

    for (int i=0; i<actors.length; i++) {
      actors[i].paint(g);     // paint each rectangle
    }

  }


  
}


abstract class Actor extends Thread {
  Applet applet;
  protected boolean drawNow;
  abstract void paint(Graphics g);
}


/////////////////////////////////////////////////////////////////
class CycleColorActor extends Actor {
  protected int locx,locy;
  protected int red,green,blue;   // color
  protected int delay;

  public CycleColorActor(Applet a,int x,int y,
		   int r,int g,int b,int d) {
    // initialize inherited variables
    applet = a;
    drawNow = false;

    // initialize other variables 
    locx = x;
    locy = y;
    red = r;
    green = g;
    blue = b;
    delay = d;
    start();           // start running!
  }

  // override run()

  public void run() {
    while (true) {
      drawNow = true;
      applet.repaint();
      updateColor();
      try {
	sleep(delay);
      }
      catch (InterruptedException e) {
      }
    }
  }


  public void updateColor() {
    red = (red + 4) % 256;
    green = (green + 4) % 256;
    blue = (blue + 4) % 256;
  }

  // provide implementation of abstract methods:

  public void paint(Graphics g) {
    if (drawNow) {
      g.setColor(new Color(red,green,blue));
      g.fillRect(locx,locy,73,73);
      drawNow = false;
    }
  }
}
  






