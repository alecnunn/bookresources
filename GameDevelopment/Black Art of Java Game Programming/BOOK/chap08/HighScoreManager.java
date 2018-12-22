import java.awt.*;
import java.util.*;
import java.lang.*;
import java.net.*;
import java.io.*;


public class HighScoreManager extends Object implements Runnable {

HighScoreList L;
int NUM_SCORES = 0;
private int red,green,num;
private int last=0;
Thread kicker = null;
final int DELAY = 120000;
long lastCheck;
boolean beenUpdated = false;
String updateStr = "";

HighScoreManager(int max) {
	NUM_SCORES=max;
	L= new HighScoreList(NUM_SCORES);
	kicker = new Thread(this);
	kicker.start();
	kicker.setPriority(Thread.MIN_PRIORITY);
	newColors();
}



String getScoreData() {
Socket s;
DataInputStream ds;
PrintStream ps;
String temp = null;


try {
	s = new Socket("localhost",2357);
	ds = new DataInputStream(s.getInputStream());
	ps = new PrintStream(s.getOutputStream());
} catch (Exception e) {
	return null;
}

StringTokenizer st;
try {
if ( updateStr != null) {
	st = new StringTokenizer( updateStr , "||");
	while (st.hasMoreTokens()) 
		ps.println("update::"+st.nextToken());
	updateStr = "";
}

ps.println("request::" + lastCheck);
temp = ds.readLine().trim();
if ( ! temp.startsWith( "none" ) ) updated(true);
ps.println("bye");
}
catch (Exception e) {
	return null;
}

System.out.println("Got: "+temp);
try {


ds.close();
ps.close();
s.close();
} catch (Exception e);

if (!temp.startsWith(  "none" ))
return temp;
else
return null;
}

void updated(boolean b) {

beenUpdated  = b;
lastCheck = System.currentTimeMillis();

}

boolean updated() {
return beenUpdated;
}

void getScores() {
String str;
int x=0;

	str=getScoreData();	

if (str==null || str == "none") return ;

L = new HighScoreList(NUM_SCORES, str);

}
	
void paintScores(Graphics g) {
paintScores(g,g.getClipRect());
}


void paintScores(Graphics g, Rectangle r) {

int x;
int fontSize,i,where=0;
String str;
int b=255;

if (L == null) return;
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
	if(L.getScore(x) != null) {
		str+=L.getScore(x).name;
		for (i= 5 + fm.stringWidth(str) ; i< r.x + r.width - 6 - fm.stringWidth(" " + L.getScore(x).score) ; i+= fm.stringWidth(".")) 
			str+=".";
		str+=" "+L.getScore(x).score+"  ";
		}
	g.setColor(Color.black);
	g.drawString(str,r.x,where);
}
}

}



public void newColors() {
	red=(int)(Math.random()*200)+56 ;
	green= (int)(Math.random()*200)+56;
	num= (int)(Math.random()*45)+15;
}
		
public void addScore( String name, float score, String email, String other) {
String temp;
	
	temp = L.tryScore( name, score, email, other);
	if (temp != null)
		updateStr += temp + "||";
}








public HSob getHighestScore() {

return L.getScore(last=0);
}

public HSob getScore(int x) {

return L.getScore(last=(x-1));
}

public HSob getNextScore() {

if(last++ >= NUM_SCORES)
	return null;
return L.getScore(last);
}

public void start() {

if (kicker == null)
	kicker = new Thread(this);

}

public void stop() {

kicker = null;
}

public void run() {

while (kicker != null) {
getScores();

try{ kicker.sleep(DELAY); } catch(Exception e);
}

}



}



		