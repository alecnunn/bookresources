import java.awt.*;
import GamePiece;

public class GameBoard {

final boolean BLACK = false;
final boolean WHITE = true;

public int pieceWidth, pieceHeight;
public int rows, cols;  // number of rows/columns
GamePiece board[][]; // the actual board itself
public int empty;
Image im[];

GameBoard( int w, int h, Image images[]) {
	board = new GamePiece[w][h];
	cols = w;
	rows = h;
	empty = w*h;
	im = images;
}

public void paintBoard(Graphics g, Rectangle r) {

int x;
int y;

pieceWidth = (int) (r.width/cols);
pieceHeight = (int) (r.height/rows);

g.setColor(Color.green);
g.fillRect( r.x, r.y, r.width, r.height );

for( x=0; x<cols; x++) {
	g.setColor( Color.black );
	g.drawLine( x* pieceWidth, 0, x*pieceWidth, r.height );
	for(y=0; y< rows; y++) {
		g.setColor( Color.black );
		g.drawLine(0, y* pieceHeight, r.width, y*pieceHeight );
		if( board[x][y] != null )
			board[x][y].paintPiece( g, new Rectangle( r.x + pieceWidth*x, r.y + pieceHeight*y, pieceWidth, pieceHeight), (board[x][y].COLOR) ? im[1] : im[2] );
		else
			g.drawImage(im[0],  r.x + pieceWidth*x, r.y + pieceHeight*y, pieceWidth, pieceHeight, null);
		}
	}
}


public GamePiece pieceAt(int x, int y) {

if( x>=cols || x<0 || y>= rows || y<0 ) return null;

return board[x][y];
}

public GamePiece pieceAtXY( int x, int y) {
int i,j;

for(i=0; i<cols; i++)
	for(j=0; j<rows; j++) 
		if( board[i][j].rect.inside(x,y) )
			return board[i][j];
return null;
}

public void addPiece( int x, int y, boolean color) {

if( x> cols || x<0 || y>rows || y<0) return ;

if( board[x][y] == null ) empty--;
board[x][y] = new GamePiece( color );
}

public int count( boolean color) {
int i,j,num=0;

for(i=0; i<rows; i++)
for(j=0; j<cols; j++)
	if(board[i][j] != null && board[i][j].COLOR == color )
		num++;
return num;
}


}