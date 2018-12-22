import java.awt.*;
import java.applet.*;

public class ButtonTest extends Applet {
  public void init() {
    Button b = new Button("Hello");
    setLayout(new FlowLayout());
    add(b);
  }
}
