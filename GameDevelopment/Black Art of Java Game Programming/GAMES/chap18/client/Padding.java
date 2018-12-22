package client;

import java.awt.*;

class Padding extends Canvas {
  int xsize, ysize;
  boolean sep;

  public Padding (int x, int y, boolean s) {
    setForeground(getBackground());
    xsize = x;
    ysize = y;
    sep = s;
  }

  public Dimension preferredSize () {
    return new Dimension(xsize, ysize);
  }

  public Dimension minimumSize() {
    return new Dimension(xsize, ysize);
  }

  public void paint (Graphics g) {
    if (!sep) return;
    g.setColor(Color.black);
    g.drawLine(0, 0, size().width - 1, 0);
  }
}

