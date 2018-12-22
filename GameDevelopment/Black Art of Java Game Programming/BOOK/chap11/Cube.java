import java.awt.*;
import java.net.*;
import java.io.*;
/**
 * A rotating cubes applet
 * .. putting the classes to work with a quick and dirty
 * applet.
 */
public class Cube extends fNoFlickerApplet implements Runnable{
	fGenericCamera camera;
	fPoint3d CamPos;
	fAngle3d CamAngle;

	fPolyhedron cube;
	fPolyhedronInstance cubeInstance[];
	fPoint3d pos[];
	fAngle3d agl;

	Thread myThread;
   /**
    * initiate the applet.
    */
	public void init(){
	   //--
      //-- create a camera
	   //--
		camera=new fGenericCamera(400,400,Math.PI/2);
      CamPos=new fPoint3d(0,0,5);
      CamAngle=new fAngle3d();
      
      //--
		//-- load a model from the file cube.f3d
      //--
		try{
			InputStream is=new URL(getCodeBase(),"cube.f3d").openStream();
			cube=new fConvexPolyhedron(is);
		} catch(Exception e){e.printStackTrace();}
		
      //-- create 9 instances of the cube
	   cubeInstance=new fPolyhedronInstance[9];
		for(int n=0; n<9; n++){
   		cubeInstance[n]=new fPolyhedronInstance(cube);
		}
		
      //--
      //-- create the positions and angle
		//--
	   pos=new fPoint3d[9];
	   int n=0;
	   for(int y=-5; y<=5; y+=5){
	      for(int x=-5; x<=5; x+=5){
         	pos[n]=new fPoint3d(x,y,0);
      		n++;
	      }
	   }
		agl=new fAngle3d();
		//--
      //-- start the thread
		//--
		myThread=new Thread(this);
		myThread.start();
	}

	public void run(){
		while(true){
		   //--
		   //-- sleep 1/10 of a second
		   //--
			try {
			   myThread.sleep(100);
			} catch ( InterruptedException e) {}
			
			//--
			//-- update the angle of the models
			//--
			agl.x+=Math.PI/20; agl.y+=Math.PI/30;
			
			//--
         //-- update camera angle and position
			//--
      	CamPos.z+=0.2; CamAngle.z+=Math.PI/50;
      	camera.setOrientation(CamPos,CamAngle);
      	
      	//--
      	//-- request a repaint
      	//--
			repaint();
		}
	}
	public void start(){
		if(myThread==null){
			myThread=new Thread(this);
			myThread.start();
		}
	}
	public void stop(){
		if(myThread!=null){
			myThread.stop();
			myThread=null;
		}
	}

	public void paint(Graphics g){
	   //-- clear screen
   	g.clearRect(0,0,size().width,size().height);
   	
   	//--
   	//-- paint the models
   	//--
   	for(int n=0; n<9; n++){
   		cubeInstance[n].setOrientation(pos[n],agl);
   		cubeInstance[n].paint(g,camera);
   	}
	}
}
