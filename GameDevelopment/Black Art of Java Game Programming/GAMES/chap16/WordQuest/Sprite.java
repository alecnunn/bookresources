import java.awt.*;


public class Sprite extends Thread {

/* here are the IDs */
static final int ENEMY = 2;
static final int BULLET = 3;
static final int USER = 5;
static final int EXPLOSION = 7;
static final int MESSAGE = 11;

int WIDTH=40,HEIGHT=25;


double speedX=0,speedY=0;
int x,y, animIndex,animMax;
int lastX,lastY;
int DELAY=300;
public Image anim[], im;

static Graphics theG;
static int warp=1;

Rectangle bounds;
Color theColor;
int id=0;
String data;


Sprite( ThreadGroup p, String d) {
	super(p,d);
	data=d;
}

public void setXY(int x, int y) {
	this.x=x;
	this.y=y;
if( data != null && !data.equals("user")) {
	FontMetrics fm = theG.getFontMetrics();
	WIDTH = fm.stringWidth( data ) + 10;

}
	
if( bounds == null)
	bounds = new Rectangle( x,y,WIDTH,HEIGHT );
else {
	bounds.x=x;
	bounds.y=y;
	}

}

public void setBounds( Rectangle r) {
	bounds = r;
	x = bounds.x;
	y = bounds.y;
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
//		paintSprite( theG );
		advance();
		}
}


public void paintSprite( Graphics g ) {

g.setColor( Color.black );

/*
if( lastX!=x || lastY!=y)
	g.fillRect( lastX,lastY,bounds.width,bounds.height);
*/

	if( anim == null ) 
		generateImage();
g.drawImage(anim[animIndex],x,y,null);
}
	
public Image currentImage() {
if( anim== null) 
	generateImage();
return anim[animIndex];
}

public synchronized void generateImage() {

if( im==null) return;

		Graphics blah = im.getGraphics();
		anim = new Image[1];
switch( id ) {
	case MESSAGE: {
		blah.fillRect(0,0,bounds.width,bounds.height);
		blah.setFont( new Font("Times New Roman",Font.BOLD,14));
		blah.setColor(theColor);
		blah.drawString(data,0,bounds.height);
		break;
		}
	case ENEMY: {
		theColor = new Color( (int)(Math.random()*255),(int)(Math.random()*255),(int)(Math.random()*255));
		blah.setColor( theColor );
		blah.fillRect( 0,0,bounds.width,bounds.height);
		blah.setColor(Color.black);
		blah.fillRect( 5, (int)(bounds.height/2-theG.getFont().getSize()/2-1),WIDTH-10,theG.getFont().getSize()+2);
		blah.setColor(Color.white);
		blah.drawString( data, 5, (int)(bounds.height/2+theG.getFont().getSize()/2) );
		break;
		}
	case BULLET: {
		theColor = Color.green;
		blah.setColor( theColor );
		blah.fillRoundRect( 0,0,bounds.width,bounds.height,5,5);
		break;
		}
	case USER: {
		theColor = Color.blue;
		blah.setColor( Color.black );
		blah.fillRect( 0,0,bounds.width,bounds.height );
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
	
public void advance() {

if( anim != null ) {
	animIndex++;
	if( animIndex >= animMax )
		animIndex = 0;
	}

lastX=x;
lastY=y;	
x += warp*(int)(speedX+.5);
y += warp*(int)(speedY+.5);

bounds.move(x,y);


}

public void move(int x, int y) {
	this.x=x;
	this.y=y;
	bounds.move(x,y);
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
		return warp*(int)(speedX/speedY);
	else
		return warp*(int)(speedX+.5);
}

public void collision( int num ) {

if( num == 0 ) return;
if( id == MESSAGE ) return;


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
anim = null;
}	

public boolean intersects( Rectangle r) {
	return bounds.intersects(r);
}

}

