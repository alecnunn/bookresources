package client;

import java.awt.*;

import common.Packet;

class FingerPlayer extends Frame {
  Client client;

  Label prompt = new Label("");
  Label id_lab = new Label("Player ID#:");
  TextField id_fld = new TextField(4);
  Label name_lab = new Label("Name:");
  TextField name_fld = new TextField(50);
  Label mail_lab = new Label("E-Mail:");
  TextField mail_fld = new TextField(50);
  Label home_lab = new Label("Home Page:");
  TextField home_fld = new TextField(50);
  Label plan_lab = new Label("Plan:");
  TextArea plan_area = new TextArea(8, 65);
  Button b_finger = new Button("Finger");
  Button b_self = new Button("Finger Self");
  Button b_update = new Button("Update Self");
  Button b_clear = new Button("Clear");
  Button b_close = new Button("Close");

  
  public FingerPlayer (Client cli) {
    client = cli;

    Font font = new Font("Courier", Font.PLAIN, 12);
    id_fld.setFont(font);
    name_fld.setFont(font);
    mail_fld.setFont(font);
    home_fld.setFont(font);
    plan_area.setFont(font);
    name_fld.setEditable(false);
    mail_fld.setEditable(false);
    home_fld.setEditable(false);
    plan_area.setEditable(false);
    b_update.disable();
    
    GridBagLayout b = new GridBagLayout();
    setLayout(b);
    GridBagConstraints c = new GridBagConstraints();
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 1.0;
    c.weighty = 1.0;
    c.gridwidth = 1;
    b.setConstraints(id_lab, c);
    add(id_lab);
    b.setConstraints(id_fld, c);
    add(id_fld);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(prompt, c);
    add(prompt);
    c.gridwidth = 1;
    b.setConstraints(name_lab, c);
    add(name_lab);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(name_fld, c);
    add(name_fld);
    c.gridwidth = 1;
    b.setConstraints(mail_lab, c);
    add(mail_lab);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(mail_fld, c);
    add(mail_fld);
    c.gridwidth = 1;
    b.setConstraints(home_lab, c);
    add(home_lab);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(home_fld, c);
    add(home_fld);
    b.setConstraints(plan_lab, c);
    add(plan_lab);
    b.setConstraints(plan_area, c);
    add(plan_area);
    c.gridwidth = 1;
    b.setConstraints(b_finger, c);
    add(b_finger);
    b.setConstraints(b_self, c);
    add(b_self);
    b.setConstraints(b_update, c);
    add(b_update);
    b.setConstraints(b_clear, c);
    add(b_clear);
    b.setConstraints(b_close, c);
    add(b_close);
    pack();

    setTitle("IMS: Finger PLayer");
  }
  public synchronized void show () {
    super.show();
    super.toFront();
    id_fld.requestFocus();
  }
  public synchronized void clear () {
    id_fld.setText("");
    name_fld.setText("");
    mail_fld.setText("");
    home_fld.setText("");
    plan_area.setText("");
    mail_fld.setEditable(false);
    home_fld.setEditable(false);
    plan_area.setEditable(false);
    b_update.disable();
  }
  public synchronized void setPrompt (String p) {
    prompt.setText(p);
  }
  public synchronized void set (int id, String n, String m, String h, String p, boolean ed) {
    prompt.setText("");
    id_fld.setText(Integer.toString(id));
    name_fld.setText(n);
    mail_fld.setText(m);
    home_fld.setText(h);
    plan_area.setText(p);
    mail_fld.setEditable(ed);
    home_fld.setEditable(ed);
    plan_area.setEditable(ed);
    b_update.enable(ed);
  }
  public synchronized boolean action (Event e, Object w) {
    if (id_fld == e.target || b_finger == e.target) {
      try {
	int id = Integer.parseInt(id_fld.getText().trim());
	client.output(Packet.CPFinger(id));
      } catch (NumberFormatException ex) {
      }
      prompt.setText("");
      return true;
    }
    if (b_self == e.target) {
      client.output(Packet.CPFinger(client.myID));
      prompt.setText("");
      return true;
    }
    if (b_update == e.target) {
      client.output(Packet.CPChangeFinger(mail_fld.getText(),
				       home_fld.getText(),
				       plan_area.getText()));
      prompt.setText("Finger info updated.");
      return true;
    }
    if (b_clear == e.target) {
      if (b_update.isEnabled()) {
	mail_fld.setText("");
	home_fld.setText("");
	plan_area.setText("");
      } else {
	clear();
	id_fld.requestFocus();
      }
      prompt.setText("");
      return true;
    }
    if (b_close == e.target) {
      hide();
      return true;
    }
    return true;
  }
}

      
