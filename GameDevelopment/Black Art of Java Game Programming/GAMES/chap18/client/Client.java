package client;

import java.io.*;
import java.net.Socket;
import java.awt.*;
import java.util.StringTokenizer;

import common.*;

class Client extends Frame implements Runnable {
  Socket sock = null;
  DataInputStream in = null;
  DataOutputStream out = null;
  Greeting greeter = null;
  Thread listener = null;
  SendPing sendping = null;

  int myID = 0;
  public String server_host = null;
  public String name = null;
  public String passwd = null;
  CPlayer me = null;
  CPlayer g_players[] = new CPlayer[Config.max_players];
  CTable g_tables[] = new CTable[Config.max_tables];
  public Hand pub_hands[] = new Hand[4];
  public boolean has_played[] = { true, true, true, true };
  public Hand pri_hand = new Hand(15);
  public Hand out_hand = new Hand(1);
  int turn = 0;
  int round = 0;
  int dealer = 0;
  int my_choices = 0;
  boolean already_quit = false;

  // default preference variables
  boolean sound_on = true;
  boolean split_dt = false;
  boolean tile_large = true;
  public int s_hsize = 45, s_vsize = 5;
  public int b_hsize = 80, b_vsize = 7;
  String lab_name = "Helvetica";
  String msg_name = "Courier";
  String btn_name = "TimesRoman";
  int lab_size = 12;
  int msg_size = 12;
  int btn_size = 14;
  int lab_style = Font.PLAIN;
  int msg_style = Font.PLAIN;
  int btn_style = Font.BOLD;

  // font managers
  public FontManager lab_font;
  public FontManager msg_font;
  public FontManager btn_font;

  // subwidgets
  ScrolledList player_list;
  ScrolledList table_list;
  TextArea msg_text;
  TextPanel msg_wrap;
  Panel msg_panel;
  Label msg_label;
  TextField msg_out;
  Button msg_send;
  Button msg_clear;
  Button msg_purge;
  Choice msg_choice;

  // subpanels
  Desktop Dp = null;
  Login Lp = null;

  // other frames
  JoinTable join_frame = null;
  FingerPlayer finger_frame = null;
  TilePool tile_pool = null;
  TableInfo table_info = null;
  Frame aux_frame = null;
  SetGeometry set_geom = null;
  ChangePasswd new_passwd = null;

  // the menu bar
  MenuBar menu_bar;
  Menu file_menu;
  Menu player_menu;
  Menu table_menu;
  Menu pref_menu;
  Menu help_menu;
  MenuItem m_file_quit;
  MenuItem m_player_passwd;
  MenuItem m_player_finger;
  MenuItem m_table_join;
  MenuItem m_table_leave;
  MenuItem m_table_serve;
  MenuItem m_table_info;
  MenuItem m_table_pool;
  MenuItem m_table_urge;
  MenuItem m_pref_split;
  MenuItem m_pref_sound;
  MenuItem m_pref_tsize;
  MenuItem m_pref_geom;
  MenuItem m_pref_l_font;
  MenuItem m_pref_m_font;
  MenuItem m_pref_b_font;
  MenuItem m_pref_save;
  MenuItem m_help_intro;
  MenuItem m_help_rules;

  public void start_login (Greeting g, Socket s, String host) {
    sock = s;
    server_host = host;
    try {
      in = new DataInputStream(s.getInputStream());
      out = new DataOutputStream(s.getOutputStream());
    } catch (IOException e) {
      g.restart();
      return;
    };
    greeter = g;
    listener = new Thread(this);
    listener.start();

    setTitle("IMS: Login");
    setLayout(new FlowLayout());
    Lp = new Login(this);
    add(Lp);
    pack();
    show();
    Lp.mode_login("Enter name and password to login.");
  }

  public void dispose_all () {
    if (Dp != null)
      Dp.dispose();
    if (tile_pool != null)
      tile_pool.dispose();
    if (join_frame != null)
      join_frame.dispose();
    if (set_geom != null)
      set_geom.dispose();
    if (new_passwd != null)
      new_passwd.dispose();
    if (finger_frame != null)
      finger_frame.dispose();
    if (table_info != null)
      table_info.dispose();
    if (aux_frame != null)
      aux_frame.dispose();
    if (lab_font != null)
      lab_font.dispose();
    if (msg_font != null)
      msg_font.dispose();
    if (btn_font != null)
      btn_font.dispose();
    dispose();
  }

  public void start_game () {
    // start sending ping packets regularly
    sendping = new SendPing(this);
    sendping.start();

    // create the fonts
    lab_font = new FontManager(this, "Label Font", lab_name,
			       lab_style, lab_size);
    msg_font = new FontManager(this, "Message Font", msg_name,
			       msg_style, msg_size);
    btn_font = new FontManager(this, "Button Font", btn_name,
			       btn_style, btn_size);

    // create the tile pool
    tile_pool = new TilePool(this);

    // create the finger player window
    finger_frame = new FingerPlayer(this);

    // create the table info window
    table_info = new TableInfo(this);
    table_info.updateMine();

    // create the join table dialog window
    join_frame = new JoinTable(this);

    // create the geometry setting window
    set_geom = new SetGeometry(this);

    // create the change password window
    new_passwd = new ChangePasswd(this);

    // create the main window
    setTitle("IMS: " + name + "@" + server_host);
    removeAll();
    Lp = null;
    hide();
    GridBagLayout b = new GridBagLayout();
    setLayout(b);
    GridBagConstraints c = new GridBagConstraints();
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 1.0;
    c.gridwidth = 1;
    c.gridheight = 1;
    player_list = new ScrolledList(s_vsize, s_hsize);
    msg_font.register(player_list);
    b.setConstraints(player_list, c);
    add(player_list);

    c.weightx = 1.0;
    c.gridwidth = GridBagConstraints.REMAINDER;
    table_list = new ScrolledList(s_vsize, s_hsize);
    msg_font.register(table_list);
    b.setConstraints(table_list, c);
    add(table_list);

    c.weightx = 0.0;
    c.weighty = 1.0;
    msg_text = new TextArea(b_vsize, b_hsize);
    msg_font.register(msg_text);
    msg_text.setEditable(false);
    msg_wrap = new TextPanel(msg_text);
    b.setConstraints(msg_wrap, c);
    add(msg_wrap);
    c.weighty = 0.0;
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(msg_panel = new Panel(), c);

    GridBagLayout bb = new GridBagLayout();
    GridBagConstraints cc = new GridBagConstraints();
    msg_panel.setLayout(bb);
    cc.fill = GridBagConstraints.BOTH;
    cc.gridwidth = 1;
    bb.setConstraints(msg_label = new Label("Message:"), cc);
    lab_font.register(msg_label);
    msg_panel.add(msg_label);
    cc.weightx = 1.0;
    bb.setConstraints(msg_out = new TextField(60), cc);
    msg_font.register(msg_out);
    msg_panel.add(msg_out);
    cc.weightx = 0.0;
    bb.setConstraints(msg_choice = new Choice(), cc);
    msg_choice.addItem("To all");
    msg_choice.addItem("To table");
    msg_choice.addItem("To god");
    lab_font.register(msg_choice);
    msg_panel.add(msg_choice);
    bb.setConstraints(msg_send = new Button("Send"), cc);
    lab_font.register(msg_send);
    msg_panel.add(msg_send);
    bb.setConstraints(msg_clear = new Button("Clear"), cc);
    lab_font.register(msg_clear);
    msg_panel.add(msg_clear);
    bb.setConstraints(msg_purge = new Button("Purge"), cc);
    lab_font.register(msg_purge);
    msg_panel.add(msg_purge);
    add(msg_panel);

    b.setConstraints(Dp = new Desktop(this, tile_pool), c);
    add(Dp);

    // set the menu bar
    menu_bar = new MenuBar();
    menu_bar.add(file_menu = new Menu("FILE"));
    menu_bar.add(player_menu = new Menu("PLAYER"));
    menu_bar.add(table_menu = new Menu("TABLE"));
    menu_bar.add(pref_menu = new Menu("PREFERENCES"));
    menu_bar.add(help_menu = new Menu("HELP"));
    file_menu.add(m_file_quit = new MenuItem("Quit"));
    player_menu.add(m_player_passwd = new MenuItem("Change Password"));
    player_menu.add(new MenuItem("-"));
    player_menu.add(m_player_finger = new MenuItem("Finger..."));
    table_menu.add(m_table_join = new MenuItem("Join"));
    table_menu.add(m_table_leave = new MenuItem("Leave"));
    m_table_leave.disable();
    table_menu.add(m_table_serve = new MenuItem("Serve"));
    table_menu.add(m_table_info = new MenuItem("Info..."));
    table_menu.add(new MenuItem("-"));
    table_menu.add(m_table_pool = new MenuItem("Show Pool"));
    pref_menu.add(m_pref_split = new MenuItem("Split Desktop"));
    pref_menu.add(m_pref_sound = new MenuItem("Sound: On"));
//    pref_menu.add(m_pref_tsize = new MenuItem("Tiles: large"));
    pref_menu.add(new MenuItem("-"));
    pref_menu.add(m_pref_geom = new MenuItem("Set Geometry"));
    pref_menu.add(m_pref_l_font = new MenuItem("Set Label Font"));
    pref_menu.add(m_pref_m_font = new MenuItem("Set Msg Font"));
    pref_menu.add(m_pref_b_font = new MenuItem("Set Button Font"));
    pref_menu.add(new MenuItem("-"));
    pref_menu.add(m_pref_save = new MenuItem("Save Preferences..."));
    help_menu.add(m_help_intro = new MenuItem("Intro"));
    help_menu.add(m_help_rules = new MenuItem("Rules"));
    menu_bar.setHelpMenu(help_menu);
    setMenuBar(menu_bar);

    pack();
    show();
    
    Dp.leave_table();
  }

  public synchronized void reconfigure () {
    if (aux_frame != null) {
      aux_frame.pack();
    }
//    player_wrap.setSize(s_vsize, s_hsize);
//    table_wrap.setSize(s_vsize, s_hsize);

    player_list.setSize(s_vsize, s_hsize);
    table_list.setSize(s_vsize, s_hsize);
    msg_wrap.setSize(b_vsize, b_hsize);
    pack();
  }

  public CPlayer Index2Player (int i) {
    if (i < 0) {
      return null;
    }
    for (int j = 0; j < g_players.length; j++) {
      CPlayer him = g_players[j];
      if (i == 0 && him != null) {
	return him;
      } else if (him != null) {
	i--;
      }
    }
    return null;
  }

  public int Player2Index (CPlayer him) {
    int i = 0;
    for (int j = 0; j < him.id; j++) {
      if (g_players[j] != null) {
	i++;
      }
    }
    return i;
  }

  public CTable Index2Table (int i) {
    for (int j = 0; j < g_tables.length; j++) {
      CTable t = g_tables[j];
      if (i == 0 && t != null) {
	return t;
      } else if (t != null) {
	i--;
      }
    }
    return null;
  }

  public int Table2Index (CTable t) {
    int i = 0;
    for (int j = 0; j < t.id; j++) {
      if (g_tables[j] != null) {
	i++;
      }
    }
    return i;
  }

  public synchronized boolean handleEvent (Event e) {
    if (e.id == Event.WINDOW_DESTROY) {
      if (Lp != null) {
	output(Packet.CPQuit());
	GB();
	if (listener != null) {
	  listener.stop();
	  listener = null;
	}
	dispose();
	greeter.restart();
      } else {
	output(Packet.CPQuit());
	dispose_all();
	GB();
	if (listener != null) {
	  listener.stop();
	  listener = null;
	}
	if (sendping != null) {
	  sendping.stop();
	  sendping = null;
	}
	greeter.restart();
      }
      return true;
    }
    return super.handleEvent(e);
  }      

  public synchronized boolean action (Event e, Object w) {
    // Menu bar actions
    if (m_file_quit == e.target) {
      output(Packet.CPQuit());
      dispose_all();
      GB();
      if (listener != null) {
	listener.stop();
	listener = null;
      }
      if (sendping != null) {
	sendping.stop();
	sendping = null;
      }
      greeter.restart();
      return true;
    }
    if (m_player_passwd == e.target) {
      new_passwd.show();
      return true;
    }
    if (m_player_finger == e.target) {
      CPlayer him = Index2Player(player_list.getSelectedIndex());
      if (him == null) {
	finger_frame.clear();
      } else {
	finger_frame.setPrompt("Sending finger request...");
	output(Packet.CPFinger(him.id));
      }
      finger_frame.show();
      return true;
    }
    if (m_table_join == e.target) {
      join_frame.clear();
      join_frame.show();
      return true;
    }
    if (m_table_leave == e.target) {
      output(Packet.CPTableJoin(Packet.TABLE_LEAVE, (byte)0));
      return true;
    }
    if (m_table_serve == e.target) {
      output(Packet.CPTableServe(table_info.getOptions()));
      return true;
    }
    if (m_table_info == e.target) {
      CTable t = Index2Table(table_list.getSelectedIndex());
      table_info.setTable(t);
      table_info.show();
      return true;
    }
    if (m_table_pool == e.target) {
      tile_pool.show();
      tile_pool.toFront();
      Dp.b_pool.setLabel("Hide pool");
      return true;
    }
    if (m_pref_split == e.target) {
      if (m_pref_split.getLabel().equals("Split Desktop")) {
	remove(Dp);
	pack();
	aux_frame = new Frame();
	aux_frame.setTitle("IMS: Desktop");
	aux_frame.setLayout(new FlowLayout());
	aux_frame.add(Dp);
	aux_frame.pack();
	aux_frame.show();
	m_pref_split.setLabel("Merge Desktop");
      } else {
	aux_frame.remove(Dp);
	aux_frame.dispose();
	aux_frame = null;
	add(Dp);
	pack();
	m_pref_split.setLabel("Split Desktop");
      }
      return true;
    }
    if (m_pref_sound == e.target) {
      if (sound_on) {
	sound_on = false;
	m_pref_sound.setLabel("Sound: Off");
      } else {
	sound_on = true;
	m_pref_sound.setLabel("Sound: On");
      }
      return true;
    }
/*
    if (m_pref_tsize ==  e.target) {
      if (tile_large) {
	tile_large = false;
	m_pref_tsize.setLabel("Tiles: small");

	for (int i = 0; i < 4; i++) {
	  Dp.tws[i].setGeom(18, 24, 5);
	}
	Dp.my_priv.setGeom(18, 24, 5);
	Dp.out_tile.setGeom(18, 24, 5);
	Dp.layout();
      } else {
	tile_large = true;
	m_pref_tsize.setLabel("Tiles: large");
	for (int i = 0; i < 4; i++) {
	  Dp.tws[i].setGeom(30, 40, 8);
	}
	Dp.my_priv.setGeom(30, 40, 8);
	Dp.out_tile.setGeom(30, 40, 8);
	Dp.layout();
      }
      return true;
    }
*/
    if (m_pref_geom == e.target) {
      set_geom.show();
      return true;
    }
    if (m_pref_l_font == e.target) {
      lab_font.choose();
      return true;
    }
    if (m_pref_m_font == e.target) {
      msg_font.choose();
      return true;
    } 
    if (m_pref_b_font == e.target) {
      btn_font.choose();
      return true;
    }
    if (m_pref_save == e.target) {
      StringBuffer buf = new StringBuffer(100);
      buf.append(s_hsize).append(':');
      buf.append(s_vsize).append(':');
      buf.append(b_hsize).append(':');
      buf.append(b_vsize).append(':');
      buf.append(lab_font.getSize()).append(':');
      buf.append(lab_font.getStyle()).append(':');
      buf.append(msg_font.getSize()).append(':');
      buf.append(msg_font.getStyle()).append(':');
      buf.append(btn_font.getSize()).append(':');
      buf.append(btn_font.getStyle()).append(':');
      buf.append(lab_font.getName()).append(':');
      buf.append(msg_font.getName()).append(':');
      buf.append(btn_font.getName()).append(':');
      output(Packet.CPPreference(buf.toString()));
      return true;
    }
    if (m_help_intro == e.target) {
      greeter.ShowDoc("mj_ims.html");
      msg_text.appendText("!! IMS Introduction is at URL: \"" +
			  greeter.doc_base + "mj_ims.html\".\n");
      return true;
    }
    if (m_help_rules == e.target) {
      greeter.ShowDoc("mj_rules.html");
      msg_text.appendText("!! MahJong Rules is at URL: \"" + 
			  greeter.doc_base + "mj_rules.html\".\n");
      return true;
    }
    // Player list
    if (player_list == e.target) {
      CPlayer him = Index2Player(((Integer)e.arg).intValue());
      if (him != null) {
	finger_frame.setPrompt("Sending finger request...");
	finger_frame.show();
	output(Packet.CPFinger(him.id));
      }
      return true;
    }
    // Table list
    if (table_list == e.target) {
      CTable t = Index2Table(((Integer)e.arg).intValue());
      table_info.setTable(t);
      table_info.show();
      return true;
    }
    // Message
    if (msg_send == e.target || msg_out == e.target) {
      if ("To table".equals(msg_choice.getSelectedItem())) {
	output(Packet.CPMessage(Packet.MSG_TABLE, msg_out.getText()));
      } else if ("To all".equals(msg_choice.getSelectedItem())) {
	output(Packet.CPMessage(Packet.MSG_ALL, msg_out.getText()));
      } else {
	output(Packet.CPMessage(Packet.MSG_GOD, msg_out.getText()));
      }
      msg_out.setText("");
      return true;
    }
    if (msg_clear == e.target) {
      msg_out.setText("");
      return true;
    }
    if (msg_purge == e.target) {
      msg_text.setText("");
      return true;
    }
    // Login window actions
    if (Lp != null) {
      if (Lp.login_new == e.target) {
	output(Packet.CPLogin(Packet.LOGIN_NEW, 
			      name = Lp.name_field.getText().trim(), 
			      passwd = Lp.pass_field.getText().trim()));
	Lp.mode_wait();
	return true;
      }
      if (Lp.login_old == e.target || Lp.pass_field == e.target) {
	output(Packet.CPLogin(Packet.LOGIN_OLD, 
			      name = Lp.name_field.getText().trim(),
			      passwd = Lp.pass_field.getText().trim()));
	Lp.mode_wait();
	return true;
      }
      if (Lp.login_cancel == e.target) {
	GB();
	if (listener != null) {
	  listener.stop();
	  listener = null;
	}
	dispose();
	greeter.restart();
	return true;
      }
    }
    // Desktop panel actions
    if (Dp != null) {
      if (Dp.b_urge == e.target) {
	output(Packet.CPUrge());
	return true;
      }
      if (Dp.b_newgame == e.target) {
	Dp.b_newgame.disable();
	Dp.prompt_1.setText("Waiting for other players...");
	output(Packet.CPPlay(Packet.PLAY_PASS, (byte)0));
	return true;
      }
      if (Dp.b_pass == e.target) {
	if (Dp.b_pass.getLabel().equals("Draw"))
	  output(Packet.CPPlay(Packet.PLAY_DRAW, (byte)0));
	else
	  output(Packet.CPPlay(Packet.PLAY_PASS, (byte)0));
	Dp.mode_auction_passed();
	return true;
      }
      if (Dp.b_conn_up == e.target) {
	byte v = OutTileValue();
	pri_hand.unselectAll();
	pri_hand.searchN(Tile.stepUp(v), 1);
	pri_hand.searchN(Tile.stepUp(Tile.stepUp(v)), 1);
	Dp.my_priv.repaint();
	output(Packet.CPPlay(Packet.PLAY_CONN_UP, (byte)0));
	Dp.mode_auction_done();
	return true;
      }
      if (Dp.b_conn_md == e.target) {
	byte v = OutTileValue();
	pri_hand.unselectAll();
	pri_hand.searchN(Tile.stepUp(v), 1);
	pri_hand.searchN(Tile.stepDn(v), 1);
	Dp.my_priv.repaint();
	output(Packet.CPPlay(Packet.PLAY_CONN_MD, (byte)0));
	Dp.mode_auction_done();
	return true;
      }
      if (Dp.b_conn_dn == e.target) {
	byte v = OutTileValue();
	pri_hand.unselectAll();
	pri_hand.searchN(Tile.stepDn(v), 1);
	pri_hand.searchN(Tile.stepDn(Tile.stepDn(v)), 1);
	Dp.my_priv.repaint();
	output(Packet.CPPlay(Packet.PLAY_CONN_DN, (byte)0));
	Dp.mode_auction_done();
	return true;
      }
      if (Dp.b_trip == e.target) {
	byte v = OutTileValue();
	pri_hand.unselectAll();
	pri_hand.searchN(v, 2);
	Dp.my_priv.repaint();
	output(Packet.CPPlay(Packet.PLAY_TRIP, (byte)0));
	Dp.mode_auction_done();
	return true;
      }
      if (Dp.b_quad == e.target) {
	byte v = OutTileValue();
	if (v == 0) {                 // self-quad
	  Hand sel = pri_hand.selection(false);
	  if (sel.length() == 0) {
	    Dp.prompt_2.setText("You must select a tile!");
	    return true;
	  }
	  v = sel.tileAt(0);
	  pri_hand.unselectAll();
	  pri_hand.searchN(v, 4);
	  Dp.my_priv.repaint();
	  output(Packet.CPPlay(Packet.PLAY_FOUR, v));
	  Dp.mode_discard_done();
	} else {                      // quad by grabbing
	  pri_hand.unselectAll();
	  pri_hand.searchN(v, 3);
	  Dp.my_priv.repaint();
	  output(Packet.CPPlay(Packet.PLAY_QUAD, (byte)0));
	  Dp.mode_auction_done();
	}
	return true;
      }
      if (Dp.b_discard == e.target) {
	Hand sel = pri_hand.selection(false);
	if (sel.length() != 1) {
	  Dp.prompt_2.setText("You must discard 1 and only 1 tile!");
	  return true;
	}
	output(Packet.CPPlay(Packet.PLAY_DISCARD, sel.tileAt(0)));
	Dp.mode_discard_done();
	return true;
      }
      if (Dp.b_claim == e.target) {
	output(Packet.CPPlay(Packet.PLAY_WIN, (byte)0));
	if (out_hand.length() == 0) {  
  	  Dp.mode_discard_done();
	} else {
	  Dp.mode_auction_done();
	}
	return true;
      }
    }
    return true;
  } 
  
  public synchronized void output (byte[] p) {
    try {
      out.write(p);
    } catch (IOException e) {
      GB();
      System.out.println("GB on output.");
      if (listener != null) {
	listener.stop();
	listener = null;
      }
      if (sendping != null) {
	sendping.stop();
	sendping = null;
      }
      dispose_all();
      greeter.restart();
    }
  }

  public void GB () {
    System.out.println("Doing GB...");
    try {
      in.close();
      out.close();
      sock.close();
    } catch (IOException e) {
    }
  }

  public void CountTiles () {
    int n = 0, i = 0;
    CTable t = me.table;
    for (i = 0; i < 4; i++) {
      if (i == me.seat)
	n += pri_hand.length();
      n += pub_hands[i].length();
      n += tile_pool.hands[i].length();
    }
    n += out_hand.length();
    n = 144 - n;
    Dp.counter.setText("Tiles remaining: " + n);
  }

  public byte OutTileValue () {
    if (out_hand.length() == 0)
      return 0;
    else
      return out_hand.tileAt(0);
  }

  public boolean isPlaying () {
    if (me.table == null)
      return false;
    if (me.seat < 0 || me.seat > 3)
      return false;
    return true;
  }

  public boolean isDealer () {
    if (me.table == null)
      return false;
    if (me.seat == dealer)
      return true;
    return false;
  }

  public int mapSeat (int s) {
    if ((me.seat >= 0 ) && (me.seat < 4)) {
      return (4 + s - me.seat) % 4;
    }
    return (4 + s - dealer) % 4;
  }

  public int seatWind (int s) {
    return (dealer - s + 4) % 4;
  }

  public int iMapSeat (int s) {
    if ((me.seat >= 0) && (me.seat < 4)) {
      return (s + me.seat) % 4;
    }
    return (4 + dealer - s) % 4;
  }

  public void SetSideLabel (int s) {
    if (me.table == null) {
      Dp.sides[s].setText("");
      tile_pool.setWind(s, "(Seat " + s + ")");
      tile_pool.setName(s, "");
      return;
    }
    int t = mapSeat(s);
    if (me.table.players[s] == null) {
      Dp.sides[t].setText("(Seat " + s + ")");
      tile_pool.setWind(s, "(Seat " + s + ")");
      tile_pool.setName(s, "");
      return;
    }
    CPlayer him = me.table.players[s];
    if (t == 1) {
      Dp.sides[t].setText("<" + him.balance + ">  " + him.name + "  (" + 
			  Tile.wind_names[seatWind(s)] + ")");
    } else {
      Dp.sides[t].setText("(" + Tile.wind_names[seatWind(s)] + 
			  ")  " + him.name + "  <" + him.balance + ">");
    }
    tile_pool.setWind(s, Tile.wind_names[seatWind(s)]);
    tile_pool.setName(s, him.name);
  }

  public String SeatPlayerName (int s) {
    if (me.table == null)
      return null;
    if (me.table.players[s] == null) {
      return ("(Seat " + s + ")");
    }
    return me.table.players[s].name;
  }

  void addPlayerText (CPlayer him) {
    player_list.addItem(him.toString(), Player2Index(him));
  }

  void updatePlayerText (CPlayer him) {
    player_list.replaceItem(him.toString(), Player2Index(him));
  }

  void removePlayerText (CPlayer him) {
    player_list.delItem(Player2Index(him));
  }

  void addTableText (CTable t) {
    table_list.addItem(t.toString(), Table2Index(t));
  }

  void updateTableText (CTable t) {
    table_list.replaceItem(t.toString(), Table2Index(t));
  }

  void removeTableText (CTable t) {
    table_list.delItem(Table2Index(t));
  }

  public void run () {
    byte type = 0;
  loop:
    for (;;) {
      try {
//	System.out.println("Reading...");
	type = in.readByte();
//	System.out.println("Packet[" + type + "]");
	switch (type) {
	case Packet.SP_URGE:
	  Dp.urgent();
	  break;
	case Packet.SP_YOU_ARE:
	  hdYouAre();
	  break;
	case Packet.SP_LOGIN:
	  hdLogin();
	  break;
	case Packet.SP_JOIN:
	  hdJoin();
	  break;
	case Packet.SP_LEAVE:
	  hdLeave();
	  break;
	case Packet.SP_TABLE_OPT:
	  hdTableOpt();
	  break;
	case Packet.SP_TABLE_PLAYER:
	  hdTablePlayer();
	  break;
	case Packet.SP_MESSAGE:
	  hdMessage();
	  break;
	case Packet.SP_GAME:
	  hdGame();
	  break;
	case Packet.SP_TURN:
	  hdTurn();
	  break;
	case Packet.SP_STAT:
	  hdStat();
	  break;
	case Packet.SP_DISCARD:
	  hdDiscard();
	  break;
	case Packet.SP_TILES:
	  hdTiles();
	  break;
	case Packet.SP_YOUR_PLAY:
	  hdYourPlay();
	  break;
	case Packet.SP_YOUR_PRIVATE:
	  hdYourPrivate();
	  break;
	case Packet.SP_YOUR_DRAW:
	  hdYourDraw();
	  break;
	case Packet.SP_OUT_TILES:
	  hdOutTiles();
	  break;
	case Packet.SP_WIN:
	  hdWin();
	  break;
	case Packet.SP_WHO_PLAYED:
	  hdWhoPlayed();
	  break;
	case Packet.SP_YOUR_CHOICES:
	  hdYourChoices();
	  break;
	case Packet.SP_PREFERENCE:
	  hdPref();
	  break;
	case Packet.SP_FINGER:
	  hdFinger();
	  break;
	default:
	  System.out.println("Unrecognizable server packet " + type);
	  GB();
	  break loop;
	}
      }
      catch (IOException e) {
	GB();
	System.out.println("GB on input.");
	break loop;
      }
    }
    if (sendping != null) {
      sendping.stop();
      sendping = null;
    }
    dispose_all();
    listener = null;
    greeter.restart();
  }
  
  void hdYouAre () throws IOException {
    myID = (int)in.readByte();
    me = new CPlayer();
    me.id = myID;
  }
  
  void hdLogin () throws IOException {
    byte result = in.readByte();
    
    switch (result) {
    case Packet.LOGIN_OK:
      start_game();
      return;
    case Packet.LOGIN_TWICE:
      Lp.mode_login("You are already logged in.");
      break;
    case Packet.LOGIN_BADPASS:
      Lp.mode_login("Bad password.");
      break;
    case Packet.LOGIN_EXIST:
      Lp.mode_login("That player already exists.");
      break;
    case Packet.LOGIN_NONEXIST:
      Lp.mode_login("No such player.");
      break;
    }
  }

  void hdJoin () throws IOException {
    int p_id = (int)in.readByte();
    String name = in.readUTF();
    String host = in.readUTF();
    int bal = in.readInt();
    boolean join = in.readBoolean();

    CPlayer him;
    if (p_id == myID) {
      g_players[p_id] = him = me;
    } else {
      g_players[p_id] = him = new CPlayer();
      him.id = p_id;
    }
    him.name = name;
    him.host = host;
    him.balance = bal;
    if (join) {
      msg_text.appendText(">> " + him.name + " (@" + him.host +
			  ") joining the server.\n");
    }
    addPlayerText(him);
  }
  
  void hdLeave () throws IOException {
    int p_id = (int)in.readByte();
    int reason = (int)in.readByte();
    
    CPlayer him = g_players[p_id];
    if (him != null) {
      removePlayerText(him);
      msg_text.appendText(">> " + him.name + " (@" + him.host + 
			  ") left the server (" + 
			  Packet.leave_reasons[reason] + ").\n");
    }
    g_players[p_id] = null;
  }
  
  void hdTableOpt () throws IOException {
    int t_id = (int)in.readByte();
    TableOptions opts = new TableOptions(in);
    
    CTable t = g_tables[t_id];
    if (t == null) {
      t = new CTable();
      g_tables[t_id] = t;
      t.id = t_id;
      t.opt = opts;
      addTableText(t);
    } else {
      t.opt = opts;
      updateTableText(t);
      table_info.updateTable(t);
    }
    if (t == me.table) 
      Dp.options.setText("Table <" + t_id + "> " + t.opt.toString());
  }
  
  void hdTablePlayer () throws IOException {
    int t_id = (int)in.readByte();
    int seat = (int)in.readByte();
    int p_id = (int)in.readByte();
    
    CTable t = g_tables[t_id];
    CPlayer him = null;

    if (t == null) {
      System.out.println("Table " + t_id + ", no such table");
      return;
    }
    if (p_id == Packet.TABLE_LEAVE) {  // leaving table
      him = t.players[seat];
      if (him == null) {
	System.out.println("Table " + t_id + ", seat " + seat + 
			   ": no such player.");
	return;
      }
      him.table = null;
      him.seat = -1;
      t.players[seat] = null;
      updatePlayerText(him);

      if (him == me) {
	for (int s = 0; s < 4; s++) {
	  pub_hands[s] = null;
	  Dp.tws[s].setHand(null);
	  Dp.tws[s].setTurn(false);
	  SetSideLabel(s);
	}
	Dp.options.setText("");
	Dp.winds.setText("");
	pri_hand.setLength(0);
	Dp.my_priv.repaint();
	out_hand.setLength(0);
	Dp.out_tile.repaint();
	Dp.leave_table();	
	table_info.updateMine();
	table_info.updateTable(t, false);
	m_table_join.enable();
	m_table_leave.disable();
	m_table_serve.enable();
      } else if (seat < 4 && t == me.table) {
	SetSideLabel(seat);
	pub_hands[seat].setLength(0);
	Dp.tws[mapSeat(seat)].repaint();
      }
    is_empty:
      {
	for (int s = 0; s < t.players.length; s++) {
	  if (t.players[s] != null)
	    break is_empty;
	}
	removeTableText(t);
	if (t == table_info.getTable()) {
	  table_info.setTable(null);
	}
	g_tables[t_id] = null;
	return;
      }
      updateTableText(t);
      table_info.updateTable(t);
    } else {                               // joining table
      him = g_players[p_id];
      him.table = t;
      him.seat = seat;
      t.players[seat] = him;
      updateTableText(t);
      table_info.updateTable(t);
      updatePlayerText(him);

      if (him == me) {                     // it's me joining
	for (int s = 0; s < 4; s++) {
	  pub_hands[s] = new Hand(15);
	  SetSideLabel(s);
	}
	join_frame.hide();
	table_info.setTable(t);
	table_info.updateMine();
	if (seat == 0) {
	  table_info.updateTable(t, true);
	} else {
	  table_info.updateTable(t, false);
	}
	m_table_join.disable();
	m_table_leave.enable();
	m_table_serve.disable();
	Dp.options.setText("Table <" + t.id + "> " + t.opt.toString());
	if (isPlaying()) {
	  Dp.end_game("Select \"New Game\" when ready.");
	} else {
	  Dp.mode_wait("", "You are a spectator.");
	}
      } else if (seat < 4) {
	if (t == me.table) {
	  SetSideLabel(seat);
	}
      }
    }
  }
  
  void hdMessage () throws IOException {
    byte type = in.readByte();
    int p_id = (int)in.readByte();
    String text = in.readUTF();

    CPlayer him = null;
    if (type == Packet.MSG_ALL) {
      him = g_players[p_id];
      if (him == null) {
	msg_text.appendText("???->All: " + text + '\n');
      } else {
	msg_text.appendText('[' + him.name + "]->All: " + text + '\n');
      }
    } else if (type == Packet.MSG_TABLE) {
      him = g_players[p_id];
      if (him == null) {
	msg_text.appendText("???->Table: " + text + '\n');
      } else {
	msg_text.appendText('[' + him.name + "]->Table: " + text + '\n');
      }
    } else {
      if (p_id == 1) {
	Dp.urgent();
      }
      msg_text.appendText(">> " + text + '\n');
    }
  }

  void hdGame () throws IOException {
    int p_id = (int)in.readByte();
    int g_round = (int)in.readByte();
    int g_seat = (int)in.readByte();
    byte action = in.readByte();

    switch (action) {
    case Packet.GAME_START:
    case Packet.GAME_PROGRESS:
      round = g_round;
      dealer = g_seat;
      out_hand.setLength(0);
      Dp.out_tile.repaint();
      tile_pool.reset(dealer);
      for (int s = 0; s < 4; s++) {
	Dp.tws[mapSeat(s)].setHand(pub_hands[s]);
	Dp.tws[s].setTurn(false);
	SetSideLabel(s);
      }
      msg_text.appendText("*** " +
			  (action == Packet.GAME_START ?
			   "New game started, " :
			   "Game in progress, ") +
			  (dealer == me.seat ? 
			   "you are the dealer.\n" : 
			   (me.table.players[dealer].name + 
			    " is the dealer.\n")));
      table_info.updateTable(me.table, false);
      Dp.winds.setText("Round Wind:  " + Tile.wind_names[round]);
      break;
    case Packet.GAME_END:
      if (isPlaying()) {
	msg_text.appendText("*** Game ended.  Press \"New Game\" now.\n");
	Dp.end_game("Press \"New Game\" when ready.");
      } else {
	msg_text.appendText("*** Game ended.\n");
	Dp.end_game("");
      }
      if (me.seat == 0) {
	table_info.updateTable(me.table, true);
      } else {
	table_info.updateTable(me.table, false);
      }
      my_choices = 0;
      Dp.SetButtons(my_choices);
      break;
    }
  }

  void hdTurn () throws IOException {
    byte action = in.readByte();
    int seat = (int)in.readByte();

    if (turn != -1) {
      Dp.tws[mapSeat(turn)].setTurn(false);
      Dp.tws[mapSeat(CTable.nextTurn(turn))].setTurn(false);
      if (action == Packet.TURN_DRAW) {
	if (out_hand.length() > 0)
	  tile_pool.add(turn, out_hand.tileAt(0));
      }
    }
    turn = seat;
    Dp.tws[mapSeat(turn)].setTurn(true);
    out_hand.setLength(0);
    Dp.out_tile.repaint();
    if (turn == me.seat) {
      if (action == Packet.TURN_GRAB) {
	if (pri_hand.delete())
	  Dp.my_priv.repaint();
      }
      Dp.mode_discard("");
    } else {
      if (action == Packet.TURN_GRAB) {
	pri_hand.unselectAll();
	Dp.my_priv.repaint();
      }
      if (action == Packet.TURN_GRAB) {
	Dp.mode_wait(SeatPlayerName(turn) + 
		  " grabbed the tile, will discard...");
      }
      if (action == Packet.TURN_DRAW) {
	Dp.mode_wait(SeatPlayerName(turn) + 
		  " drew a tile, will discard...");
      }
      if (action == Packet.TURN_NONE) {
	Dp.mode_wait(SeatPlayerName(turn) + "'s turn to discard.");
      }
      if (action == Packet.TURN_FOURTH) {
	Dp.mode_wait(SeatPlayerName(turn) + "'s turn to discard.");
      }
    }
    CountTiles();
  }

  void hdStat () throws IOException {
    int p_id = (int)in.readByte();
    int bal = in.readInt();

    CPlayer him = g_players[p_id];
    him.balance = bal;
    updatePlayerText(him);
    if (me != null && me.table != null && me.table == him.table) {
      int s = 0;
      for (s = 0; s < 4; s++) {
	if (me.table.players[s] == him) {
	  SetSideLabel(s);
	}
      }
    }
  }

  void hdDiscard () throws IOException {
    byte tile = in.readByte();
    byte action = in.readByte();

    if (action == Packet.DISCARD_DISCARD) {
      Dp.tws[mapSeat(turn)].setTurn(false);
      Dp.tws[mapSeat(CTable.nextTurn(turn))].setTurn(true);
      out_hand.addTile(tile);
      Dp.out_tile.repaint();
      if (isPlaying()) {
	if (me.seat == turn) {
	  if (pri_hand.delete())
	    Dp.my_priv.repaint();
	  Dp.mode_wait("You just discarded:");
	} else {
	  if (CTable.nextTurn(turn) == me.seat) {
	    Dp.mode_auction_next("");
	  } else {
	    Dp.mode_auction_other("");
	  }
	}
      } else {
	Dp.mode_wait(SeatPlayerName(turn) + " discarded:");
      }
    } else {  // FOURTH
      out_hand.addTile(tile);
      Dp.out_tile.repaint();
      if (isPlaying()) {
	if (me.seat == turn) {
	  if (pri_hand.delete())
	    Dp.my_priv.repaint();
	  Dp.mode_wait("You just made a quadruplet, waiting for others...");
	} else {
	  Dp.mode_auction_fourth(SeatPlayerName(turn) + 
			      " made a quadruplet, you may MahJong if you can.");
	}
      } else {
	Dp.mode_wait(SeatPlayerName(turn) + " made a quadruplet:");
      }
    }
    CountTiles();
  }

  void hdTiles () throws IOException {
    int p_id = (int)in.readByte();
    int seat = (int)in.readByte();
    int n_priv = (int)in.readByte();
    Hand hand = Packet.readHand(in, null);    
    
    int i = 0;
    CTable t = me.table;
    pub_hands[seat].setLength(0);
    if (seat != me.seat) {
      for (i = 0; i < n_priv; i++) {
	pub_hands[seat].addTile((byte)0);
      }
    }
    pub_hands[seat].append(hand);
    Dp.tws[mapSeat(seat)].repaint();
  }

  void hdYourPlay () throws IOException {
    byte action = in.readByte();
    boolean ok = in.readBoolean();
    int reason = (int)in.readByte();

    switch (action) {
    case Packet.YPLAY_BID:
      if (ok) {
	if (pri_hand.delete())
	  Dp.my_priv.repaint();
      } else {
	if (CTable.nextTurn(turn) == me.seat) {
	  Dp.mode_auction_next(Packet.play_errors[reason]);
	} else {
	  Dp.mode_auction_other(Packet.play_errors[reason]);
	}
	Dp.SetButtons(my_choices);
      }
      break;
    case Packet.YPLAY_DISCARD:
      if (ok) {
	if (pri_hand.delete())
	  Dp.my_priv.repaint();
	Dp.mode_discard("");
      } else {
	Dp.mode_discard(Packet.play_errors[reason]);
	Dp.SetButtons(my_choices);
      }
      break;
    case Packet.YPLAY_CLAIM:
      if (ok) {
	pri_hand.setLength(0);
	Dp.my_priv.repaint();
      } else {
	if (out_hand.length() == 0) {
	  Dp.mode_discard(Packet.play_errors[reason]);
	} else {
	  if (CTable.nextTurn(turn) == me.seat) {
	    Dp.mode_auction_next(Packet.play_errors[reason]);
	  } else {
	    Dp.mode_auction_other(Packet.play_errors[reason]);
	  }
	}
      }
    }
  }
  
  void hdYourPrivate () throws IOException {
    Packet.readHand(in, pri_hand);
    Dp.my_priv.repaint();
  }

  void hdYourDraw () throws IOException {
    byte tile = in.readByte();

    pri_hand.addTile(tile);
    Dp.my_priv.repaint();
  }

  void hdOutTiles () throws IOException {
    int p_id = (int)in.readByte();
    int seat = (int)in.readByte();
    Hand hand = Packet.readHand(in, null);

    tile_pool.append(seat, hand);
  }

  void hdWin () throws IOException {
    int p_id = (int)in.readByte();
    int seat = (int)in.readByte();
    int score = in.readInt();
    int feeder = (int)in.readByte();

    if (p_id != -1) {
      msg_text.appendText("*** " + SeatPlayerName(seat) + 
			  " has won this hand, " + 
			  (feeder == -1 ? "self-feeding, " : 
			   SeatPlayerName(feeder) + " feeding, ") +
			  "score is " + score + " points.\n");
    } else {
      msg_text.appendText("*** This hand is a draw.\n");
    }
  }
  
  void hdWhoPlayed () throws IOException {
    int flags = (int)in.readByte();
/*
    int s = 0;
    CTable t = me.table;
    for (s = 0; s < 4; s++) {
      has_played[s] = ((flags & (1 << s)) != 0);
      SetSideLabel(s);
    }
*/
  }

  void hdYourChoices () throws IOException {
    int flags = in.readInt();
    int win_score = in.readInt();

    my_choices = flags;
    Dp.SetButtons(flags);
    if (win_score != -1) {
      Dp.winds.setText("Round Wind:  " + Tile.wind_names[round] +
		       "    Score:  " + win_score);
    } else {
      Dp.winds.setText("Round Wind:  " + Tile.wind_names[round]);
    }      
  }

  void hdPref () throws IOException {
    String pref = in.readUTF();
    
    StringTokenizer st = new StringTokenizer(pref, ":");
    int S_hsize, S_vsize, B_hsize, B_vsize;
    int Lab_size, Lab_style, Msg_size, Msg_style, Btn_size, Btn_style;
    String Lab_name, Msg_name, Btn_name;
    try {
      S_hsize = Integer.parseInt(st.nextToken());
      S_vsize = Integer.parseInt(st.nextToken());
      B_hsize = Integer.parseInt(st.nextToken());
      B_vsize = Integer.parseInt(st.nextToken());
      Lab_size = Integer.parseInt(st.nextToken());
      Lab_style = Integer.parseInt(st.nextToken());
      Msg_size = Integer.parseInt(st.nextToken());
      Msg_style = Integer.parseInt(st.nextToken());
      Btn_size = Integer.parseInt(st.nextToken());
      Btn_style = Integer.parseInt(st.nextToken());
      Lab_name = st.nextToken();
      Msg_name = st.nextToken();
      Btn_name = st.nextToken();
    } catch (Exception e) {
      System.out.println(e);
      return;
    }
    s_hsize = S_hsize;
    s_vsize = S_vsize;
    b_hsize = B_hsize;
    b_vsize = B_vsize;
    lab_name = Lab_name;
    lab_size = Lab_size;
    lab_style = Lab_style;
    msg_name = Msg_name;
    msg_size = Msg_size;
    msg_style = Msg_style;
    btn_name = Btn_name;
    btn_size = Btn_size;
    btn_style = Btn_style;
  }

  void hdFinger () throws IOException {
    int p_id = (int)in.readByte();
    String name = in.readUTF();
    String mail = in.readUTF();
    String home = in.readUTF();
    String plan = in.readUTF();

    finger_frame.set(p_id, name, mail, home, plan, p_id == myID);
  }
}
