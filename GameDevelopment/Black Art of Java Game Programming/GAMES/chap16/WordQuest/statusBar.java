import java.awt.*;
import Sprite;

public class statusBar extends Sprite {
String quest;
int score=0, lives=3, level=1;
boolean update=true;

statusBar(ThreadGroup tg, String d) {
	super(tg,d);
	data = null;
	setID( MESSAGE );
	theColor = Color.cyan;
	advance();
}

public void addScore(int amt) {
	score+=amt;
	update=true;
}

public void addLives(int amt) {
	lives+=amt;
	update=true;
}

public void addLevel(int amt) {
	level+=amt;
	update=true;
}

public void setQuestion( String str ) {
	quest=str;
	update=true;
}

public void advance() {

if( !update )
	return;

data="Score: "+score+"  Lives: "+lives+"  Level: "+level+"   "+quest;

generateImage();
update=false;
}


}