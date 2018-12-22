package client;

import java.awt.*;

class Login extends Panel {
  private Client client = null;

  public Label name_prompt = new Label("Name:");
  public Label pass_prompt = new Label("Password:");
  public Label result_prompt = new Label("");
  public TextField name_field = new TextField(25);
  public TextField pass_field = new TextField(25);
  public Button login_old = new Button("Login");
  public Button login_new = new Button("New Player");
  public Button login_clear = new Button("Clear");
  public Button login_cancel = new Button("Cancel");

  public Login (Client cli) {
    GridBagLayout b = new GridBagLayout();
    GridBagConstraints c = new GridBagConstraints();

    client = cli;

    Font fix_font = new Font("Courier", Font.PLAIN, 12);
    name_field.setFont(fix_font);
    pass_field.setFont(fix_font);
    pass_field.setEchoCharacter('?');

    setLayout(b);
    c.fill = GridBagConstraints.BOTH;
    c.weightx = 0.0;
    c.weighty = 0.0;
    c.gridwidth = 1;
    b.setConstraints(name_prompt, c);
    add(name_prompt);
    c.weightx = 1.0;
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(name_field, c);
    add(name_field);
    c.weightx = 0.0;
    c.gridwidth = 1;
    b.setConstraints(pass_prompt, c);
    add(pass_prompt);
    c.weightx = 1.0;
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(pass_field, c);
    add(pass_field);
    c.weightx = 0.0;
    c.gridwidth = 1;
    b.setConstraints(login_old, c);
    add(login_old);
    b.setConstraints(login_new, c);
    add(login_new);
    b.setConstraints(login_clear, c);
    add(login_clear);
    c.gridwidth = GridBagConstraints.REMAINDER;
    b.setConstraints(login_cancel, c);
    add(login_cancel);
    b.setConstraints(result_prompt, c);
    add(result_prompt);
    
    name_field.requestFocus();
  }
  
  public boolean action (Event e, Object w) {
    if (login_clear == e.target) {
      name_field.setText("");
      pass_field.setText("");
      name_field.requestFocus();
      result_prompt.setText("Enter name and password to login.");
      return true;
    }
    if (name_field == e.target) {
      pass_field.setText("");
      pass_field.requestFocus();
      return true;
    }
    return client.action(e, w);
  }

  public void mode_wait () {
    login_new.disable();
    login_old.disable();
    result_prompt.setText("Waiting for the server to respond...");
  }

  public void mode_login (String s) {
    login_new.enable();
    login_old.enable();
    name_field.requestFocus();
    result_prompt.setText(s);
  }
}

      
    
