package client;

import java.awt.*;
import java.util.Vector;
import java.util.Enumeration;

class FontManager extends Frame {
  private Client client;
  private String name = null;
  private String family = null;
  private int style = 0;
  private int size = 0;
  private Font font = null;
  private FontMetrics metrics = null;
  private Vector widgets = null;
  
  private String[] all_fonts = null;
  private Button b_cancel;
  private Button b_apply;
  private Label label;
  private Label size_label;
  private List font_list;
  private TextField font_size;
  private Checkbox s_bold;
  private Checkbox s_italic;
  
  public FontManager (Client cli, String n, String f, int st, int s) {
    client = cli;
    name = n;
    widgets = new Vector(8);
    setFont(f, st, s);
  }

  public synchronized void setFont (String f, int st, int s) {
    family = f;
    style = st;
    size = s;
    font = new Font(f, st, s);
    for (Enumeration e = widgets.elements(); e.hasMoreElements();) {
      Component c = (Component)e.nextElement();
      c.setFont(font);
    }
  }
  
  public synchronized String getName () {
    return family;
  }

  public synchronized int getStyle () {
    return style;
  }

  public synchronized int getSize () {
    return size;
  }
  
  public synchronized void remove (Component comp) {
    widgets.removeElement(comp);
  }

  public synchronized void register (Component comp) {
    widgets.addElement(comp);
    comp.setFont(font);
  }

  public synchronized void choose () {
    if (all_fonts == null) {
      all_fonts = getToolkit().getFontList();
      font_list = new List(Math.min(5, all_fonts.length), false);
      for (int i = 0; i < all_fonts.length; i++) {
	font_list.addItem(all_fonts[i]);
      }
      setTitle("IMS: Set Font");
      GridBagLayout b = new GridBagLayout();
      setLayout(b);
      GridBagConstraints c = new GridBagConstraints();
      c.fill = GridBagConstraints.BOTH;
      c.weightx = 1.0;
      c.weighty = 1.0;
      c.gridwidth = GridBagConstraints.REMAINDER;
      b.setConstraints(label = new Label(name), c);
      add(label);
      b.setConstraints(font_list, c);
      add(font_list);
      c.gridwidth = 1;
      b.setConstraints(size_label = new Label("Size:"), c);
      add(size_label);
      c.gridwidth = GridBagConstraints.REMAINDER;
      b.setConstraints(font_size = new TextField(3), c);
      add(font_size);
      c.gridwidth = 1;
      b.setConstraints(s_bold = new Checkbox("Bold"), c);
      add(s_bold);
      c.gridwidth = GridBagConstraints.REMAINDER;
      b.setConstraints(s_italic = new Checkbox("Italic"), c);
      add(s_italic);
      c.gridwidth = 1;
      b.setConstraints(b_apply = new Button("Apply"), c);
      add(b_apply);
      b.setConstraints(b_cancel = new Button("Cancel"), c);
      add(b_cancel);
      pack();
    }
    for (int i = 0; i < all_fonts.length; i++) {
      if (all_fonts[i].equals(family)) {
	font_list.select(i);
	break;
      }
    }
    font_size.setText(Integer.toString(size));
    s_bold.setState((style & Font.BOLD) != 0);
    s_italic.setState((style & Font.ITALIC) != 0);
    show();
  }

  public synchronized boolean action (Event e, Object w) {
    if (b_apply == e.target) {
      try {
	int size = Integer.parseInt(font_size.getText());
	int style = (s_bold.getState() ? Font.BOLD : Font.PLAIN) |
	  (s_italic.getState() ? Font.ITALIC : Font.PLAIN);
	setFont(font_list.getSelectedItem(), style, size);
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

