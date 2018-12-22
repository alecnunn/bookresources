package client;

import java.awt.*;
import common.*;

class TableInfo extends Frame {
  Client client = null;
  CTable the_table = null;

  Label l_id = new Label("ID#");
  TextField table_id = new TextField(3);
  Button b_info = new Button("Info");

  Label l_options = new Label("Options:");
  Checkbox cb_count_flowers = new Checkbox("Count flowers", null, true);
  Checkbox cb_multiple_wins = new Checkbox("Allow multiple winners", null, true);
  CheckboxGroup cbg_fan = new CheckboxGroup();
  Checkbox cb_no_min = new Checkbox("No minimum fan", cbg_fan, true);
  Checkbox cb_two_min = new Checkbox("2 fan minimum", cbg_fan, false);

  Button b_apply = new Button("Apply");
  Button b_reset = new Button("Reset");
  Padding pad_1 = new Padding(1, 1, false);

  Padding pad_2 = new Padding(1, 1, true);

  Label l_players = new Label("");
  Label players[] = { new Label(""), new Label(""),
		      new Label(""), new Label("") };

  Padding pad_3 = new Padding(1, 1, true);
  
  Label l_mine = new Label("");
  Button b_join = new Button("Join");
  Button b_leave = new Button("Leave");
  Button b_serve = new Button("Serve");

  Padding pad_4 = new Padding(1, 1, true);
  
  Button b_current = new Button("Current");
  Button b_clear = new Button("Clear");
  Button b_close = new Button("Close");

  public TableInfo (Client cli) {
    client = cli;

    GridBagLayout b = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();
    setLayout(b);
    setTitle("IMS: Table Info");
    
    c.fill = GridBagConstraints.HORIZONTAL;
    c.gridheight = 1;
    c.gridwidth = 1;
    b.setConstraints(l_id, c);
    add(l_id);
    l_id.setAlignment(Label.RIGHT);
    b.setConstraints(table_id, c);
    add(table_id);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(b_info, c);
    add(b_info);

    b.setConstraints(l_options, c);
    add(l_options);
    b.setConstraints(cb_count_flowers, c);
    add(cb_count_flowers);
    b.setConstraints(cb_multiple_wins, c);
    add(cb_multiple_wins);
    b.setConstraints(cb_no_min, c);
    add(cb_no_min);
    b.setConstraints(cb_two_min, c);
    add(cb_two_min);

    c.gridwidth = 1;
    b.setConstraints(b_apply, c);
    add(b_apply);
    b_apply.disable();
    b.setConstraints(b_reset, c);
    add(b_reset);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(pad_1, c);
    add(pad_1);

    c.weighty = 1.0;
    b.setConstraints(pad_2, c);
    add(pad_2);

    c.weighty = 0.0;
    b.setConstraints(l_players, c);
    add(l_players);
    for (int i = 0; i < 4; i++) {
      b.setConstraints(players[i], c);
      add(players[i]);
    }
    c.weighty = 1.0;
    b.setConstraints(pad_3, c);
    add(pad_3);

    c.weighty = 0.0;
    b.setConstraints(l_mine, c);
    add(l_mine);
    c.gridwidth = 1;
    b.setConstraints(b_join, c);
    add(b_join);
    b.setConstraints(b_leave, c);
    add(b_leave);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(b_serve, c);
    add(b_serve);
    
    b.setConstraints(pad_4, c);
    add(pad_4);
    
    c.weightx = 1.0;
    c.gridwidth = 1;
    b.setConstraints(b_current, c);
    add(b_current);
    b.setConstraints(b_clear, c);
    add(b_clear);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(b_close, c);
    add(b_close);

    pack();
    refresh(new TableOptions());
  }

  public synchronized void refresh (TableOptions opt) {
    cb_count_flowers.setState(opt.count_flowers);
    cb_multiple_wins.setState(opt.multiple_wins);
    if (opt.min_fan == 0) {
      cbg_fan.setCurrent(cb_no_min);
    } else {
      cbg_fan.setCurrent(cb_two_min);
    }
  }

  public synchronized TableOptions getOptions () {
    TableOptions opt = new TableOptions();
    opt.count_flowers = cb_count_flowers.getState();
    opt.multiple_wins = cb_multiple_wins.getState();
    opt.min_fan = cb_no_min.getState() ? 0 : 2;
    return opt;
  }

  public synchronized CTable getTable () {
    return the_table;
  }

  public synchronized void setTable (CTable t) {
    the_table = t;
    if (t == null) {
      clear();
      return;
    }
    table_id.setText(Integer.toString(t.id));
    refresh(t.opt);
    int n = 0;
    for (int i = 0; i < 4; i++) {
      if (t.players[i] != null) {
	n++;
	players[i].setText("[" + t.players[i].id + "] " + t.players[i].name);
      } else {
	players[i].setText("");
      }
    }
    int s = 0;
    for (int i = 4; i < t.players.length; i++) {
      if (t.players[i] != null)
	s++;
    }
    l_players.setText(n + " players and " + s + " spectators:");
  }

  public synchronized void updateTable (CTable t) {
    if (t == the_table) {
      setTable(t);
    }
  }
  
  public synchronized void updateTable (CTable t, boolean e) {
    if (t == the_table) {
      b_apply.enable(e);
    } else {
      b_apply.disable();
    }
  }

  public synchronized void clear () {
    the_table = null;
    table_id.setText("");
    refresh(new TableOptions());
    l_players.setText("");
    for (int i = 0; i < 4; i++)
      players[i].setText("");
  }
  
  public synchronized void updateMine () {
    if (client.me.table == null) {
      l_mine.setText("You are not on a table.");
      b_join.enable();
      b_serve.enable();
      b_leave.disable();
      b_current.disable();
    } else {
      l_mine.setText("You are on table <" + client.me.table.id + ">.");
      b_join.disable();
      b_serve.disable();
      b_leave.enable();
      b_current.enable();
    }
  }

  public synchronized boolean action (Event e, Object w) {
    if (e.target == b_apply) {
      client.output(Packet.CPTableServe(getOptions()));
    }
    if (e.target == b_reset) {
      if (the_table == null)
	refresh(new TableOptions());
      else 
	refresh(the_table.opt);
    }
    if (e.target == b_close) {
      hide();
    }
    if (e.target == b_join) {
      if (the_table != null)
	client.join_frame.setID(the_table.id);
      else
	client.join_frame.clear();
      client.join_frame.show();
    }
    if (e.target == b_serve) {
      client.output(Packet.CPTableServe(getOptions()));
    }
    if (e.target == b_leave) {
      client.output(Packet.CPTableJoin(Packet.TABLE_LEAVE, (byte)0));
    }
    if (e.target == table_id || e.target == b_info) {
      try {
	int t_id = Integer.parseInt(table_id.getText());
	if (t_id < 0 || t_id >= client.g_tables.length ||
	    client.g_tables[t_id] == null) {
	  clear();
	} else {
	  setTable(client.g_tables[t_id]);
	}
      } catch (NumberFormatException ex) { }
    }
    if (e.target == b_current) {
      setTable(client.me.table);
    }
    if (e.target == b_clear) {
      clear();
    }
    return true;
  }
}

