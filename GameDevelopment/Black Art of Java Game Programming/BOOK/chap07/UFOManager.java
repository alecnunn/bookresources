
import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class UFOManager {


  private UFO ufo[];                 // array of ufos

  int ufosKilled;                    // count ufos killed
  int level;                         // the level 
                                     //    (i.e. #ufos on screen)
  int startLevel;
  int maxLevel;
  boolean playSound = false;         // initially no sound
  AudioClip expsound;                // sound clip of explosion

  // kill 13 ufos until next level
  static final int KILL_FOR_NEXT_LEVEL = 13; 

  static int width, height;          // applet dimensions

  // constructor
  public UFOManager(int startLevel,int maxLevel,int width,int height,
		    Image ufoImages[],
		    Image attackImages[],
		    Image explodeImages[],
		    Applet a, AudioClip exp) {
    this.startLevel = startLevel;
    this.maxLevel = maxLevel;
    this.width = width;
    this.height =  height;

    ufo = new UFO[maxLevel];
    for (int i=0; i<ufo.length; i++) {
      ufo[i] = new UFO(ufoImages,attackImages,explodeImages,
		       width,height,this,a);
    }
    expsound = exp;
    newGame();
  }
  

  // allow the UFO class to communicate with the gun
  public void initialize(GunManager gm) {
    UFO.initialize(gm);
  }

  // set ufo at a random screen location
  private void initializePosition(Moveable m) {
    m.setPosition(UFO.getRand(width - 100)+50,
		  UFO.getRand(height - 150)+31);

  }

  // initialize parameters of new Game
  public void newGame() {
    ufosKilled = 0;
    level = startLevel;              // start with startLevel ufos
                                     //   on the screen    
    for (int i=0; i<ufo.length; i++) {
      initializePosition(ufo[i]);
      if (i >= level) {              // suspend ufos
                                     //   above start level
	ufo[i].suspend();
      }
    }
  }


  // return array of ufos
  public UFO[] getUFO() {
    return ufo;
  }

  // paint all ufos in a level
  public void paint(Graphics g) {
    for (int i=0; i<level; i++) {
      ufo[i].paint(g);
    }
  }

  // update all ufos in a level. Otherwise start
  //   ufo if it's not on screen 
  public void update() {
    for (int i=0; i<level; i++) {
      if (ufo[i].isActive()) {
	ufo[i].update();
      }
      else {                  // make new ufo
	initializePosition(ufo[i]);
	ufo[i].init();
      }
    }
  }

  // tracks the number of ufos killed. If the
  //   num_killed is divisible by  KILL_FOR_NEXT_LEVEL
  //   increment the level
  public void killed() {
    ufosKilled++;
    if (ufosKilled % KILL_FOR_NEXT_LEVEL == 0) {
      level = (level == maxLevel) ? maxLevel : level+1;
    }
  }

  public void setStartLevel(int start) {
    startLevel = start;
  }

  public void setSound(boolean s) {
    playSound = s;
  }

  public void playExplosion() {
    if (playSound && expsound != null) {
      expsound.play();
    }
  }
}


