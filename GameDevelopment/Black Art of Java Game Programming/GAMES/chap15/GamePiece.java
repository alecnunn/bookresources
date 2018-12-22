import java.awt.*;

public class GamePiece {

final boolean BLACK = false;
final boolean WHITE = true;

public boolean COLOR;
public Rectangle rect;
Image im;

GamePiece( boolean c) {
	COLOR=c;
}

public void flip() {
	COLOR=!COLOR;
}

public void paintPiece(Graphics g) {
	paintPiece( g, rect );
}

public void paintPiece(Graphics g, Rectangle r ) {
	paintPiece(g,r,null);
	}

public void paintPiece(Graphics g, Rectangle r, Image im ) {

rect = r;

if( ! r.intersects(g.getClipRect()) ) return;

if ( COLOR == WHITE ) 
	g.setColor(Color.white);
else
	g.setColor(Color.black);

if( im == null )
g.fillOval( rect.x, rect.y, rect.width, rect.height );
else
g.drawImage(im, rect.x,rect.y,rect.width, rect.height, null );

}

}
