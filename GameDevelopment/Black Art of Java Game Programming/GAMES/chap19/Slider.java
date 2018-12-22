import java.applet.Applet;
import java.applet.AudioClip;
import java.lang.Object;
import java.awt.*;
import java.net.*;

// Slider.java 
// Steve Green 1996


// This is a simple wood block slider game. It will display up to an 8 * 8
// grid of blocks depending on how much room is allocated for the applet.
// NOTE: Each block is 48*48

// Thanks to Joe Carlson (j.carlson10@genie.geis.com or jcarlson@ecs.fullerton.edu 
// for both the inspiration and the wonderful wood block images.

public class Slider extends Applet implements Runnable {
	Tile grid[][];
	public static AudioClip clinkSound,introSound,winSound,badSound;
	public boolean intro = true, random = false;
	Dimension GridSize;
	Point blank;
	Thread kicker = null;

	public void init() {
		int x,y,block=1,maxBlocks;
		String name;
		
//		Set height and width

		GridSize = new Dimension(size().width/48,(size().height-50)/48);
		
		grid = new Tile[GridSize.height][GridSize.width];
		
//		Layout the applet

		setLayout(new BorderLayout());
		resize(size().width,size().height);
		Panel p = new Panel();
		add("South",p);
		p.add(new Button("Randomize"));

//		Load sounds...

		try {
			introSound = getAudioClip(new URL(getDocumentBase(),"sounds/spacemusic.au"));
			introSound.loop();
			clinkSound = getAudioClip(new URL(getDocumentBase(),"sounds/clink.au"));
			winSound = getAudioClip(new URL(getDocumentBase(),"sounds/laugh.au"));
		badSound = getAudioClip(new URL(getDocumentBase(),"sounds/crash.au"));
		} catch ( java.net.MalformedURLException e) {}

//		Load the blocks...

		maxBlocks = GridSize.height * GridSize.width;		
		
		for (y=0;y<GridSize.height;y++) 
			for (x=0;x<GridSize.width;x++) {
				if ( block < maxBlocks ) {
					name = "graphics/" + block + ".gif";
					grid[y][x] = new Tile(getImage(getCodeBase(),name),block,name);
					block++;
				}
				else {
					grid[x][y] = new Tile(null,0,null);
					blank = new Point(x,y);
				}
		}
		repaint();
	}

	public boolean action(Event evt, Object arg) {

		if ( arg.equals("Randomize") ) {
			random = true;
			repaint();
		}

		return true;
	}

	private void randomize(Graphics g){

		int i,iter=GridSize.height*GridSize.width*GridSize.width;
		Point from;
		
		for (i=0; i<iter;i++) {
			if ( Math.random() > 0.5 )
				move(new Point(blank.x,(int)(Math.random() * GridSize.height)));
			else
				move(new Point((int)(Math.random() * GridSize.width),blank.y));

			drawGrid(g);
		}
	}
				
	public void run() {
		Thread.currentThread().setPriority(Thread.MIN_PRIORITY);
		while (kicker != null) {
//			repaint();
			try {
				Thread.sleep(100);
			} catch (InterruptedException e){}
		}
	}

	private void drawAll() {
		int x,y;

		for (y=0;y<GridSize.height;y++)
			for (x=0;x<GridSize.width;x++)
				grid[y][x].changed = true;
		repaint();
	}

	public void paint(Graphics g) {

		drawAll();
	}

	public void start() {
		if ( kicker == null ) {
			kicker = new Thread(this);
			kicker.start();
		}
	}

	public void stop() {
		if ( intro )
			introSound.stop();
		kicker = null;
	}

	public void update(Graphics g) {

		if ( random )
			randomize(g);
		else
			drawGrid(g);

		random = false;
	}

	private void drawGrid(Graphics g) {
		int x,y;

//		Only draw blocks that have changed to improve display speed.

		for (y=0;y<GridSize.height;y++) 
			for (x=0;x<GridSize.width;x++)  {
				if ( grid[y][x].changed ) {
					if ( grid[y][x].number != 0 ) 
						g.drawImage(grid[y][x].img,x*48,y*48,this);
					else {
						g.clearRect(x*48,y*48,48,48);
					}
					grid[y][x].changed = false;
				}
			}
	}

	public boolean imageUpdate(Image img, int flags, int x, int y, int w, int h) {
	// When image is drawn, redraw THIS BLOCK.
	// This is done by first finding where the block in the grid.
	// THIS IS A BIT CRUDE!

		int iy,ix;

		if ((flags & (SOMEBITS|FRAMEBITS|ALLBITS)) != 0) {
//		if ((flags & ALLBITS) != 0) {
			for (iy=0;iy<GridSize.height;iy++)
				for (ix=0;ix<GridSize.width;ix++)
					if ( grid[iy][ix].img == img ) {
						grid[iy][ix].changed = true;
						repaint(100);
						return (flags & (ALLBITS|ERROR)) == 0;
					}
		}
		return (flags & (ALLBITS|ERROR)) == 0;
	}

	private boolean solved() {

//	Return true if the puzzle is solved, otherwise false

		int  y,x,block=1,iter = GridSize.height * GridSize.width;
		for (y=0;y<GridSize.height;y++)
			for (x=0;x<GridSize.width;x++)
				if ( block < iter )
					if ( grid[y][x].number != block++ )
						return false;
		return true;
	}

	private int delta(int a,int b) {

//	Return the direction to move the block in

		if ( a == b ) return 0;
		else return((b-a)/Math.abs(b-a));
	}

	public boolean mouseDown(Event evt, int x, int y) {

//	Mouse clicked somewhere

		Point from;	

//	Translate to block coordinates

		from = new Point(x/48,y/48);

// 	Kill the intro sound if still playing...

		if ( intro )
			introSound.stop();
		intro = false;

		if ( from.x >= GridSize.width || from.y >= GridSize.height )
			return true;

//	If on the same row or column as the blank we can move

		if ( move(from) ) {
			clinkSound.play();

//			Wait for about 250ms so that the sound and the
//			animation appear to be in sync.

			try {
				Thread.sleep(250);
			} catch (InterruptedException e) {}
			repaint();
			if ( solved() ) 
				winSound.play();
		}
		else 
			badSound.play();

		return true;
	}

	private boolean move(Point from) {
	  Point to,by;
	  
	  	by = new Point(delta(from.x,blank.x),delta(from.y,blank.y));
	  
		if ( (by.x == 0 && by.y == 0) || (by.x != 0 && by.y != 0) )
			return false;

		to = new Point(from.x + by.x, from.y + by.y);

//	Try and move, if blocked by another block, move it first...

		if ( grid[to.y][to.x].number != 0 ) 
			move(to);

//	Move the block and repaint

		grid[to.y][to.x].number = grid[from.y][from.x].number;	// to here...
		grid[to.y][to.x].img = grid[from.y][from.x].img;
		grid[to.y][to.x].changed = true;

		grid[from.y][from.x].number = 0;			// from here...
		grid[from.y][from.x].img = null;
		grid[from.y][from.x].changed = true;

		blank = from;				// blank is now here...
		return true;
	}
}

class Tile {
  Image img=null;
  int number=0;
  String name;
  boolean changed=true;

	public Tile(Image theImage,int i,String imageName) {

		name = imageName;
		number = i;
		img =  theImage;
	}
}
