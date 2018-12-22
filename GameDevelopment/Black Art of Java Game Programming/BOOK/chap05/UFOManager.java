
import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class UFOManager {


  static int width, height;            // applet dimensions
  private UFO ufo[];
  static final int NUM_UFOS = 7;
  
  public UFOManager(int width,int height,
		    Image ufoImages[],Applet a) {
    this.width = width;
    this.height =  height;

    ufo = new UFO[NUM_UFOS];
    for (int i=0; i<ufo.length; i++) {
      ufo[i] = new UFO(ufoImages,width,height,a);
      initializePosition(ufo[i]);
    }
  }
  

  public void initialize(GunManager gm) {
    UFO.initialize(gm);
  }

  private void initializePosition(Moveable m) {
    m.setPosition(UFO.getRand(width - 100)+50,
		  UFO.getRand(height - 150)+10);

  }

  public UFO[] getUFO() {
    return ufo;
  }

  public void paint(Graphics g) {
    for (int i=0; i<ufo.length; i++) {
      ufo[i].paint(g);
    }
  }

  public void update() {
    for (int i=0; i<ufo.length; i++) {
      if (ufo[i].isActive()) {
	ufo[i].update();
      }
      else {                  // make new ufo
	initializePosition(ufo[i]);
	ufo[i].restore();
      }
    }
  }

}


