
import java.awt.*;
import java.applet.*;
import java.util.*;

/////////////////////////////////////////////////////////////////
//
// EffManager: handles effects, such as explosions and sound
//
/////////////////////////////////////////////////////////////////
 
public class EffManager extends Object {

  Vector explosions;
  AudioClip expsound;

/////////////////////////////////////////////////////////////////
// EffManager constructor
/////////////////////////////////////////////////////////////////

  public EffManager(AudioClip expsound) {
    explosions = new Vector();
    this.expsound = expsound;
  }

/////////////////////////////////////////////////////////////////
// make a ship explosion at x,y
/////////////////////////////////////////////////////////////////

  public void addShipExplosion(int x,int y) {
    Explosion exp = new Explosion(Color.red,x,y,20);
    Explosion exp1 = new Explosion(Color.yellow,x+2,y+2,25);
    explosions.addElement(exp);
    explosions.addElement(exp1);
    if (expsound != null) {
      expsound.play();
    }    
  }

/////////////////////////////////////////////////////////////////
// make an asteroid explosion at x,y
/////////////////////////////////////////////////////////////////
  public void addAstExplosion(int x,int y) {
    Explosion exp = new Explosion(Color.white,x,y,15);
    explosions.addElement(exp);
    if (expsound != null) {
      expsound.play();
    }
  }

/////////////////////////////////////////////////////////////////
// make an asteroid explosion at x,y
/////////////////////////////////////////////////////////////////
  public void addEnemyExplosion(int x,int y) {
    Explosion exp = new Explosion(Color.orange,x,y,15);
    explosions.addElement(exp);
    if (expsound != null) {
      expsound.play();
    }
  }

/////////////////////////////////////////////////////////////////
// make an explosion of the given color at x,y
/////////////////////////////////////////////////////////////////
  public void addExplosion(Color c,int x, int y, int u) {
    Explosion exp = new Explosion(c,x,y,u);
    explosions.addElement(exp);
    if (expsound != null) {
      expsound.play();
    }
  }

/////////////////////////////////////////////////////////////////
// paint the explosions by stepping through the 
//   explosions Vector
///////////////////////////////////////////////////////////////// 
  public void paint (Graphics g) {
     Enumeration e;
      e = explosions.elements();
      while (e.hasMoreElements()) {
	Sprite exp = (Sprite) e.nextElement();
	exp.paint(g);
      }
   }

/////////////////////////////////////////////////////////////////
// update the explosions by stepping through the 
//   explosions Vector
/////////////////////////////////////////////////////////////////
  public void update() {
    Enumeration e;
    e = explosions.elements();
    while (e.hasMoreElements()) {
      Explosion exp = (Explosion) e.nextElement();
      exp.update();
      // if the explosion's finished, remove it from vector
      if (exp.isDone()) {
	explosions.removeElement(exp);
      }
    }
  }
}
