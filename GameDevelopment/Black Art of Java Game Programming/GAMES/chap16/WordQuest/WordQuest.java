import java.applet.*;
import java.awt.*;
import java.util.*;
import java.net.*;
import java.io.*;
import Terrain;
import StarField;                  
import Sprite;
import userSprite;
import syncSprite;
import HighScoreManager;

public class WordQuest extends Applet implements Runnable {
Thread kicker;      
StarField SF=null;                         
Terrain T=null;
statusBar bar;
userSprite user;

ThreadGroup group,bullets,everything;

Rectangle SFrect, Trect,statRect;
Dimension d;
promptFrame PF;
Image im;
Graphics offScreenG;


Vector questions;
Question currentQ;
String name;

boolean playing = false;

public void init() {
	d = size();
	statRect = new Rectangle(0, 0, d.width, 16);
	SFrect = new Rectangle( 0,statRect.height,d.width,(int)(d.height*.8));
	Trect = new Rectangle( 0, statRect.height+SFrect.height, d.width, (int)(d.height-statRect.height-SFrect.height) );

	Sprite.theG = getGraphics().create();

	everything = new ThreadGroup( "all" );	
	group = new ThreadGroup( everything,"enemies" );
	bullets = new ThreadGroup(everything,"bullets");

	bar = new statusBar(everything, "statusbar");
	bar.setBounds( statRect );
	bar.im = createImage( statRect.width, statRect.height );
	bar.setPriority(Thread.MIN_PRIORITY);
	bar.start();

	PF = new promptFrame();
	PF.resize(d.width,d.height);
	PF.show();

//	kicker = new Thread(everything,this,"blah");
	kicker = new Thread(this);
	kicker.setPriority(Thread.MAX_PRIORITY);
	kicker.start();


}

void initUser() {

	if( SF == null ) {
		SF = new StarField(100, SFrect, createImage(SFrect.width, SFrect.height), everything, "sf");
		SF.setBounds( SFrect );
		SF.setSpeed(2,0);
		SF.start();
		} 
	if( T == null || !T.isAlive()) {
               		T = new Terrain(50,Trect,getGraphics());
		T.start();
		T.suspend();
		} 
	if( user != null && user.isAlive() ) user.stop();
	user = new userSprite(everything, "user");
	user.setID( Sprite.USER );
	user.setXY( 50, (int)(SFrect.height/2) );
	user.im = createImage( user.WIDTH, user.HEIGHT );
	user.setSpeed(0);
	user.start();
}

/*
public void update(Graphics g) {


	if( Sprite.theG == null) {
		Sprite.theG = g.create();
		Sprite.theG.clipRect(0,0,SFrect.width,SFrect.height+statRect.height);
		}

	paint(g);

}
*/

public void paint(Graphics g) {

	if(im!=null)
		g.drawImage(im,0,0,null);

}                      

public void start() {
questions = new Vector();
	try{ 
		URL theURL = new URL(getCodeBase(), "data.txt"); 
		new dataThread( questions, theURL ).start();
	} catch (Exception e);
}

public void run() {


//nextQuestion();

while(true) 
if( playing ) {
	try{kicker.sleep(50); }catch(Exception e);
	doCheck();

	im = createImage(d.width, d.height-Trect.height);
	offScreenG = im.getGraphics();
try{ 	while(SF.flag) kicker.sleep(100); } catch(Exception e);
	offScreenG.drawImage(SF.currentImage(),SF.x,SF.y,null); //} catch(Exception e);
try {
	Sprite s[]=new Sprite[everything.activeCount()];
	everything.enumerate( s );
	for(int i=0;i<everything.activeCount();i++)
		if( s[i] != SF )
			try {
			offScreenG.drawImage(s[i].currentImage(),s[i].x,s[i].y,null);
			} catch(NullPointerException e);
} catch(ArrayIndexOutOfBoundsException e);

	getGraphics().drawImage(im,0,0,null);		

} else {
	if( PF.ready ) {
		PF.ready=false;
		name=PF.gotName;
		playing=true;
		PF.hide();
		initUser();
		nextQuestion();
		everything.resume();
		T.resume();
	}
		try{kicker.sleep(500);}catch(Exception e);
	}
	
}  

public void stop() {
	everything.stop();
//	everything.destroy();
}

public void doCheck() {
int i,j;
Sprite sprites[]=new Sprite[group.activeCount()],bul[]=new Sprite[bullets.activeCount()];

group.enumerate( sprites );
bullets.enumerate( bul );
int x;


if( !user.isAlive() ) {
	System.out.println("You lose!\n");
	bar.addScore(-10);
	bar.addLives(-1);
	bullets.stop();
	T.suspend();
	if( bar.lives==0) {
		playing=false;
		PF.HS.addScore(name,bar.score,null,null);
		PF.show();
		return;
		}
	initUser();		
	nextQuestion();
	return;
	}
	
for( x=0;x<group.activeCount();x++) {
sprites[x].suspend();
	if( !sprites[x].intersects( SFrect ) )
		sprites[x].stop();
	else
	if( user.intersects( sprites[x].bounds )) {
		user.collision( user.id * sprites[x].id );
		sprites[x].collision( user.id * sprites[x].id );
		}
	for(int y=0; y < bullets.activeCount(); y++ ) {
	Sprite bullet = bul[y];
	bullet.suspend();
		if( ! bullet.intersects( SFrect ) || bullet.getSpeed() == 0 )
			bullet.stop();
		else
		if( bullet.intersects(user.bounds) ) {
			user.stop();
			bullet.stop();
		} else
		if( sprites[x].intersects( bullet.bounds ) ) 
			if( currentQ.correct.equals( sprites[x].data ) ) {
				sprites[x].stop();
				bullet.stop();
				bar.addScore(50);
			} else 
				bullet.setSpeed( - bullet.getSpeed(),0 );
		bullet.resume();
		}
		sprites[x].resume();	
	}

if( group.activeCount() <= 2 )
	nextQuestion();


}	

public boolean mouseDown(Event evt, int x, int y) {
Sprite b= newBullet(user.x,user.y);

b.setSpeed(x-user.bounds.x,y-user.bounds.y);
b.start();

return true;
}


public Sprite newBullet(int x, int y) {
Sprite b;
Rectangle temp = new Rectangle( x+user.bounds.width+5, y+(int)(user.bounds.width/2), 15,3 );

	b = new Sprite(bullets,"bang");
	b.setBounds( temp );
	b.im = createImage(temp.width, temp.height);
	b.setSpeed(25,0);
	b.setID( Sprite.BULLET );
bar.addScore(-1);
return b;
}

public boolean keyUp(Event evt, int key) {

switch(key) {

case 'w': {

	Sprite.warp=1;
	return true;
	}
}
return false;
}

public boolean keyDown(Event evt, int key) {

switch( key ) {

case 'p' : {
	everything.suspend();
	return true;
	}
case 'r' : {
	everything.resume();
	return true;
	}

case 'w': {

	Sprite.warp= 5;
	return true;
	}


case ' ': {
	Sprite b=newBullet(user.x,user.y);
	b.start();
	return true;
	}

case '8': {
	user.move( user.x,user.y-5);
	return true;
	}

case '2': {
	user.move( user.x,user.y+5);
	return true;
	}

case '4': {
	user.move( user.x-5,user.y);
	return true;
	}
case '6': {
	user.move( user.x+5,user.y);
	return true;
	}
case '1': {
	user.move( user.x-5,user.y+5);
	return true;
	}
case '3': {
	user.move( user.x+5,user.y+5);
	return true;
	}
case '7': {
	user.move( user.x-5,user.y-5);
	return true;
	}
case '9': {
	user.move( user.x+5,user.y-5);
	return true;
	}


}
return false;
}
	

public void nextQuestion() {

group.stop();


if( questions.isEmpty() ) 
	try{ 
		URL theURL = new URL(getCodeBase(), "data.txt"); 
		new dataThread( questions, theURL ).start();
	} catch (Exception e);


currentQ=null;
do {
	try {
	currentQ = (Question)questions.elementAt((int)(Math.random()*questions.size()));
	questions.removeElement(currentQ);
	} catch(Exception e);
} while( currentQ == null );

bar.setQuestion( currentQ.question );

String dataArray[],temp;

dataArray = new String[ 5 ];

boolean flag = false;
for(int x=0; x<5; x++) { 
		temp = currentQ.takeAnswer();
		if( temp == null )
		do {
		flag = false;
			temp = Question.randAnswer();

		for(int i =0;i<x;i++)
			if( temp == null || temp.equals(dataArray[i]) ) 
				flag = true;
		} while( flag );

		dataArray[x]=temp;
		}

double spdX;

int h = (int)(SFrect.height/5);
spdX = 20*Math.random()+5*bar.level;

Sprite s;
for(int x=0; x<5; x++) {
s = new syncSprite( group, dataArray[x] );
s.HEIGHT=h;
s.setXY( SFrect.width+SFrect.x-25, SFrect.y+(h*x));
	s.setSpeed( -spdX, 0);
	s.setID( Sprite.ENEMY );
s.im=createImage(s.WIDTH,s.HEIGHT);
s.start();
s.suspend();
	}
group.resume();
T.resume();

}

	
/* END OF CLASS */
}

class Question {

static Vector biglist = new Vector();

public String question,correct;
Vector answers;


Question(String q, String a) {
	question = q;
	correct  = a;
	answers = new Vector(5);
	addAnswer(a);
}

public void addAnswer(String a) {
	answers.addElement( a );

if( !biglist.contains(a))
	biglist.addElement(a);
	
}

public String getQuestion() {
	return question;
}

public String takeAnswer() {

if( answers.isEmpty() )
	return null;

int rand = (int)(Math.random()*answers.size());

	String temp = (String)answers.elementAt(rand);
	answers.removeElementAt(rand);
return temp;
}

public static String randAnswer() {
int rand = (int)(Math.random()*biglist.size());

return (String)biglist.elementAt(rand);
}

}

class dataThread extends Thread {
URL theURL;
Vector questions;

dataThread( Vector v, URL u ) {
questions = v;
theURL = u;
}

public void run() {

DataInputStream dis;

try {

	dis = new DataInputStream( theURL.openStream() );

Question temp=null;
String q = null;
for( String str= dis.readLine(); str != null; str = dis.readLine() ) {
StringTokenizer st = new StringTokenizer(str,"||");

//System.out.println(str);
if( st.countTokens() >= 2 )
	switch( st.nextToken().charAt(0) ) {

		case 'Q': {
			q = st.nextToken();
			if( temp != null ) {
				questions.addElement( temp );
				temp = null;
				}
			break;
		}
		case 'A': {
			if(temp == null)
				temp = new Question(q, st.nextToken());
			else
				temp.addAnswer(st.nextToken());
			break;
		}
	}
/*if( temp != null ) {
	System.out.println(temp.getQuestion());
	System.out.println(temp.question);
	}*/
}
if( temp != null )
	questions.addElement( temp );

} catch (Exception e) {
	System.out.println("Data file not found.\n"+e);
}

}

}

/* WINDOW CODE */
class promptFrame extends Frame {
TextField TF;
HighScoreManager HS = new HighScoreManager(10);
Panel p;
public String gotName;
boolean ready;
promptFrame() {
	super("Ready for a new game?");
	setLayout( new BorderLayout() );
	p = new Panel();
	p.setLayout( new FlowLayout() );
	p.add( "West",new Label("Input your name: ") );
	p.add("West",TF = new TextField(20) );
	p.add("Center",new Button("OK") );
	add("South",p);
	gotName = null;
	ready=false;
}

public void paint(Graphics g) {
Graphics offG;
Dimension d = size();
d.height-=p.preferredSize().height;
Image im=createImage(d.width,d.height);
Rectangle r = new Rectangle(0,0,d.width,d.height);


offG=im.getGraphics();
HS.paintScores(offG,r);
g.drawImage(im,0,0,null);
}

public boolean handleEvent(Event evt) {

if( (evt.target instanceof TextField && evt.id==Event.ACTION_EVENT) || (evt.target instanceof Button && evt.arg.equals("OK")) ) {

	if( TF.getText() != "" )
		gotName = TF.getText();
	ready = true;
	return true;
	}
return super.handleEvent(evt);
}
}