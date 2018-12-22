import java.applet.*;
import java.awt.*;
import java.util.*;
import Terrain;
import StarField;                  
import Sprite;
import userSprite;

public class SpaceDeath extends Applet implements Runnable {
Thread kicker;      
StarField SF=null;                         
Terrain T=null;
Rectangle SFrect, Trect;
Dimension d;
int LEVEL=1;
int SPEED =2;
Sprite user;
Vector sprites;

public void init() {
        Trect = new Rectangle( 0, (int)(size().height*.8), size().width, (int)(size().height*.2) );
        SFrect = new Rectangle( 0,0,size().width,size().height-Trect.height);
	d = size();
        kicker = new Thread(this);
	kicker.setPriority(Thread.MAX_PRIORITY);
        kicker.start();
	repaint();
sprites = new Vector();
setBackground( Color.black );
	user = newEnemy();
	user.setID( Sprite.USER );
	user.move( 50,50 );
	user.setSpeed(0);
	user.start();
	sprites.addElement(user);
}

public void update(Graphics g) {

	if( Sprite.theG == null) 
		Sprite.theG = g.create();
	if( SF == null ) {
		SF = new StarField(100, SFrect, createImage(SFrect.width, SFrect.height));
		g.setColor(Color.black);
		g.fillRect(0,0,size().width,size().height-Trect.height);

		SF.setSpeed(2,0);
		SF.start();
		} 

	if( T == null || !T.isAlive()) {
               		T = new Terrain(50,Trect,g);
		T.start();
		} 

	paint(g);

}

public void paint(Graphics g) {


	SF.paintSprite(g,SFrect);
//	T.repaint();
}                      


public void run() {

repaint();
while(true) {
        try{ kicker.sleep(100); } catch(Exception e);
	doCheck();
        }
}  

public void doCheck() {
int i,j;


	if( sprites.isEmpty() || (sprites.size()<10 &&Math.random() > LEVEL/10)) {
		Sprite s = newEnemy();
		s.start();
		sprites.addElement( s );
		}
Sprite s1,s2;
for(i=0;i<sprites.size();i++) {
	s1 = (Sprite) sprites.elementAt(i);
	if( s1== null || !s1.isAlive() ) {
		sprites.removeElement(s1);
	} else
	for(j=i+1;j<sprites.size();j++) {
		s2 = (Sprite) sprites.elementAt(j);
		if( s1.bounds.intersects(s2.bounds) ) {
			s1.collision( s1.id * s2.id );
			s2.collision( s1.id * s2.id );
			}
		s1.advance();
		}
}
/*
Sprite bullet, s;
for(i=0;i<enemies.size();i++) {
s = (Sprite)enemies.elementAt(i);

if( s==null ) 
	enemies.removeElementAt(i);
else 
	if(  !s.bounds.intersects( SFrect ) || s.bounds.intersects(Trect)) {
		s.stop();
		s=null;
		enemies.removeElementAt(i);
		}
	else
	if( !bullets.isEmpty() )
		for(int j=0;j<bullets.size();j++) {
	bullet = (Sprite) bullets.elementAt(j);

			if( bullet.bounds.intersects( s.bounds ) ) {
				bullet.stop();
				s.stop();
				s= null;
				b = null;
				enemies.removeElementAt(i);
				bullets.removeElementAt(j);
				}

		}

}
*/
}	

public boolean mouseDown(Event evt, int x, int y) {
Sprite b;

	b = newBullet(x,y);
	sprites.addElement(b);
	b.start();
return true;
}


void suspendAll() {
int i;
	for(i=0;i<sprites.size();i++)
		sprites.elementAt(i).suspend();
	SF.suspend();
	T.suspend();
	kicker.suspend();
}

void resumeAll() {
	kicker.resume();
int i;
	for(i=0;i<sprites.size();i++)
		sprites.elementAt(i).resume();

	SF.resume();
	T.resume();
}



public Sprite newEnemy() {
Sprite s;
double spdX, spdY;
Rectangle temp = new Rectangle( SFrect.width + SFrect.x-10, SFrect.y+(int)(Math.random()*SFrect.height), 15,15);

	spdX = 10*Math.random()+2;
	spdY= 5*Math.random();
	s = new Sprite( temp, createImage(temp.width, temp.height ));

if( Math.random() > .5 )
	spdY=-spdY;
		s.setSpeed( -spdX, spdY);
s.setID( Sprite.ENEMY );
return s;
}

public Sprite newBullet(int x, int y) {
Sprite b;
Rectangle temp = new Rectangle( x,y, 15,3 );
	b = new Sprite(temp, createImage(temp.width, temp.height));
	b.setSpeed(25,0);
	b.setID( Sprite.BULLET );
return b;
}

public boolean keyDown(Event evt, int key) {

if(key == 'p') {
	suspendAll();
	return true;
	}
if(key == 'r') {
	resumeAll();
	return true;
	}

if( key == ' ') {
	Sprite b=newBullet(user.x,user.y);
	b.start();
	sprites.addElement(b);
	return true;
	}

if( evt.id == Event.UP ) {
	user.y-=5;
	return true;
	}

if( evt.id == Event.DOWN) {
	user.y+=5;
	return true;
	}
return false;
}
	


	
/* END OF CLASS */
}


