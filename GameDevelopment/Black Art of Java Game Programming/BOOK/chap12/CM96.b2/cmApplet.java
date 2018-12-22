import  java.awt.*;
import  java.net.*;
import  java.io.*;
import  java.net.*;
import  java.applet.*;
import  java.util.*;


public class cmApplet extends Applet implements Runnable{
	Thread myThread;
	boolean alive;
	boolean key [ ];
	cmWorld world;
	cmAbstractPlayer player;
	cmDisplayPanel display1;
   Panel panel;

	public void init () {
      double viewAngle=new Double(getParameter("cmApplet_viewAngle")).doubleValue();
      viewDistance=new Double(getParameter("cmApplet_viewDistance")).doubleValue();
      gridSize=new Double(getParameter("cmApplet_gridSize")).doubleValue();

      key=new boolean[100];
   
	   showStatus(" Initiating Combat Machines 96 ");

      world=new cmWorld(this);
		
      setLayout(new GridLayout(1,1));
      add(display1=new cmDisplayPanel(world.getActivePlayer(),"C"));

		myThread=new Thread(this);
		myThread.start();
      myThread.setPriority(Thread.MAX_PRIORITY);
   	alive=true;
 	}

   int frames;
   long time;
   public final void run () {
      lastUpdate=System.currentTimeMillis()   ;
      while(alive){
         long currentTime=System.currentTimeMillis();
         long dtmillis=currentTime-lastUpdate;
         double dt=(double)dtmillis/1000;
         lastUpdate=currentTime;

         if(dt>0.2) dt=0.2;
         //-- get the active player
         player=world.getActivePlayer();
         //-- handle keyboard events
         handleKeyboard();
         //-- update the world
         world.update(dt);
         //-- update the display
         display1.update(dt);
         //-- calculate frame rate once a second
         frames++;
         time+=dtmillis;
         if(time>1000){
            showStatus("fps="+(double)((int)(10000*(double)frames/(double)time))/10);
            time=0;
            frames=0;
         }
      }
	}

   public final void start () {
      showStatus(" Starting Combat Machines 96 ");
      if(myThread==null){
         myThread=new Thread(this);
         myThread.setPriority(Thread.MAX_PRIORITY);
         myThread.start();
         alive=true;
      }
   }

   public final synchronized void stop () {
      showStatus(" Stoping applet.");
      if(myThread!=null){
         myThread.stop();
         myThread=null;
         alive=false;
      }
   }

   public boolean keyDown (Event ev, int k) {
      keyboardEvent(ev.key,true);
      return true;
   }

   public boolean keyUp (Event ev, int k) {
      keyboardEvent(ev.key,false);
      return true;
   }

   protected void keyboardEvent (int k, boolean pressed) {
	   switch(k){
         case 'h': key[cmEventSteeringCommand.TURN_LEFT]=pressed;break;			
         case 'k': key[cmEventSteeringCommand.TURN_RIGHT]=pressed;break;			
         case 't': key[cmEventSteeringCommand.INCREASE_VELOCITY]=pressed;break;			
         case 'g': key[cmEventSteeringCommand.BRAKE]=pressed;break;			
         case 'y': key[cmEventSteeringCommand.CLIMB]=pressed;break;
         case 'i': key[cmEventSteeringCommand.DECENT]=pressed;break;
         case 'u': key[cmEventSteeringCommand.PITCH_DOWN]=pressed;break;
         case 'j': key[cmEventSteeringCommand.PITCH_UP]=pressed;break;
         case 'a': key[cmEventWeaponCommand.FIRE]=pressed;break;			
         case '1': key[cmEventWeaponCommand.MINICANNON]=pressed;break;			
         case '2': key[cmEventWeaponCommand.MISSILE]=pressed;break;			
         case '3': key[cmEventWeaponCommand.BOMB]=pressed;break;			
      }
   }

   protected void handleKeyboard () {
      //-- handle keyboard;
      if(key[cmEventSteeringCommand.TURN_LEFT]) player.turnLeft(1,0.1);
      if(key[cmEventSteeringCommand.TURN_RIGHT]) player.turnRight(1,0.1);
      if(key[cmEventSteeringCommand.INCREASE_VELOCITY]) {
         player.increaseVelocity(1,0.1);
      }
      if(key[cmEventSteeringCommand.BRAKE]) player.brake(1,0.1);
      if(key[cmEventSteeringCommand.CLIMB]) player.climb(1,0.1);
      if(key[cmEventSteeringCommand.DECENT]) player.decent(1,0.1);
      if(key[cmEventSteeringCommand.PITCH_UP]) player.pitchUp(1,0.1);
      if(key[cmEventSteeringCommand.PITCH_DOWN]) player.pitchDown(1,0.1);

      if(key[cmEventWeaponCommand.FIRE]) player.fireSelectedWeapon();
      if(key[cmEventWeaponCommand.MINICANNON]){
         int com=cmEventWeaponCommand.SELECT;
         int arg=cmEventWeaponCommand.MINICANNON;
         player.addEvent(new cmEventWeaponCommand(world.getTime(),com,arg));
      }
      if(key[cmEventWeaponCommand.MISSILE]){
         int com=cmEventWeaponCommand.SELECT;
         int arg=cmEventWeaponCommand.MISSILE;
         player.addEvent(new cmEventWeaponCommand(world.getTime(),com,arg));
      }
      if(key[cmEventWeaponCommand.BOMB]){
         int com=cmEventWeaponCommand.SELECT;
         int arg=cmEventWeaponCommand.BOMB;
         player.addEvent(new cmEventWeaponCommand(world.getTime(),com,arg));
      }
   }

   protected long lastUpdate;
   static double viewAngle;
   static double viewDistance;
   static double gridSize;
}

