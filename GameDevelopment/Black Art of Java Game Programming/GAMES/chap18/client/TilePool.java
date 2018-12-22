package client;

import common.Hand;
import java.awt.*;

class TilePool extends Frame {
  public Button hide = new Button("Hide");
  public Padding pad = new Padding(100, 1, false);
  public Label winds[] = { new Label("(Seat 0)"), new Label("(Seat 1)"),
			   new Label("(Seat 2)"), new Label("(Seat 3)"), };
  public Label names[] = { new Label(""), new Label(""), 
			   new Label(""), new Label(""), };
  public Client client = null;
  public TileWindow outs[] = new TileWindow[4];
  public Hand hands[] = new Hand[4];
  public int dealer = -1;

  public TilePool (Client cli) {
    GridBagLayout b = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    int i = 0;
    setTitle("IMS: Discarded Tile Pool");
    client = cli;
    for (i = 0; i < 4; i++) {
      outs[i] = new TileWindow(426, 2, TileWindow.NORTH, false);
      hands[i] = new Hand(30);
      outs[i].setHand(hands[i]);
    }
    setLayout(b);
    c.fill = GridBagConstraints.NONE;
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridheight = 1;
    c.gridwidth = 1;
    b.setConstraints(pad, c);
    add(pad);
    c.anchor = GridBagConstraints.EAST;
    b.setConstraints(hide, c);
    add(hide);
    c.fill = GridBagConstraints.HORIZONTAL;
    c.gridwidth = 1;
    for (i = 0; i < 4; i++) {
      c.gridheight = 1;
      c.gridx = 0;
      c.gridy = 2 * i + 1;
      c.anchor = GridBagConstraints.SOUTH;
      b.setConstraints(winds[i], c);
      add(winds[i]);
      c.gridy = 2 * i + 2;
      c.anchor = GridBagConstraints.NORTH;
      b.setConstraints(names[i], c);
      add(names[i]);
      c.gridx = 1;
      c.gridy = 2 * i + 1;
      c.gridheight = 2;
      c.anchor = GridBagConstraints.CENTER;
      b.setConstraints(outs[i], c);
      add(outs[i]);
    }
    pack();
  }

  public void setWind (int i, String s) {
    winds[(dealer - i + 4) % 4].setText(s);
  }
  
  public void setName (int i, String s) {
    names[(dealer - i + 4) % 4].setText(s);
  }
  
  public void reset (int s) {
    int i = 0;
    dealer = s;
    for (i = 0; i < 4; i++) {
      outs[i].hand.setLength(0);
      outs[i].repaint();
    }
  }

  public void add (int i, byte t) {
    int j = (dealer - i + 4) % 4;
    hands[j].addTile(t);
    outs[j].repaint();
  }
  
  public void append(int i, Hand h) {
    int j = (dealer - i + 4) % 4;
    hands[j].append(h);
    outs[j].repaint();
  }

  public boolean action (Event evt, Object what) {
    if (evt.target == hide) {
      hide();
      client.Dp.b_pool.setLabel("Show pool");
    }
    return true;
  }
}


