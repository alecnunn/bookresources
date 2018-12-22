import Sprite;
import java.awt.*;

public class userSprite extends Sprite {
int WIDTH = 25;
int HEIGHT = 25;

userSprite(ThreadGroup tg, String name) {
super(tg,name);
setID(Sprite.USER);
data = null;
}

public void move( int x, int y) {
	if( theG.getClipRect().inside(x,y) ) 
		super.move(x,y);
	else
		return;
}



}