package client;

import java.awt.*;

import common.Packet;

class JoinTable extends Frame {
  Client client;
  Label label = new Label("Join table:");
  TextField id = new TextField(2);
  Choice seat = new Choice();
  Button join = new Button("Join");
  Button cancel = new Button("Cancel");
  
  public JoinTable (Client cli) {
    client = cli;
    setTitle("IMS: Join Table");
    GridBagLayout b = new GridBagLayout();
    setLayout(b);
    GridBagConstraints c = new GridBagConstraints();
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    b.setConstraints(label, c);
    add(label);
    b.setConstraints(id, c);
    add(id);
    c.gridwidth = GridBagConstraints.REMAINDER;
    seat.addItem("Player");
    seat.addItem("Spectator");
    b.setConstraints(seat, c);
    add(seat);
    c.gridwidth = 1;
    b.setConstraints(join, c);
    add(join);
    c.gridx = 2;
    b.setConstraints(cancel, c);
    add(cancel);
    pack();
  }
  public synchronized void clear () {
    id.setText("");
    seat.select(0);
  }
  public synchronized void show () {
    super.show();
    super.toFront();
    id.requestFocus();
  }
  public synchronized void setID (int t_id) {
    id.setText(Integer.toString(t_id));
    seat.select(0);
  }
  public synchronized boolean action (Event e, Object w) {
    if (join == e.target || id == e.target) {
      try {
	int t_id = Integer.parseInt(id.getText());
	if (t_id >= 0) {
	  client.output(Packet.CPTableJoin(t_id, (byte)seat.getSelectedIndex()));
	}
      } catch (NumberFormatException ex) {
      }
      return true;
    }
    if (cancel == e.target) {
      hide();
      return true;
    }
    return true;
  }
}
