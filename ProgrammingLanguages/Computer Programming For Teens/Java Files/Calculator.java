// A third example using numbers with buttons in java
//  Chapter 23 ( online)

import java.awt.*;
import java.applet.*;
import javax.swing.*;
import java.awt.event.*;

public class Calculator extends Applet implements ActionListener
{
private int x = 5;
private int y = 9;
private int c = 0;
private Label b1;
private Button b2;
private Label b3;


public void init ()
{
b1 = new Label ("X = 5");
//b1.addActionListener(this);
add(b1);
b2 = new Button ("Multiply");
b2.addActionListener(this);
add(b2);
b3 = new Label ("Y = 9");
//b3.addActionListener(this);
add(b3);
}

public  void actionPerformed (ActionEvent event)
{if (event.getSource()== b2)
{c = 1;
     repaint();
}
}

  public void paint (Graphics g)
{
if (c==1)
g.drawString(x * y + " is the answer to your problem.",30,50);
}
}//ends class


