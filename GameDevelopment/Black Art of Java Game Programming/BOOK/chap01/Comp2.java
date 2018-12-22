import java.applet.*;
import java.awt.*;

public class Comp2 extends Applet {
  public void paint(Graphics g) {
	g.setColor(Color.green);
	g.drawLine(17,17,273,273);
	g.drawLine(17,18,273,274);
	
	g.setColor(Color.red);
	g.fillRect(30,130,130,130);

	g.setColor(Color.yellow);
	g.fillOval(100,50,130,130);
}

  public void stop() {

    System.out.println(">> stop <<");

  }

}
