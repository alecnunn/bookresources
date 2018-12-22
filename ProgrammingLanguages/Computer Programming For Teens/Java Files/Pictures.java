// A second example using images in java
// Chapter 23  (online)
import java.awt.*;

import java.applet.*;
import javax.swing.*;
import java.awt.event.*;

public class Pictures extends Applet implements ActionListener
{
  Image pic[]= new Image[3];
private int x;
private Button b1;
private Button b2;
private Button b3;

public void init ()
{
pic[0]= getImage(getCodeBase(),"Eiffel.gif");
pic[1]= getImage(getCodeBase(),"BigBen.gif");
pic[2]= getImage(getCodeBase(),"Japan.gif");
b1 = new Button ("The Eiffel Tower");
b2 = new Button ("Big Ben");
b3 = new Button ("The Mountains of Shikoku, Japan");
add(b1);
add(b2);
add(b3);
b1.addActionListener(this);
b2.addActionListener(this);
b3.addActionListener(this);
}

public  void actionPerformed (ActionEvent event)
{if (event.getSource()== b1)
     {
x = 0;
repaint();
}
if (event.getSource()== b2)
     {
x = 1;
repaint();
}
if (event.getSource()== b3)
     {
x = 2;
repaint();
}
}//ends ActionListener
  public void paint (Graphics g)
{
g.drawImage(pic[x],50,50,this);
}
}//ends class


