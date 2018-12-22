import java.applet.*;
import java.awt.*;

public class FrameExample extends Applet {
   public void init() {
     // create new Frame, with "FrameTest" as the title
     Frame f = new Frame("FrameTest"); 
     f.resize(113,117);
     f.setResizable(false);
     f.setCursor(Frame.CROSSHAIR_CURSOR);
     f.show();
  
   }
 }

