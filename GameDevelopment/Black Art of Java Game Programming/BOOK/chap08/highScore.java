import java.awt.*;
import java.util.*;
import java.lang.*;


public class highScore extends Object {


HSob scores[];
int NUM_SCORES=10;
private int red,green,num;
private int last=0;

highScore() {

	NUM_SCORES=getScores();
	newColors();
}

highScore(int max) {
	NUM_SCORES=max;
	getScores();
	newColors();
}

String getScoreData() {

return "Eric&1000&eries@netcom.com||Dude&2&blah@cool||Darn&0&darn@lame.com||THeGuy&24235&bhffh||Whoever&234";
}

int getScores() {
String str;
int x=0;

	scores=new HSob[NUM_SCORES];
	str=getScoreData();	


StringTokenizer st1=new StringTokenizer(str,"||");

		while (st1.hasMoreTokens()  ) {
			StringTokenizer st2=new StringTokenizer(st1.nextToken(),"&");
			addScore(st2.nextToken(), Integer.valueOf(st2.nextToken()).intValue());
			}


return st1.countTokens();
}
	
void paintScores(Graphics g) {
paintScores(g,g.getClipRect());
}


void paintScores(Graphics g, Rectangle r) {

int x;
int fontSize,i,where=0;
String str;
int b=255;

for(x=r.x  ; x < r.width + r.x  ;x += (int)(r.width/num)) {
	b-=(int)(255/num);
	if (b<0) b=0;
	g.setColor(new Color( red , green , b));
if (g.getClipRect().intersects( new Rectangle(x,r.y,r.width,r.height)))
	g.fillRect ( x , r.y , r.width , r.height);
	}

fontSize=(int)(r.height/(NUM_SCORES + 4));
g.setFont(new Font("Times New Roman", Font.PLAIN, fontSize));
FontMetrics fm = g.getFontMetrics();

where= 5 + fontSize;
g.setColor(Color.black);
str= "High Scores";
g.drawString( str, (int)((r.width + r.x - fm.stringWidth(str))/2), where);


for (x = 0 ; x< NUM_SCORES ; x ++) {
	where+=fontSize + 5;
if( g.getClipRect().intersects( new Rectangle ( r.x, where-fontSize-5, r.width, fontSize))) {
	str="   "+(x+1)+". ";
	if(scores[x] != null) {
		str+=scores[x].name;
		for (i= 5 + fm.stringWidth(str) ; i< r.x + r.width - 6 - fm.stringWidth(" " + scores[x].score) ; i+= fm.stringWidth(".")) 
			str+=".";
		str+=" "+scores[x].score+"  ";
		}
	g.setColor(Color.black);
	g.drawString(str,r.x,where);
}
}

}

public int addScore(String name, float score) {
int x,i;


x=0;
while (x<NUM_SCORES) {

if (scores[x] == null || score > scores[x].score) {
	for( i=NUM_SCORES-2 ; i>=x ; i--) 
		scores[i+1]=scores[i];
	scores[x]=new HSob(name, score);

	return x+1;
	}
x++;
}

return 0;
}	


public void newColors() {
	red=(int)(Math.random()*200)+56 ;
	green= (int)(Math.random()*200)+56;
	num= (int)(Math.random()*50)+2;
}
		
public HSob getHighestScore() {

return scores[last=0];
}

public HSob getScore(int x) {

return scores[last=(x-1)];
}

public HSob getNextScore() {

if(last++ >= NUM_SCORES)
	return null;
return scores[last];
}



}

class HSob extends Object {

	public String name,email,other;
	public float score;

	HSob(String n, float sc) {
		name=n;
		score=sc;
	}

	HSob() {
		name="noname";
		score=0;
	}
	}


		