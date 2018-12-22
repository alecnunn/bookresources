import java.awt.*;
import java.lang.*;

public class StarField extends Sprite {

int xpt[],ypt[];
int NUM_STARS;
public boolean flag;

Graphics g;

StarField( int num, Rectangle r, Image im, ThreadGroup tg, String name) {
	super(tg,name);
	this.im=im;
	DELAY = 300;
	NUM_STARS = num;       
	bounds = r;

xpt = new int[num];
ypt = new int[num];
for(int i=0;i<NUM_STARS;i++) {
        xpt[i]=-1;
        ypt[i]=-1;
        addStar(i);
        }

anim = new Image[1];
animMax=1;

}                     


public void run() {
Graphics bg = im.getGraphics();
	while(true) {
		flag = true;
		paintSprite( bg );
		flag = false;
		anim[0]=im;
		im.flush();
//		theG.drawImage(im,bounds.x,bounds.y,null);
		advance();
		try{ sleep(300); } catch(Exception e);
	}
}

public void advance() {
int i, spd = getSpeed();


for(i=0;i<NUM_STARS-1;i++) {
	xpt[i]-=spd;


if( !bounds.inside(xpt[i],ypt[i]) ) {
        killStar(i);
        addStar(bounds.width-50);
	}
}
}

public void addStar(int min) {
int i,j;

for(i=0;i<NUM_STARS;i++)
        if(xpt[i]==-1 && ypt[i]==-1) {
                xpt[i] = bounds.x+min+(int)((bounds.width-min)*Math.random());
                ypt[i] = bounds.y+(int)(bounds.height*Math.random());
                }
}
         
public void killStar(int i) {
        xpt[i]=-1;
        ypt[i]=-1;
}

public void paintSprite(Graphics bg) {
int i;
im.flush();
g = im.getGraphics();
Rectangle r=bounds;
r.move(0,0);

g.setColor( Color.black );
g.fillRect( r.x,r.y,r.width,r.height );                                              

for(i=0;i<NUM_STARS;i++) {
        if( r.inside(xpt[i],ypt[i] ) ) {
/*
	g.setColor(Color.black);
	g.drawLine(xpt[i]+getSpeed(),ypt[i], xpt[i]+2*getSpeed(), ypt[i]);
*/
	g.setColor(Color.white);                                       
                g.drawLine( xpt[i], ypt[i], xpt[i]+getSpeed(), ypt[i] );
	}
	}
bg.drawImage(im,0,0,null);
}

public void paintChange( Graphics g, Rectangle r ) {
int i;
               
for(i=0;i<NUM_STARS;i++) {
        if( r.inside(xpt[i],ypt[i] ) ) {
	g.setColor(Color.black);
	g.drawLine(xpt[i]+getSpeed(),ypt[i], xpt[i]+2*getSpeed(), ypt[i]);
	g.setColor(Color.white);                                       
                g.drawLine( xpt[i], ypt[i], xpt[i]+getSpeed(), ypt[i] );
	}
	}
}       

}






