package client;

import java.awt.*;

import common.*;

class Desktop extends Panel {
  private Client client = null;
  private TilePool the_pool = null;
  
  public TileWindow tws[] = new TileWindow[4];
  public TileWindow my_priv = null;
  public TileWindow out_tile = null;
  private Padding l_pad = new Padding(200, 1, false);
  private Padding r_pad = new Padding(200, 1, false);
  public Label sides[] = { new Label(""), 
			   new Label(""), 
			   new Label(""),
			   new Label(""),
			 };
  public Label counter = new Label("");
  public Label options = new Label("");
  public Label winds = new Label("");
  public Label prompt_1 = new Label("");
  public Label prompt_2 = new Label("");
  public Notifier urge = new Notifier(60, 72);
  public Button b_urge = new Button("Urge!");

  public Panel l_panel = new Panel();
  public Button b_pool = new Button("Show pool");
  public Button b_sort = new Button("Sort");
  public Button b_pass = new Button("Pass");
  public Button b_conn_up = new Button("Conn > >");
  public Button b_conn_md = new Button("< Conn >");
  public Button b_conn_dn = new Button("< < Conn");
  public Button b_trip = new Button("Triplet");
  public Button b_quad = new Button("Quadruplet");
  public Button b_discard = new Button("Discard");
  public Button b_claim = new Button("MahJong");
  public Button b_newgame = new Button("NEW GAME");
  public Label sep_1 = new Label("   ");
  public Label sep_2 = new Label("   ");
  public Label sep_3 = new Label("   ");

  private boolean pass_is_draw = false;
  private boolean conn_up_on = false;
  private boolean conn_md_on = false;
  private boolean conn_dn_on = false;

  public Desktop (Client cli, TilePool pool) {
    client = cli;
    the_pool = pool;
    
    client.btn_font.register(b_pass);
    client.btn_font.register(b_conn_up);
    client.btn_font.register(b_conn_md);
    client.btn_font.register(b_conn_dn);
    client.btn_font.register(b_trip);
    client.btn_font.register(b_quad);
    client.btn_font.register(b_discard);
    client.btn_font.register(b_claim);
    client.lab_font.register(sides[0]);
    client.lab_font.register(sides[1]);
    client.lab_font.register(sides[2]);
    client.lab_font.register(sides[3]);
    client.lab_font.register(counter);
    client.lab_font.register(winds);
    client.lab_font.register(options);
    client.lab_font.register(prompt_1);
    client.lab_font.register(prompt_2);
    client.lab_font.register(b_urge);
    client.lab_font.register(b_pool);
    client.lab_font.register(b_sort);
    client.lab_font.register(b_newgame);

    GridBagLayout b = new GridBagLayout();
    GridBagLayout lb = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    GridBagConstraints lc = new GridBagConstraints();
    
    setLayout(b);

    // empty padding to make things symmetric
    c.fill = GridBagConstraints.NONE;
    c.gridx = 1;
    c.gridy = 0;
    c.weightx = 1.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    b.setConstraints(l_pad, c);
    add(l_pad);

    // ditto.
    c.gridx = 3;
    c.gridy = 0;
    c.weightx = 1.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    b.setConstraints(r_pad, c);
    add(r_pad);

    // SOUTH tile window
    c.fill = GridBagConstraints.NONE;
    c.anchor = GridBagConstraints.SOUTH;
    c.gridx = 1;
    c.gridy = 7;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 3;
    c.gridheight = 1;
    tws[0] = new TileWindow(11, 2, TileWindow.SOUTH, false);
    b.setConstraints(tws[0], c);
    add(tws[0]);

    // WEST tile window
    c.anchor = GridBagConstraints.WEST;
    c.gridx = 0;
    c.gridy = 1;
    c.weightx = 0.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    c.gridheight = 7;
    tws[1] = new TileWindow(8, 3, TileWindow.WEST, false);
    b.setConstraints(tws[1], c);
    add(tws[1]);

    // NORTH tile window
    c.anchor = GridBagConstraints.NORTH;
    c.gridx = 1;
    c.gridy = 1;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 3;
    c.gridheight = 1;
    tws[2] = new TileWindow(11, 2, TileWindow.NORTH, false);
    b.setConstraints(tws[2],c);
    add(tws[2]);

    // EAST tile window
    c.anchor = GridBagConstraints.EAST;
    c.gridx = 4;
    c.gridy = 1;
    c.weightx = 0.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    c.gridheight = 7;
    tws[3] = new TileWindow(8, 3, TileWindow.EAST, false);
    b.setConstraints(tws[3], c);
    add(tws[3]);

    // private tile window
    c.anchor = GridBagConstraints.NORTH;
    c.gridx = 1;
    c.gridy = 8;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 3;
    c.gridheight = 1;
    my_priv = new TileWindow(14, 1, TileWindow.SOUTH, true);
    my_priv.setHand(client.pri_hand);
    b.setConstraints(my_priv, c);
    add(my_priv);

    // upper prompt
    c.anchor = GridBagConstraints.CENTER;
    c.fill = GridBagConstraints.HORIZONTAL;
    c.gridx = 1;
    c.gridy = 2;
    c.weightx = 0.0;
    c.weighty = 1.0;
    c.gridwidth = 3;
    c.gridheight = 1;
    b.setConstraints(prompt_1, c);
    add(prompt_1);

    // lower prompt
    c.gridx = 1;
    c.gridy = 6;
    c.weightx = 0.0;
    c.weighty = 1.0;
    c.gridwidth = 2;
    c.gridheight = 1;
    b.setConstraints(prompt_2, c);
    add(prompt_2);

    // tile counter
    c.gridx = 1;
    c.gridy = 3;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    b.setConstraints(counter, c);
    add(counter);

    // round wind display
    c.gridx = 1;
    c.gridy = 5;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    b.setConstraints(winds, c);
    add(winds);

    // table options
    c.gridx = 3;
    c.gridy = 6;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    options.setAlignment(Label.RIGHT);
    b.setConstraints(options, c);
    add(options);

    // SOUTH player name
    c.anchor = GridBagConstraints.NORTH;
    c.gridx = 2;
    c.gridy = 5;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 2;
    c.gridheight = 1;
    sides[0].setAlignment(Label.LEFT);
    b.setConstraints(sides[0], c);
    add(sides[0]);

    // WEST player name
    c.anchor = GridBagConstraints.EAST;
    c.gridx = 1;
    c.gridy = 4;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    sides[1].setAlignment(Label.RIGHT);
    b.setConstraints(sides[1], c);
    add(sides[1]);

    // NORTH player name
    c.anchor = GridBagConstraints.SOUTH;
    c.gridx = 2;
    c.gridy = 3;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 2;
    c.gridheight = 1;
    sides[2].setAlignment(Label.LEFT);
    b.setConstraints(sides[2], c);
    add(sides[2]);

    // EAST player name
    c.anchor = GridBagConstraints.WEST;
    c.gridx = 3;
    c.gridy = 4;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    sides[3].setAlignment(Label.LEFT);
    b.setConstraints(sides[3], c);
    add(sides[3]);

    // central discarded tile
    c.anchor = GridBagConstraints.CENTER;
    c.fill = GridBagConstraints.NONE;
    c.gridx = 2;
    c.gridy = 4;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    out_tile = new TileWindow(1, 1, TileWindow.SOUTH, false);
    out_tile.setHand(client.out_hand);
    b.setConstraints(out_tile, c);
    add(out_tile);

    // duke image
    c.anchor = GridBagConstraints.CENTER;
    c.fill = GridBagConstraints.NONE;
    c.gridx = 0;
    c.gridy = 8;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    b.setConstraints(urge, c);
    add(urge);

    // urge button
    c.anchor = GridBagConstraints.CENTER;
    c.fill = GridBagConstraints.NONE;
    c.gridx = 4;
    c.gridy = 8;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    b.setConstraints(b_urge, c);
    add(b_urge);

    // bottom panel of buttons
    c.fill = GridBagConstraints.HORIZONTAL;
    c.gridx = 0;
    c.gridy = 9;
    c.weightx = 1.0;
    c.weighty = 0.0;
    c.gridwidth = GridBagConstraints.REMAINDER;
    c.gridheight = 1;
    b.setConstraints(l_panel, c);

    l_panel.setLayout(lb);
    lc.weightx = 0.0;
    lc.weighty = 0.0;
    lc.gridwidth = 1;
    lc.gridheight = 1;
    lc.fill = GridBagConstraints.BOTH;
    lb.setConstraints(b_pool, lc);
    l_panel.add(b_pool);
    lb.setConstraints(b_sort, lc);
    l_panel.add(b_sort);
    lb.setConstraints(sep_2, lc);
    l_panel.add(sep_2);
    lb.setConstraints(b_pass, lc);
    l_panel.add(b_pass);
    lb.setConstraints(b_conn_up, lc);
    l_panel.add(b_conn_up);
    lb.setConstraints(b_conn_md, lc);
    l_panel.add(b_conn_md);
    lb.setConstraints(b_conn_dn, lc);
    l_panel.add(b_conn_dn);
    lb.setConstraints(b_trip, lc);
    l_panel.add(b_trip);
    lb.setConstraints(b_quad, lc);
    l_panel.add(b_quad);
    lb.setConstraints(b_discard, lc);
    l_panel.add(b_discard);
    lb.setConstraints(b_claim, lc);
    l_panel.add(b_claim);
    lb.setConstraints(sep_3, lc);
    l_panel.add(sep_3);
    lb.setConstraints(b_newgame, lc);
    l_panel.add(b_newgame);
    
    add(l_panel);
  }

  public void dispose () {
    urge.dispose();
  }

  public void urgent () {
    urge.start();
    if (client.sound_on) {
      try {
	client.greeter.play(client.greeter.getDocumentBase(), 
				"audio/doorbell.au");
      } catch (Exception e) {
      }
    }
  }

  public boolean action (Event e, Object w) {
    if (b_pool == e.target) {
      if (b_pool.getLabel().equals("Show pool")) {
	the_pool.show();
	b_pool.setLabel("Hide pool");
      } else {
	the_pool.hide();
	b_pool.setLabel("Show pool");
      }
      return true;
    }
    if (b_sort == e.target) {
      my_priv.hand.sort();
      my_priv.repaint();
      return true;
    }
    return client.action(e, w);
  }
  
  public void SetButtons (int flags) {
    int v = Tile.getSerial(client.OutTileValue());
    if ((flags & (1 << Packet.PLAY_DRAW)) != 0) {
      if (!pass_is_draw) {
	b_pass.setLabel("Draw");
	pass_is_draw = true;
      }
    } else {
      if (pass_is_draw) {
	b_pass.setLabel("Pass");
	pass_is_draw = false;
      }
    }
    b_pass.enable((flags & ((1 << Packet.PLAY_PASS) | (1 << Packet.PLAY_DRAW))) != 0);

    if ((flags & (1 << Packet.PLAY_CONN_UP)) != 0) {
      b_conn_up.setLabel("[" + v + "] " + (v + 1) + " " + (v + 2));
      conn_up_on = true;
      b_conn_up.enable();
    } else {
      if (conn_up_on) {
	b_conn_up.setLabel("Conn > >");
	conn_up_on = false;
      }
      b_conn_up.disable();
    }
    if ((flags & (1 << Packet.PLAY_CONN_MD)) != 0) {
      b_conn_md.setLabel("" + (v - 1) + " [" + v + "] " + (v + 1));
      conn_md_on = true;
      b_conn_md.enable();
    } else {
      if (conn_md_on) {
	b_conn_md.setLabel("< Conn >");
	conn_md_on = false;
      }
      b_conn_md.disable();
    }
    if ((flags & (1 << Packet.PLAY_CONN_DN)) != 0) {
      b_conn_dn.setLabel("" + (v - 2) + " " + (v - 1) + " [" + v + "]");
      conn_dn_on = true;
      b_conn_dn.enable();
    } else {
      if (conn_dn_on) {
	b_conn_dn.setLabel("< < Conn");
	conn_dn_on = false;
      }
      b_conn_dn.disable();
    }
    b_trip.enable((flags & (1 << Packet.PLAY_TRIP)) != 0);
    b_quad.enable((flags & ((1 << Packet.PLAY_QUAD) | (1 << Packet.PLAY_FOUR))) != 0);
    b_claim.enable((flags & (1 << Packet.PLAY_WIN)) != 0);
  }

  public void leave_table () {
    counter.setText("");
    options.setText("");
    winds.setText("");
    b_urge.disable();
    b_sort.disable();
    b_pass.setLabel("Pass");
    b_pass.disable();
    b_conn_up.disable();
    b_conn_md.disable();
    b_conn_dn.disable();
    b_trip.disable();
    b_quad.disable();
    b_discard.disable();
    b_claim.disable();
    b_newgame.disable();
    prompt_1.setText("Please join a table or serve one.");
    prompt_2.setText("");
    the_pool.hide();
    b_pool.setLabel("Show pool");
  }
  
  public void end_game (String s) {
    b_urge.enable();
    b_sort.disable();
    b_pass.setLabel("Pass");
    b_pass.disable();
    pass_is_draw = false;
    b_conn_up.disable();
    b_conn_md.disable();
    b_conn_dn.disable();
    b_trip.disable();
    b_quad.disable();
    b_discard.disable();
    b_claim.disable();
    if (client.isPlaying()) {
      b_newgame.enable();
    } else {
      b_newgame.disable();
    }
    prompt_1.setText(s);
    prompt_2.setText("");
  }
    
  public void mode_discard (String err) {
    my_priv.editable = true;
    b_sort.enable();
    b_pass.disable();
    b_discard.enable();
    b_newgame.disable();
    prompt_1.setText("Your turn to discard a tile.");
    prompt_2.setText(err);
  }

  public void mode_discard_done () {
    my_priv.editable = false;
    b_sort.disable();
    b_quad.disable();
    b_discard.disable();
    b_claim.disable();
    b_newgame.disable();
    prompt_1.setText("");
    prompt_2.setText("");
  }

  public void mode_auction_next (String err) {
    my_priv.editable = true;
    b_sort.enable();
    b_discard.disable();
    b_newgame.disable();
    prompt_1.setText("Tile is out.  Your turn to draw, or grab the tile if you can.");
    prompt_2.setText(err);
  }

  public void mode_auction_other (String err) {
    my_priv.editable = true;
    b_sort.enable();
    b_discard.disable();
    b_newgame.disable();
    prompt_1.setText("Tile is out.  Pass, or grab the tile if you can.");
    prompt_2.setText(err);
  }    

  public void mode_auction_fourth (String notice) {
    my_priv.editable = true;
    b_sort.enable();
    b_discard.disable();
    b_newgame.disable();
    prompt_1.setText(notice);
    prompt_2.setText("");
  }

  public void mode_auction_passed () {
    my_priv.editable = true;
    b_sort.enable();
    b_pass.disable();
    b_conn_up.disable();
    b_conn_md.disable();
    b_conn_dn.disable();
    b_trip.disable();
    b_quad.disable();
    b_claim.disable();
    prompt_1.setText("Waiting for others...");
    prompt_2.setText("");
  }

  public void mode_auction_done () {
    my_priv.editable = false;
    b_sort.disable();
    b_pass.disable();
    b_conn_up.disable();
    b_conn_md.disable();
    b_conn_dn.disable();
    b_trip.disable();
    b_quad.disable();
    b_claim.disable();
    prompt_1.setText("Waiting for others...");
    prompt_2.setText("");
  }

  public void mode_wait (String p1, String p2) {
    my_priv.editable = true;
    b_sort.enable();
    b_pass.disable();
    b_conn_up.disable();
    b_conn_md.disable();
    b_conn_dn.disable();
    b_trip.disable();
    b_quad.disable();
    b_discard.disable();
    b_claim.disable();
    b_newgame.disable();
    prompt_1.setText(p1);
    prompt_2.setText(p2);
  }

  public void mode_wait (String p) {
    mode_wait(p, "");
  }

}
  
