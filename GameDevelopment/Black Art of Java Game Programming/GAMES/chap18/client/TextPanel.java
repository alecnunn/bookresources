package client;

import java.awt.*;

class TextPanel extends Panel {
  TextArea text_area;
  int rows, cols;

  public TextPanel (TextArea ta) {
    BorderLayout b = new BorderLayout();
    setLayout(b);
    b.addLayoutComponent("Center", ta);
    super.add(ta);
    text_area = ta;
    rows = text_area.getRows();
    cols = text_area.getColumns();
  }
  public synchronized void setSize (int r, int c) {
    rows = r;
    cols = c;
    resize(text_area.preferredSize(r, c));
    text_area.resize(text_area.preferredSize(r, c));
  }
  public synchronized Dimension preferredSize () {
    return text_area.preferredSize(rows, cols);
  }
  public synchronized Dimension minimumSize() {
    return text_area.preferredSize(rows, cols);
  }
}
