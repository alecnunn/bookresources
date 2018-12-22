import java.awt.*;
import java.applet.*;
/**
 * Represents a panel that does the painting offscreen which avoids flickering. Good for using in animations.
 */
class fNoFlickerApplet extends Applet {
	private Image offScreenImage;
	private Graphics offScreenGraphics;
	private Dimension offScreenSize;

	public final void update(Graphics theG){
		Dimension d=size();
		if((offScreenImage==null) || (d.width != offScreenSize.width) || (d.height != offScreenSize.height)){
			offScreenImage = createImage(d.width,d.height);
			offScreenSize = d;
			offScreenGraphics = offScreenImage.getGraphics();
   		offScreenGraphics.clearRect(0,0,offScreenSize.width,offScreenSize.height);
		}
		paint(offScreenGraphics);
		theG.drawImage(offScreenImage,0,0,null);
	}
}
