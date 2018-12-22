import java.applet.*;
import java.awt.*;
import java.util.*;
import java.net.*;
import java.io.*;
import GameBoard;

public class Othello extends Applet implements Runnable {

final boolean WHITE = true;
final boolean BLACK = false;

/* the Game stuff */
GameBoard theBoard;
boolean turn;  // whose turn is it? 
boolean local; // What color is the local computer?

/* the GUI stuff */
TextArea dispA;
Panel inputPanel, turnPanel;
promptFrame pf;

/* the Thread */
Thread kicker;

/* the network stuff */
PrintStream ps;
Socket s=null;
DataInputStream dis = null;
String name, theHost="localhost";
int thePort=8314;

Image im[];

public void init() {

im= new Image[3];
im[0] = getImage( getCodeBase(), "images/empty_red.jpg" );
im[1] = getImage( getCodeBase(), "images/white_red.jpg" );
im[2] = getImage( getCodeBase(), "images/black_red.jpg" );

/* set up all GUI components */
setLayout( new BorderLayout() );
inputPanel = new Panel();
inputPanel.setLayout( new BorderLayout() );
inputPanel.add( "West", dispA=new TextArea( 5,35));
inputPanel.add( "South",new TextField(20) );
inputPanel.add("Center", turnPanel=new Panel() );
turnPanel.add( new Button("Reload") );
add( "South", inputPanel );

/* check applet parameters */
try{
	thePort = Integer.valueOf( getParameter("port") ).intValue();
} catch(Exception e) {
	thePort = 8314;
}

/* start a new game */
newGame();
}

public void newGame() {
/* initialize the board, and pop up a window */
	initBoard();
	promptUser();

/* start the thread */
	kicker = new Thread(this);
	kicker.setPriority(Thread.MIN_PRIORITY);
	kicker.start();
}

/* this method sets up the board */
public void initBoard() {
turn = WHITE;
	theBoard = new GameBoard( 8, 8, im);
	theBoard.addPiece( 3,3, WHITE);
	theBoard.addPiece( 3,4, BLACK);
	theBoard.addPiece( 4,3, BLACK);
	theBoard.addPiece( 4,4, WHITE);
}

/* pops up a new promptFrame (see promptFrame.class below) */
public void promptUser() {

pf = new promptFrame();
pf.resize(300,100);
pf.show();
}


/* the main Thread loop */
public void run() {

/* first, wait for pf to get a name from the user */
name = null;
while( name == null) {
	name = pf.gotName;
	try { kicker.sleep(500); } catch(Exception e);
	}
/* get rid of pf */
pf.dispose();
s = null;

/* ok, now make the socket connection */
while( s == null )
try{
	theHost = getCodeBase().getHost();
	display("Attempting to make connection");
	s = new Socket(theHost,thePort);
	dis = new DataInputStream( s.getInputStream() );
	ps = new PrintStream( s.getOutputStream() );
} catch( Exception e) {
	try { kicker.sleep(7500); } catch(Exception ex);
}

display("Connection established");
display("Waiting for another player...");


/* here is the main event loop */
while( kicker != null) {
String input=null;
StringTokenizer st = null;

while( input == null) 
try {
	kicker.sleep(100);
	input = dis.readLine();
} catch (Exception e) {
	input = null;
}
System.out.println("Got: "+input);

/* if the other person disconnected for any reason... start over */
if( input.equals("bye" ) ) {
	display("Your partner has left the game... Restarting");
	newGame();
	repaint(); 
	return;
	}

st = new StringTokenizer(input,"||");
String cmd = st.nextToken();
String val = st.nextToken();

/* if we are ready to start a game */
if( cmd.equals("start") ) {
	display("Got another player.");
	if( val.equals("black") ) local = BLACK;
	else local = WHITE;
	display("You will play "+val);
	repaint();
	}
else /* if we got a move, make it */
if(  cmd.equals("move")) 
try{ 
	doMove( Integer.valueOf(val).intValue(), Integer.valueOf(st.nextToken()).intValue(), turn);
} catch(Exception e) {
	System.out.println("POINT FOUR\n"+e);
}
else /* if this is a message from a player */
if(cmd.equals("say"))
	display( val );
}
}

/* if the Thread stops, be sure to clean up! */
public void stop() {

try {
	ps.println("bye");
	dis.close();
	ps.close()	;
	s.close();

} catch (Exception e);
}

/* just pass along all paint work to the GameBoard class */
public void paint(Graphics g) {
	Dimension d = size();
	d.height -= inputPanel.preferredSize().height;
	theBoard.paintBoard(g, new Rectangle( 0,0, d.width, d.height ) );
}

/* check if the screen coordinates x,y are in a valid square */
/* validMove() does the work, all we do is convert the numbers  */
boolean validMoveXY( int x, int y, boolean color) {
	return validMove( (int) (x/theBoard.pieceWidth), (int) (y/theBoard.pieceHeight), color );
}

/* ok, check if x,y is a valid square for color */
boolean validMove(int x, int y, boolean color) {

/* if there already is a piece at x,y */
if( theBoard.pieceAt(x,y) != null ) return false;

int a,b,i,j, num;
GamePiece temp=null;

/* check in all four directions */
for(i=-1;i<=1;i++)
for(j=-1;j<=1;j++) 
if( !(j == 0 && i == 0) ) {  /* can't check in the 0 direction */
a=x;
b=y;
num=0;
	do {
		a+=i;
		b+=j;
		temp = theBoard.pieceAt(a,b);
		if( temp != null && temp.COLOR == !color )
			num++;
		if( temp != null && temp.COLOR == color )
			if( num > 0) return true; /* this is the only thing that returns true */
			else temp = null;
	} while (temp != null);
}
return false;

}

public boolean mouseDown(Event evt, int x, int y) {

if( turn == local) { /* if it's our turn */
	if( validMoveXY(x,y,turn) ) { /* if we got a valid move */
	int xx,yy;
		xx= (int)(x/theBoard.pieceWidth);
		yy=(int)(y/theBoard.pieceHeight);
	boolean flag=false;
/* be SURE the move gets sent */
	while( !flag ) 
		try{
			ps.println("move||"+xx+"||"+yy);
			ps.flush();
			System.out.println("Sent move: "+xx+","+yy);
			flag = true;
		} catch (Exception e) {
			flag = false;
		}
	return true;
	}
}

return false;
}

/* ok this actually makes a move.  Very similar to validMove() */
public void doMove(int x, int y, boolean color) {

int a=x,b=y,i,j, num;
GamePiece temp=null;


theBoard.addPiece( x,y,color );
repaint( a * theBoard.pieceWidth, b * theBoard.pieceHeight, theBoard.pieceWidth, theBoard.pieceHeight);

for(i=-1;i<=1;i++)
for(j=-1;j<=1;j++) 
if( !(j == 0 && i == 0) ) {
a=x;
b=y;
num=0;
	do {	
		a+=i;
		b+=j;
		temp = theBoard.pieceAt(a,b);
		if( temp != null) {
		if( temp.COLOR == !color ) 
			num++;
		else if( temp.COLOR == color ) {
			if( num > 0) {
				a=x+i;
				b=y+j;
				temp = theBoard.pieceAt(a,b);
				while( temp.COLOR == !color) {
					theBoard.pieceAt(a,b).flip();
					repaint( a * theBoard.pieceWidth, b * theBoard.pieceHeight, theBoard.pieceWidth, theBoard.pieceHeight);
					a+=i;
					b+=j;
					temp = theBoard.pieceAt(a,b);
					}		
				}
				temp = null;
				}
			}
	//	} 
		} while (temp != null);
}

int bl,wh;

if( validMoves( !turn ) > 0 ) /* if the other player has valid moves, switch turns */
	turn = !turn;

	Dimension d = turnPanel.size();
	if( turn == WHITE ) turnPanel.getGraphics().drawImage(im[1],0,0,d.width,d.height,null);
	else turnPanel.getGraphics().drawImage(im[2],0,0,d.width,d.height,null);


/* if the game is over */
	if( endGame() ) 
		if( (wh=theBoard.count(WHITE)) > (bl=theBoard.count(BLACK)) )
			display("White wins!");
		else if(wh<bl) 
			display("Black wins!");
		else if(bl==wh)
			display("It's a TIE!");
}

/* check if the game is over */
public boolean endGame() {

	if( theBoard.empty == 0) 
		return true;
	else 
	if( theBoard.count( BLACK ) == 0 || theBoard.count(WHITE) ==0 )
		return true;
	else
	if( validMoves( BLACK ) ==0 && validMoves( WHITE ) ==0)
		return true;
return false;
}

/* count the number of valid moves for color */
int validMoves( boolean color ) {
int i,j,num=0;

for(i=0; i<8; i++)
	for( j=0; j<8; j++)
		if( validMove( i,j, color ) )
			num++;
return num;
}

/* display a string in the TextArea */
public void display(String str) {
	dispA.appendText(str+"\n");
}


public boolean handleEvent(Event evt) {

/* if the user entered text in the TextField and hit enter */
if( evt.target instanceof TextField && evt.id == Event.ACTION_EVENT ) {
	ps.println("say||"+name+" says: "+((TextField)evt.target).getText() );
	((TextField)evt.target).setText("");
	return true;
	}

/* if the user clicked on the restart button */
if( evt.target instanceof Button && evt.arg.equals("Restart") ) {
	kicker.stop();
	newGame();
	repaint();
	return true;
	}
return super.handleEvent(evt);
}
}

/* this is a simple window that gets a name from the user */
class promptFrame extends Frame {
TextField TF;
public String gotName;
promptFrame() {
	super("Ready to play Othello?");
	setLayout( new FlowLayout() );
	add( "West",new Label("Input your name: ") );
	add("West",TF = new TextField(20) );
	add("Center",new Button("Login") );
	gotName = null;
}

public boolean handleEvent(Event evt) {

if( (evt.target instanceof TextField && evt.id==Event.ACTION_EVENT) || (evt.target instanceof Button && evt.arg.equals("Login")) ) {
	if( TF.getText() == "" )
		gotName = "Noname";
	else
		gotName = TF.getText();
	return true;
	}
return super.handleEvent(evt);
}

}



