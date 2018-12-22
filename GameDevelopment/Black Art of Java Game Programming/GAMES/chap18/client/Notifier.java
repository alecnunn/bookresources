package client;

import java.awt.*;

class Notifier extends Canvas implements Runnable {
  private Thread runner = null;
  private int xsize, ysize;

  public Notifier (int x, int y) {
    xsize = x;
    ysize = y;
    runner = new Thread(this);
    runner.setPriority(runner.MAX_PRIORITY);
    runner.start();
  }
  
  public Dimension minimumSize () {
    return new Dimension(xsize, ysize);
  }

  public Dimension preferredSize () {
    return new Dimension(xsize, ysize);
  }

  public void dispose () {
    if (runner != null)
      runner.stop();
    runner = null;
  }

  public synchronized void start () {
    notify();
  }

  public boolean mouseDown (Event evt, int x, int y) {
    if (evt.clickCount == 2) {
      start();
    }
    return true;
  }

  public void update (Graphics g) {
    g.drawImage(Greeting.dukes[0], 0, 0, this);
  }

  public void paint (Graphics g) {
    g.drawImage(Greeting.dukes[0], 0, 0, this);
  }    

  public synchronized void my_wait () throws InterruptedException {
    wait();
  }

  public void run () {
    int i = 0;
    Graphics g = null;
    Thread thd = runner;

    while (true) {
      try {
	my_wait();
      } catch (InterruptedException e) {
      }
      if ((g = getGraphics()) == null)
	continue;
      for (i = 0; i < 10; i++) {
	g.drawImage(Greeting.dukes[i], 0, 0, this);
	try {
	  thd.sleep(100);
	} catch (InterruptedException e) {
	}
      }
      g.drawImage(Greeting.dukes[0], 0, 0, this);
    }
  }
}
