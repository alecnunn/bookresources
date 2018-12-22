import Sprite;
import java.lang.*;

public class syncSprite extends Sprite {
int count=0;
boolean flag=true;

syncSprite(ThreadGroup tg, String d) {
	super(tg,d);
	count=0;
	flag=true;
	DELAY=50;
}

public void run() {
	while(true) {
		try{ sleep(DELAY); } catch(Exception e);
//		paintSprite( theG );
	if( flag || count==0 || count >= getThreadGroup().activeCount() ) {	
		count=0;
		advance();
	flag = false;
	int i;
	syncSprite s[]= new syncSprite[i=getThreadGroup().activeCount()];
	enumerate(s);

	while(i-- > 0)
		s[i].count++;
	} else
		yield();
		}
}

}
