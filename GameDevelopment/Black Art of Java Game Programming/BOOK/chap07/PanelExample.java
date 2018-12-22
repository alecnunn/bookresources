import java.applet.*;
import java.awt.*;


public class PanelExample extends Applet{
  public void init() {
    setLayout(new GridLayout(2,3,13,13));
    Panel p = new Panel();    // define a panel

    // set panel layout
    p.setLayout(new FlowLayout(FlowLayout.RIGHT));

    // insert buttons into panel
    p.add(new Button("Buttons"));
    p.add(new Button("in"));
    p.add(new Button("Panel"));

    // add labels
    add(new Label("Label 1", Label.CENTER));
    add(new Label("Label 2", Label.CENTER));
    add(new Label("Label 3", Label.CENTER));
    add(new Label("Label 4", Label.CENTER));
    add(new Label("Label 5", Label.CENTER));

    // add panel
    add(p);

  }
}


