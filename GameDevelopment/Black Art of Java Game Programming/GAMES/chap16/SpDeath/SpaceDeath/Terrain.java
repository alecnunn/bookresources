import java.awt.*;
import java.util.*;
import java.lang.*;

public class Terrain extends Thread {

Vector v;
int WIDTH;                 
Rectangle bounds;
Graphics theG;
int lasty=0;
boolean repaint;

Terrain( int w, Rectangle r, Graphics g ) {
        WIDTH = w;
        bounds = r;
	theG = g.create();

int num = (int) (r.width/WIDTH);
v = new Vector(num);
while( num-- >= 0 ) 
        v.addElement(nextPoly());
}

public void repaint() {
	repaint=true;
}

public void run() {
	while(true) {
		if( repaint ) {
			paintAll(theG,bounds);
			repaint = false;
			}
		else
			paintChange(theG,bounds);
		advance();
		try{ sleep(200); } catch(Exception e);

	}
}		

      
void paintPolyXY(Graphics g, Polygon p, int x, int y) {
	for(int i=0;i<p.npoints;i++) {
		p.xpoints[i]+=x;
		p.ypoints[i]+=y;
		}
	g.fillPolygon(p);

}

public void paintAll( Graphics g, Rectangle r) {

g.setColor(Color.black);
g.fillRect(bounds.x,bounds.y,bounds.width,bounds.height);

g.setColor( Color.yellow );
for(int x=0;x<v.size();x++) {
	Polygon p2 = (Polygon) v.elementAt(x);
	Polygon p = new Polygon(p2.xpoints,p2.ypoints,p2.npoints);
	paintPolyXY(g,p,r.x+x*WIDTH,r.y);
        }
}
         
public void paintChange( Graphics g, Rectangle r) {


	g.copyArea( r.x,r.y,r.width,r.height,-WIDTH,0);
	g.setColor(Color.black);
	g.fillRect( r.x+r.width-WIDTH,r.y,WIDTH,r.height);
	g.setColor(Color.yellow);
	paintPolyXY(g, (Polygon)v.lastElement(), r.x+r.width-WIDTH,r.y);

}

public void advance() {

v.removeElementAt(0);
v.addElement( nextPoly() );

}

public Polygon nextPoly() {
Polygon p = new Polygon();
        p.addPoint( (int)(WIDTH/2), (int)(bounds.height * Math.random()) );
        p.addPoint( 0, lasty );
        p.addPoint( 0, bounds.height );
        p.addPoint( WIDTH, bounds.height );
        p.addPoint( WIDTH, lasty = (int)(bounds.height * Math.random() ) );
return p;
}        
                       
}




