/////////////////////////////////////////////////////////////////
// simple MediaTracker demo
/////////////////////////////////////////////////////////////////

import java.applet.*;
import java.awt.*;


public class Track extends Applet {
  MediaTracker t;
  Image i,j;
  
  public void init() {
    setBackground(Color.black);
    t = new MediaTracker(this);
    i = getImage(getCodeBase(),"sushi.gif");
    t.addImage(i,0);
    j = getImage(getCodeBase(),"chef.gif");
    t.addImage(j,0);
    showStatus("loading");

    // wait for all images to finish loading //
    try {
      t.waitForAll();               
    }  catch (InterruptedException e) {
    }

    // check for errors //
    if (t.isErrorAny()) {
      showStatus("error");
    }
    else if (t.checkAll()) {
      showStatus("successfully loaded");
    }

  }

  public void paint(Graphics g) {
    g.drawImage(i,13,17,this);
    g.drawImage(j,203,207,this);

  }
}
