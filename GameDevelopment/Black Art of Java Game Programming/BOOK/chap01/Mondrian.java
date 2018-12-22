import java.applet.*;
import java.awt.*;

public class Mondrian extends Applet {
  
  public void init() {

    System.out.println(">> init <<");
    setBackground(Color.black);
    
  }


  public void start() {

    System.out.println(">> start <<");

  }

  public void paint(Graphics g) {
    System.out.println(">> paint <<");

    g.setColor(Color.yellow);
    g.fillRect(0,0,90,90);
    g.fillRect(250,0,40,190);
    g.fillRect(80,110,100,20);

    g.setColor(Color.blue);
    g.fillRect(80,200,220,90);
    g.fillRect(100,10,90,80);

    g.setColor(Color.lightGray);
    g.fillRect(80,100,110,90);

    g.setColor(Color.red);
    g.fillRect(200,0,45,45);
    g.fillRect(0,100,70,200);
   
    g.setColor(Color.magenta);
    g.fillRect(200,55,60,135);
  }

  public void stop() {

    System.out.println(">> stop <<");

  }

}
