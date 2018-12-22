import java.awt.*;


public class Sprite extends Thread {

/* here are the IDs */
static final int ENEMY = 2;
static final int BULLET = 3;
static final int USER = 5;
static final int EXPLOSION = 7;


double speedX=0,speedY=0;
int x,y, animIndex,animMax;
int lastX,lastY;
int DELAY=100;
Image anim[], im;
static Graphics theG;
Rectangle bounds;
Color theColor;
int id;


Sprite( Graphics g, Rectangle r, Image im ) {
	this(r,im);
	theG = g.create();

}

Sprite( Rectangle r, Image im) {
	x = r.x;
	y = r.y;
	bounds = r;
	this.im= im;
	setPriority( MIN_PRIORITY );
}	

public void setID(int id) {
	this.id = id;
}

public int queryID() {
	return id;
}
public void run() {
	while(true) {
		try{ sleep(DELAY); } catch(Exception e);
		paintSprite( theG );
//		advance();
		}
}


public void paintSprite( Graphics g ) {

if( theColor == null)
	switch( id ) {
	case ENEMY: {
		theColor = new Color( (int)(Math.random()*255),(int)(Math.random()*255),(int)(Math.random()*255));
		break;
		}
	case BULLET: {
		theColor = Color.green;
		break;
		}
	case USER: {
		theColor = Color.blue;
		break;
		}
	}		
	g.setColor( Color.black );
/*
	Rectangle temp = new Rectangle( new Point(x,y) );
	temp.add(lastX+bounds.width,lastY+((speedY>0) ? -1 : 1)*bounds.height);
	g.fillRect(x- temp.x,y-temp.y, temp.width+2, temp.height+2 );
*/
	g.fillRect( lastX,lastY,bounds.width,bounds.height);

	if( anim == null ) {
		Graphics blah = im.getGraphics();
		anim = new Image[1];
switch( id ) {
	case ENEMY: {
		blah.setColor( theColor );
		blah.fillRect( 0,0,bounds.width,bounds.height);
		break;
		}
	case BULLET: {
		blah.setColor( theColor );
		blah.fillRoundRect( 0,0,bounds.width,bounds.height,5,5);
		break;
		}
	case USER: {
		blah.setColor( Color.white );
		blah.drawOval( 0,0, bounds.width, bounds.height);
		blah.setColor( theColor );
		blah.fillOval( 0,0, bounds.width, bounds.height);
		break;
		}
	}

		anim[0]=im;
		animIndex=0;
		animMax=1;
		}
g.drawImage(anim[animIndex],x,y,null);
}
		
public void advance() {

if( anim != null ) {
	animIndex++;
	if( animIndex >= animMax )
		animIndex = 0;
	}

lastX=x;
lastY=y;	
x += (int)(speedX+.5);
y += (int)(speedY+.5);

bounds.move(x,y);
}

public void move(int x, int y) {
	this.x=x;
	this.y=y;
	bounds.x=x;
	bounds.y=y;
}
public void setSpeed( double X, double Y ) {
	speedX=X;
	speedY=Y;
}

public void setSpeed( double factor ) {
	speedX=factor;
	speedY=factor;
}

public int getSpeed() {
	if( speedY!=0 )
		return (int)(speedX/speedY);
	else
		return (int)(speedX+.5);
}

public void collision( int num ) {

if( id == BULLET || num%BULLET == 0)
	stop();
else
if( num == USER*ENEMY )
	stop();
}




public void finalize() {
	im.flush();
if( anim != null)
	for( int x=0;x<animMax;x++)
		anim[x].flush();
}	

}

