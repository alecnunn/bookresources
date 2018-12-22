package client;

import java.awt.*;
import java.util.Vector;
import java.util.Enumeration;

public class SimpleList extends Canvas {
  protected static final int PAD = 3;

  protected int cols, rows;
  protected int font_height, font_width, font_ascent, font_descent;
  protected int width, height;
  protected int n_items;
  protected int max_len;
  protected Vector items;
  protected int n0;
  protected int c0;
  protected int sel;

  public SimpleList (int r, int c) {
    super();
    cols = c;
    rows = r;
    n_items = 0;
    max_len = 0;
    items = new Vector();
    n0 = 0;
    c0 = 0;
    sel = -1;
    setFont(new Font("Courier", Font.PLAIN, 12));
    setBackground(Color.lightGray);
    setForeground(Color.black);
  }

  public void setSize (int r, int c) {
    cols = c;
    rows = r;
    super.resize(minimumSize());
    repaint();
  }

  public int getRows () {
    return rows;
  }

  public int getCols () {
    return cols;
  }

  public void setFont (Font font) {
    super.setFont(font);
    FontMetrics metrics = getToolkit().getFontMetrics(font);
    font_height = metrics.getHeight();
    font_width = metrics.charWidth(' ');
    font_ascent = metrics.getAscent();
    font_descent = metrics.getDescent();
  }
  
  public Dimension minimumSize () {
    return new Dimension(cols * font_width + 2 * PAD, 
			 rows * font_height + 2 * PAD);
  }
  
  public Dimension preferredSize () {
    return minimumSize();
  }

  public void reshape (int x, int y, int w, int h) {
    width = w;
    height = h;
    super.reshape(x, y, w, h);
    cols = (w - 2 * PAD) / font_width;
    rows = (h - 2 * PAD) / font_height;
    getParent().postEvent(new Event(this, Event.ACTION_EVENT, "RESHAPE"));
  }    

  public void resize (int w, int h) {
    width = w;
    height = h;
    super.resize(w, h);
    cols = (w - 2 * PAD) / font_width;
    rows = (h - 2 * PAD) / font_height;
    getParent().postEvent(new Event(this, Event.ACTION_EVENT, "RESHAPE"));
  }

  public void resize (Dimension d) {
    resize(d.width, d.height);
  }

  protected boolean visible (int n) {
    return n >= n0 && n < n0 + rows;
  }

  public int countItems () {
    return n_items;
  }

  public int maxLength () {
    return max_len;
  }

  public synchronized void addItem (String s, int i) {
//    System.out.print("Add " + i);
    if (i >= 0 && i < n_items) {
      items.insertElementAt(s, i);
    } else {
      items.addElement(s);
      i = n_items;
    }
    n_items++;
    int l = s.length();
    if (l > max_len) {
      max_len = l;
    }
    if (i >= n0 && i < n0 + rows) {
      if (i < n_items - 1) {
	repaint();
      } else {
	repaintOne(i);
      }
    } else if (i < n0) {
      n0++;
    }
    getParent().postEvent(new Event(this, Event.ACTION_EVENT, "RESHAPE"));
//    System.out.println("@[" + i + "]:" + s);
  }
  
  public synchronized void replaceItem (String s, int i) {
//    System.out.print("Rep " + i);
    boolean reshape = false;
    String old_s = null;
    if (i < 0 || i >= n_items) {
      items.addElement(s);
      i = n_items++;
      reshape = true;
    } else {
      old_s = (String)items.elementAt(i);
      items.setElementAt(s, i);
    }
    int l = s.length();
    if (l > max_len) {
      reshape = true;
      max_len = l;
    } else if (old_s != null && max_len == old_s.length()) {
      Enumeration enum = items.elements();
      reshape = true;
      max_len = 0;
      for (int j = 0; j < n_items; j++) {
	String t = (String)enum.nextElement();
	l = t.length();
	if (l > max_len) {
	  max_len = l;
	}
      }
    }
    if (reshape) {
      getParent().postEvent(new Event(this, Event.ACTION_EVENT, "RESHAPE"));
    }
    if (i >= n0 && i < n0 + rows) {
      repaintOne(i);
    }
//    System.out.println("@[" + i + "]:" + s);
  }

  public synchronized void delItem (int i) {
    if (i >= 0 && i < n_items) {
      if (i == sel) {
	sel = -1;
      }
      String s = (String)items.elementAt(i);
      items.removeElementAt(i);
      n_items--;
      if (sel > i)
	sel--;
      if (s.length() >= max_len) {
	max_len = 0;
	Enumeration enum = items.elements();
	for (int j = 0; j < n_items; j++) {
	  String t = (String)enum.nextElement();
	  int l = t.length();
	  if (l > max_len) {
	    max_len = l;
	  }
	}
      }
      if (i >= n0 && i < n0 + rows) {
	repaint();
      } else if (i < n0) {
	n0--;
      }
      getParent().postEvent(new Event(this, Event.ACTION_EVENT, "RESHAPE"));
//      System.out.println("Del @[" + i + "]:" + s);
    } else {
//      System.out.println("Del @[" + i + "] out of range");
    }
  }

  public synchronized int getSelectedIndex () {
    return sel;
  }

  public synchronized void setN0 (int i) {
    if (i < 0) {
      n0 = 0;
    } else if (i >= n_items) {
      n0 = n_items - 1;
    } else {
      n0 = i;
    }
    repaint();
  }

  public synchronized void setC0 (int i) {
    if (i < 0) {
      c0 = 0;
    } else if (i >= max_len) {
      c0 = max_len - 1;
    } else {
      c0 = i;
    }
    repaint();
  }

  protected void repaintOne (int i) {
    Graphics g = getGraphics();
    String s = (String)items.elementAt(i);
    int len = s.length();
    g.clearRect(PAD, PAD + font_height * (i - n0),
		width - 2 * PAD, font_height);
    if (c0 < len) {
      g.drawString(s.substring(c0, Math.min(c0 + cols, len)),
		   PAD, PAD + font_height * (i - n0) + font_ascent);
    }
    if (i == sel) {
      g.setXORMode(getBackground());
      g.fillRect(PAD, PAD + font_height * (i - n0),
		 width - 2 * PAD, font_height);
      g.setPaintMode();
    }
  }

  public void repaint () {
    Graphics g = getGraphics();
    if (g != null) {
      g.clearRect(1, 1, width - 3, height - 3);
      update(g);
    }
  }

  public void update (Graphics g) {
    paint(g);
  }

  public void paint (Graphics g) {
    g.draw3DRect(0, 0, width - 1, height - 1, false);
    int n = Math.min(n0 + rows, items.size());
    for (int i = n0; i < n; i++) {
      String s = (String)items.elementAt(i);
      int len = s.length();
      if (c0 < len) {
	g.drawString(s.substring(c0, Math.min(c0 + cols, len)), 
		     PAD, PAD + font_height * (i - n0) + font_ascent);
      }
    }
    if (sel >= n0 && sel < n0 + rows) {
      g.setXORMode(getBackground());
      g.fillRect(PAD, PAD + font_height * (sel - n0),
		 width - 2 * PAD, font_height);
      g.setPaintMode();
    }
  }

  public boolean mouseDown (Event e, int x, int y) {
    int i = n0 + (y - PAD) / font_height;
    int old_sel = sel;
    if ((e.modifiers & Event.SHIFT_MASK) != 0 || i < 0 || i >= n_items) {
      sel = -1;
      if (old_sel >= n0 && old_sel < n0 + rows) {
	repaintOne(old_sel);
      }
      return true;
    }
    if (sel != i) {
      sel = i;
      repaint();
    }
    if (e.clickCount > 1 || (e.modifiers & Event.CTRL_MASK) != 0) {
      getParent().postEvent(new Event(this, Event.ACTION_EVENT,
				      new Integer(sel)));
    }
    return true;
  }

}
