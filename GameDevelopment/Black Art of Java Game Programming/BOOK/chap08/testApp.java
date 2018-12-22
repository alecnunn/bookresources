import java.applet.*;
import java.awt.*;
import HighScoreManager;

public class testApp extends Applet {

HighScoreManager HS=new HighScoreManager(10);
Graphics g;
Image im;
TextField F=new TextField(10);
Panel p = new Panel();

public void init() {
setLayout(new BorderLayout());

	add("South", p);
	p.add(new Button("Add"));
	p.add( new Button("Change Color"));
	p.add("Field",F);
	repaint();
}

public void update(Graphics bg) {

	Rectangle r= new Rectangle(0,0,size().width,size().height-p.preferredSize().height);
	im=createImage(size().width,size().height);
	g=im.getGraphics();
	HS.paintScores(g,r);

paint(bg);
}

public void paint(Graphics bg) {
	if(im!=null)
	bg.drawImage(im,0,0,null);
}

    public boolean action(Event evt, Object arg) {

	if ("Add".equals(arg)) {
		HS.addScore(F.getText(),(int)(Math.random() * 1000),"blah@blah"," ");
		F.setText(null);
		repaint();
	    return true;
	}
	if("Change Color".equals(arg)) {
		HS.newColors();
		repaint();
	return true;
	}
	return false;
    }

}
	