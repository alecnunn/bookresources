package client;

import java.awt.*;

class ScrolledList extends Panel {
  protected SimpleList list;
  protected Scrollbar hbar, vbar;

  public ScrolledList (int r, int c) {
    super();
    list = new SimpleList(r, c);
    hbar = new Scrollbar(Scrollbar.HORIZONTAL, 0, c, 0, 0);
    vbar = new Scrollbar(Scrollbar.VERTICAL, 0, r, 0, 0);
    BorderLayout layout = new BorderLayout();
    setLayout(layout);
    layout.addLayoutComponent("South", hbar);
    layout.addLayoutComponent("East", vbar);
    layout.addLayoutComponent("Center", list);
    add(list);
    add(hbar);
    add(vbar);
  }

  public void setSize (int r, int c) {
    list.setSize(r, c);
    hbar.setPageIncrement(c);
    vbar.setPageIncrement(r);
  }
  
  public void setFont (Font font) {
    list.setFont(font);
  }

  protected void adjustScrollbars () {
    int c = list.getCols();
    int r = list.getRows();
    vbar.setValues(vbar.getValue(), r, 0, Math.max(list.countItems() - r, 0));
    vbar.setPageIncrement(r);
    hbar.setValues(hbar.getValue(), c, 0, Math.max(list.maxLength() - c, 0));
    hbar.setPageIncrement(c);
    System.out.println("len: " + list.maxLength());
  }    

  public void addItem (String s, int i) {
    list.addItem(s, i);
  }

  public void replaceItem (String s, int i) {
    list.replaceItem(s, i);
  }

  public void delItem (int i) {
    list.delItem(i);
  }
  
  public int getSelectedIndex () {
    return list.getSelectedIndex();
  }

  public synchronized boolean handleEvent (Event e) {
    switch (e.id) {
    case Event.SCROLL_LINE_UP:
    case Event.SCROLL_LINE_DOWN:
    case Event.SCROLL_PAGE_UP:
    case Event.SCROLL_PAGE_DOWN:
    case Event.SCROLL_ABSOLUTE:
      if (e.target == hbar) {
	list.setC0(((Integer)e.arg).intValue());
      }
      if (e.target == vbar) {
	list.setN0(((Integer)e.arg).intValue());
      }
      return true;
    }
    if (e.target == list) {
      if (e.id == Event.ACTION_EVENT) {
	if (e.arg instanceof String && ((String)e.arg).equals("RESHAPE")) {
	  adjustScrollbars();
	  return true;
	}
	if (e.arg instanceof Integer) {
	  e.target = this;
	  return super.handleEvent(e);
	}
      }
    }
    return super.handleEvent(e);
  }

}

