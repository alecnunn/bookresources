package client;

import java.awt.*;

import common.Packet;

class ChangePasswd extends Frame {
  Client client;
  Label op_lab = new Label("Old Password:");
  Label np_lab = new Label("New Password:");
  Label rp_lab = new Label("Retype Password:");
  Label reply = new Label("");
  TextField old_p = new TextField(20);
  TextField new_p = new TextField(20);
  TextField re_p = new TextField(20);
  Button b_ok = new Button("OK");
  Button b_clear = new Button("Clear");
  Button b_cancel = new Button("Cancel");
  
  public ChangePasswd (Client cli) {
    client = cli;
    Font font = new Font("Courier", Font.PLAIN, 12);
    old_p.setFont(font);
    new_p.setFont(font);
    re_p.setFont(font);
    old_p.setEchoCharacter('?');
    new_p.setEchoCharacter('?');
    re_p.setEchoCharacter('?');
    GridBagLayout b = new GridBagLayout();
    setLayout(b);
    GridBagConstraints c = new GridBagConstraints();
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    b.setConstraints(op_lab, c);
    add(op_lab);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(old_p, c);
    add(old_p);
    c.gridwidth = 1;
    b.setConstraints(np_lab, c);
    add(np_lab);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(new_p, c);
    add(new_p);
    c.gridwidth = 1;
    b.setConstraints(rp_lab, c);
    add(rp_lab);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(re_p, c);
    add(re_p);
    b.setConstraints(reply, c);
    add(reply);
    c.gridwidth = 1;
    b.setConstraints(b_ok, c);
    add(b_ok);
    b.setConstraints(b_clear, c);
    add(b_clear);
    b.setConstraints(b_cancel, c);
    add(b_cancel);
    pack();
    setTitle("IMS: Change Password");
  }
  public synchronized void show () {
    clear();
    super.show();
    super.toFront();
    old_p.requestFocus();
  }
  public synchronized void clear () {
    old_p.setText("");
    new_p.setText("");
    re_p.setText("");
    old_p.requestFocus();
  }    
  public synchronized boolean action (Event e, Object w) {
    if (b_ok == e.target || re_p == e.target) {
      if (!old_p.getText().trim().equals(client.passwd)) {
	reply.setText("Bad password, retry.");
	clear();
	return true;
      }
      String pw = new_p.getText().trim();
      if (!pw.equals(re_p.getText().trim())) {
	reply.setText("New passwords don't match.");
	clear();
	return true;
      }
      client.output(Packet.CPChangePasswd(pw));
      client.passwd = pw;
      hide();
      return true;
    }
    if (old_p == e.target) {
      new_p.requestFocus();
      return true;
    }
    if (new_p == e.target) {
      re_p.requestFocus();
      return true;
    }
    if (b_clear == e.target) {
      clear();
      return true;
    }
    if (b_cancel == e.target) {
      hide();
      return true;
    }
    return true;
  }
}
