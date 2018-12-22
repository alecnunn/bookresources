package client;

import java.awt.*;

class SetGeometry extends Frame {
  Client client;
/*
  Label tile_lab = new Label("Tiles:");
  Label x_lab = new Label("X");
  Label y_lab = new Label("Y");
  Label p_lab = new Label("pad");
  Button b_default = new Button("Default");
  TextField x_fld = new TextField(3);
  TextField y_fld = new TextField(3);
  TextField p_fld = new TextField(3);
  Padding pad = new Padding(1, 1, true);
*/
  Label text_lab = new Label("Text panels:");
  Label s_lab = new Label("Players / Tables");
  Label b_lab = new Label("Messages");
  Label rows = new Label("rows");
  Label cols = new Label("cols");
  TextField SV = new TextField(3);
  TextField SH = new TextField(3);
  TextField BV = new TextField(3);
  TextField BH = new TextField(3);
  Button b_apply = new Button("Apply");
  Button b_cancel = new Button("Cancel");

  public SetGeometry (Client cli) {
    client = cli;
    setTitle("IMS: Set Geometry");
    GridBagLayout b = new GridBagLayout();
    setLayout(b);
    GridBagConstraints c = new GridBagConstraints();
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 1.0;
    c.weighty = 1.0;
/*
    c.gridwidth = 1;
    b.setConstraints(tile_lab, c);
    add(tile_lab);
    b.setConstraints(x_lab, c);
    add(x_lab);
    b.setConstraints(y_lab, c);
    add(y_lab);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(p_lab, c);
    add(p_lab);
    c.gridwidth = 1;
    b.setConstraints(b_default, c);
    add(b_default);
    b.setConstraints(x_fld, c);
    add(x_fld);
    b.setConstraints(y_fld, c);
    add(y_fld);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(p_fld, c);
    add(p_fld);
    b.setConstraints(pad, c);
    add(pad);
*/
    c.gridwidth = 2;
    b.setConstraints(text_lab, c);
    add(text_lab);
    c.gridwidth = 1;
    b.setConstraints(rows, c);
    add(rows);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(cols, c);
    add(cols);
    c.gridwidth = 2;
    b.setConstraints(s_lab, c);
    add(s_lab);
    c.gridwidth = 1;
    b.setConstraints(SV, c);
    add(SV);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(SH, c);
    add(SH);
    c.gridwidth = 2;
    b.setConstraints(b_lab, c);
    add(b_lab);
    c.gridwidth = 1;
    b.setConstraints(BV, c);
    add(BV);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(BH, c);
    add(BH);
    c.gridwidth = 2;
    b.setConstraints(b_apply, c);
    add(b_apply);
    b.setConstraints(b_cancel, c);
    add(b_cancel);
    pack();
  }

  public synchronized void show () {
    SV.setText(Integer.toString(client.s_vsize));
    SH.setText(Integer.toString(client.s_hsize));
    BV.setText(Integer.toString(client.b_vsize));
    BH.setText(Integer.toString(client.b_hsize));
    super.show();
  }

  public synchronized boolean action (Event e, Object w) {
    if (b_apply == e.target) {
      try {
	client.s_vsize = Integer.parseInt(SV.getText());
	client.s_hsize = Integer.parseInt(SH.getText());
	client.b_vsize = Integer.parseInt(BV.getText());
	client.b_hsize = Integer.parseInt(BH.getText());
	client.reconfigure();
	hide();
      } catch (NumberFormatException ex) {
      }
      return true;
    }
    if (b_cancel == e.target) {
      hide();
      return true;
    }
    return true;
  }
}
