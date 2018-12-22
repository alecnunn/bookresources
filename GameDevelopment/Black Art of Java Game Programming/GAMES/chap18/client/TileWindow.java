package client;

import java.awt.*;
import java.awt.image.*;

import common.*;

class TileWindow extends Canvas {
  public static final int SOUTH = 0;
  public static final int WEST = 1;
  public static final int NORTH = 2;
  public static final int EAST = 3;

  private static Image[] tiles = Greeting.tiles_large;

  // size of tile images
  private static int Xsize = 30;
  private static int Ysize = 40;
  private static int Pad = 8;

  private static int[] Rsize = { 40, 30, 40, 30 };
  private static int[] Csize = { 30, 40, 30, 40 };
  private static int[] CXinc = { 1, 0, 1, 0 };
  private static int[] CYinc = { 0, 1, 0, 1 };
  private static int[] RXinc = { 0, 1, 0, -1 };
  private static int[] RYinc = { -1, 0, 1, 0 };
  
  public Hand hand = null;
  public boolean editable = true;
  public boolean has_turn = false;
  private boolean default_size = true;
  private int rows, cols, col_width;
  private int side = SOUTH;

  public boolean dragable = false, dragged = false;
  public int drag_dx = 0, down_x = 0, down_pos = 0, drag_x0 = 0;

  public TileWindow (int Cols, int Rows, int Side, boolean Editable) {
    cols = Cols;
    rows = Rows;
    side = Side;
    switch (side) {
    case SOUTH:
    case NORTH:
      col_width = cols * (Pad + Xsize) + Pad;
      break;
    case EAST:
    case WEST:
      col_width = cols * (Pad + Ysize) + Pad;
      break;
    }
    editable = Editable;
    setBackground(Color.gray);
  }

  public void setTurn (boolean t) {
    Graphics g = getGraphics();
    has_turn = t;
    if (g != null) {
      if (has_turn) {
	g.setColor(Color.yellow);
      } else {
	g.setColor(getBackground());
      }
      g.drawRect(1, 1, size().width - 3, size().height - 3);
      g.drawRect(2, 2, size().width - 5, size().height - 5);
    }
  }

  public void setHand (Hand h) {
    hand = h;
    repaint();
  }

  public Dimension minimumSize () {
    return preferredSize();
  }

  public Dimension preferredSize () {
    switch (side) {
    case SOUTH:
    case NORTH:
      return new Dimension(cols * (Pad + Xsize) + Pad, 
			   rows * (Pad + Ysize) + Pad);
    case EAST:
    case WEST:
      return new Dimension(rows * (Pad + Xsize) + Pad, 
			   cols * (Pad + Ysize) + Pad);
    }
    return null;
  }

  public synchronized boolean mouseDown (Event evt, int x, int y) {
    int i = 0, pos = 0;
    if (!editable) {
      return true;
    }
    if ((x % (Xsize + Pad)) < Pad) {
      dragable = false;
      hand.unselectAll();
      paint(getGraphics());
      return true;
    }
    pos = x / (Xsize + Pad);
    if (pos < 0 || pos >= hand.length()) {
      dragable = false;
      hand.unselectAll();
      paint(getGraphics());
      return true;
    }
    hand.unselectAll();
    hand.select(pos);
    dragable = true;
    drag_dx = (Xsize + Pad) * pos + Pad - 2 - x;
    down_x = x;
    down_pos = pos;
    
    paint(getGraphics());
    return true;
  }
  
  public synchronized boolean mouseDrag (Event evt, int x, int y) {
    if (!editable)
      return true;
    if (!dragable)
      return true;
    Graphics g = getGraphics();
    if (!dragged) {
      dragged = true;
      drag_x0 = x;
      g.setColor(Color.black);
      g.setXORMode(Color.white);
      g.drawRect(x + drag_dx, Pad - 2, Xsize + 4, Ysize + 4);
      g.setPaintMode();
      return true;
    } else {
      g.setColor(Color.black);
      g.setXORMode(Color.white);
      g.drawRect(drag_x0 + drag_dx, Pad - 2, Xsize + 4, Ysize + 4);
      drag_x0 = x;
      g.drawRect(x + drag_dx, Pad - 2, Xsize + 4, Ysize + 4);
      g.setPaintMode();
      return true;
    }
  }

  public synchronized boolean mouseUp (Event evt, int x, int y) {
    int pos = 0;
    byte tile = 0;
    boolean sel = false;
    if (!editable)
      return true;
    if (!dragged)
      return true;
    dragged = false;
    dragable = false;
    Graphics g = getGraphics();
    g.setColor(Color.black);
    g.setXORMode(Color.white);
    g.drawRect(drag_x0 + drag_dx, Pad - 2, Xsize + 4, Ysize + 4);
    g.setPaintMode();
    if (x < down_x - 3) {           // drag left
      pos = (x + drag_dx + Xsize ) / (Xsize + Pad);
    } else if (x > down_x + 3) {
      pos = (x + drag_dx - Pad) / (Xsize + Pad);
    } else {
      return true;
    }
    tile = hand.tileAt(down_pos);
    sel = hand.isSelected(down_pos);
    hand.killTile(down_pos).insertTile(pos, tile, Hand.Single, sel);
    repaint();
    return true;
  }
  
  public boolean keyDown (Event evt, int key) {
    if (!editable) {
      return true;
    }
    if (key == 'c') {
      hand.unselectAll();
      repaint();
    }
    if (key == 's') {
      hand.sort();
      repaint();
    }
    return true;
  }

  public boolean imageUpdate (Image img, int flags,
			      int x, int y, int w, int h) {
    if ((flags & ALLBITS) != 0) {
      paint(getGraphics());
    }
    return (flags & (ALLBITS | ERROR)) == 0;
  }

  public void paint (Graphics g) {
    if (hand == null)
      return;
    if (has_turn) {
      g.setColor(Color.yellow);
    } else {
      g.setColor(getBackground());
    }
    g.drawRect(1, 1, size().width - 3, size().height - 3);
    g.drawRect(2, 2, size().width - 5, size().height - 5);

    // start drawing tiles.

    int i_row = 0;
    int p_col = Pad;
    int i_tile = 0, n_grp = 0;
    
    while (i_tile < hand.length()) {
      if (hand.groupAt(i_tile) == Hand.Single) {
	n_grp = 1;
      } else {
	int i = i_tile + 1;
	n_grp = 2;
	while (hand.groupAt(i) != Hand.Tail) {
	  i++;
	  n_grp++;
	}
      }
      int n_col = p_col + Csize[side] * n_grp + Pad;
      if (n_col > col_width) {
	if (++i_row == rows)             
	  return;                        // tile window is full
	n_col -= p_col - Pad;
	p_col = Pad;
      }
      int x = 0, y = 0;
      switch (side) {
      case SOUTH:
	x = p_col;
	y = (rows - 1 - i_row) * (Pad + Ysize) + Pad;
	break;
      case NORTH:
	x = p_col;
	y = i_row * (Pad + Ysize) + Pad;
	break;
      case EAST:
	x = (rows - 1 - i_row) * (Pad + Xsize) + Pad;
	y = p_col;
	break;
      case WEST:
	x = i_row * (Pad + Xsize) + Pad;
	y = p_col;
	break;
      }
      if (hand.isSelected(i_tile)) {
	g.setColor(Color.yellow);
      } else {
	g.setColor(getBackground());
      }
      g.drawRect(x - 2, y - 2,
		 Xsize + (n_grp - 1) * Xsize * CXinc[side] + 3,
		 Ysize + (n_grp - 1) * Ysize * CYinc[side] + 3);
      for (int n = 0; n < n_grp; n++) {
	if (default_size) {
	  g.drawImage(tiles[(int)hand.tileAt(i_tile++)],
		      x + n * Xsize * CXinc[side], 
		      y + n * Ysize * CYinc[side], this);
	} else {
	  g.drawImage(tiles[(int)hand.tileAt(i_tile++)],
		      x + n * Xsize * CXinc[side],
		      y + n * Ysize * CYinc[side],
		      Xsize, Ysize, this);
	}
      }
      p_col = n_col;
    }
  }
}

