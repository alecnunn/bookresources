import java.awt.*;
import java.lang.*;

public class StarField extends Sprite {

int x[],y[];
int NUM_STARS;

Graphics g;

StarField( int num, Rectangle r, Image im) {

	this.im=im;
	DELAY = 300;
	NUM_STARS = num;       
	bounds = r;

x = new int[num];
y = new int[num];
for(int i=0;i<NUM_STARS;i++) {
        x[i]=-1;
        y[i]=-1;
        addStar(i);
        }
}                     


public void run() {
Graphics bg = im.getGraphics();
	while(true) {
//		paintSprite( bg,bounds);
//		theG.drawImage(im,bounds.x,bounds.y,null);
		advance();
		try{ sleep(300); } catch(Exception e);
	}
}

public void advance() {
int i, spd = (int)(speedX+.5);


for(i=0;i<NUM_STARS-1;i++) {
	x[i]-=spd;


if( !bounds.inside(x[i],y[i]) ) {
        killStar(i);
        addStar(bounds.width-50);
	}
}
}

public void addStar(int min) {
int i,j;

for(i=0;i<NUM_STARS;i++)
        if(x[i]<0 || y[i]<0) {
                x[i] = bounds.x+min+(int)((bounds.width-min)*Math.random());
                y[i] = bounds.y+(int)(bounds.height*Math.random());
                }
}
         

public void killStar(int i) {
        x[i]=-1;
        y[i]=-1;
}

public void paintSprite(Graphics bg) {
int i;
im.flush();
g = im.getGraphics();
Rectangle r=bounds;

g.setColor( Color.black );
g.fillRect( r.x,r.y,r.width,r.height );                                              

for(i=0;i<NUM_STARS;i++) {
        if( r.inside(x[i],y[i] ) ) {
/*
	g.setColor(Color.black);
	g.drawLine(x[i]+getSpeed(),y[i], x[i]+2*(int)(speedX+.5), y[i]);
*/
	g.setColor(Color.white);                                       
                g.drawLine( x[i], y[i], x[i]+(int)(speedX+.5), y[i] );
	}
	}
bg.drawImage(im,0,0,null);
}

public void paintChange( Graphics g, Rectangle r ) {
int i;
               
for(i=0;i<NUM_STARS;i++) {
        if( r.inside(x[i],y[i] ) ) {
	g.setColor(Color.black);
	g.drawLine(x[i]+(int)(speedX+.5),y[i], x[i]+2*(int)(speedX+.5), y[i]);
	g.setColor(Color.white);                                       
                g.drawLine( x[i], y[i], x[i]+(int)(speedX+.5), y[i] );
	}
	}
}       

}






