import java.applet.*;
import java.awt.*;

/////////////////////////////////////////////////////////////////
public class MouseTest extends Applet {
  Font courierFont;
  String testString = "Test the mouse in here!";

  public void init() {
    courierFont = new Font("Courier",Font.BOLD+Font.ITALIC,14);
  }

  public void paint(Graphics g) {
    g.setFont(courierFont);

    // center the string
    FontMetrics m = g.getFontMetrics();
    int stringWidth = m.stringWidth(testString);
    int width = (bounds().width - stringWidth )/2;
    int height = bounds().height / 2;

    // draw the string
    g.setColor(Color.green);
    g.drawString(testString,width,height);
  }

  public boolean mouseDown(Event e,int x,int y) {
    System.out.println("mouseDown at (" + x + "," + y + ")" );
    return true;
  }


  public boolean mouseUp(Event e,int x,int y) {
    System.out.println("mouseUp at (" + x + "," + y + ")" );
    return true;
  }

  public boolean mouseMove(Event e,int x,int y) {
    System.out.println("mouseMove at (" + x + "," + y + ")" );
    return true;
  }


  public boolean mouseDrag(Event e,int x,int y) {
    System.out.println("mouseDrag at (" + x + "," + y + ")" );
    return true;
  }

  public boolean mouseEnter(Event e,int x,int y) {
    System.out.println("mouseEnter at (" + x + "," + y + ")" );
    return true;
  }

  public boolean mouseExit(Event e,int x,int y) {
    System.out.println("mouseExit at (" + x + "," + y + ")" );
    return true;
  }
}
