import java.awt.*;
import java.applet.*;

public class RotatingPoints extends Applet implements Runnable{
	//-- # points
	int pts;
	//-- the source points
	double x[],y[],z[];
	//-- the transformed points
	double xt[],yt[],zt[];
	//-- the angles 
	double Ax,Ay,Az;
	double da;
	//-- the thread
	Thread myThread;

	public void init(){
		pts=10;
		x =new double[pts]; y =new double[pts]; z =new double[pts];
		xt=new double[pts]; yt=new double[pts]; zt=new double[pts];
		//-- create some random 3d points
		for(int n=0;n<pts;n++){
			x[n]=Math.random()*100-50;
			y[n]=Math.random()*100-50;
			z[n]=Math.random()*100-50;
		}
		//-- set the angular velocity
		da=Math.PI/10;
		//-- start the thread
		myThread=new Thread(this);
		myThread.start();
	}
	public void run(){
		while(myThread!=null){
			try { myThread.sleep(100);} catch ( InterruptedException e) {}
			rotatePoints(x,y,z,xt,yt,zt,pts,Ax,Ay,Az);
			Az+=da; //Ay+=da*0.1;
			repaint();
		}
	}
	public void start(){
		if(myThread==null){ myThread=new Thread(this);myThread.start();}
	}
	public void stop(){
		if(myThread!=null){ myThread.stop(); myThread=null;}
	}
	public void paint(Graphics g){
		int Xo=size().width>>1,Yo=size().height>>1;
		for(int n=0;n<pts;n++){
			int x=(int)xt[n],y=(int)yt[n];
			//-- set the different colors
			g.setColor(new Color(n<<3,0,0));
			//-- draw a line from origot to center
			g.drawLine(Xo,Yo,Xo+x,Yo+y);
			//-- draw a little circle
			g.fillOval(Xo+x-4,Yo+y-4,8,8);
		}
	}
	//-- rotates points
	private void rotatePoints(double xs[],double ys[],double zs[],
	                            double xd[],double yd[],double zd[],
	                            int pts,double a,double b,double c){
		for(int n=0;n<pts;n++){
			double Xa,Ya,Za,Xb,Yb,Zb;
			
			//-- rotate the dource in x-y-plane
			Xa = xs[n]*Math.cos(a)-ys[n]*Math.sin(a);
			Ya = xs[n]*Math.sin(a)+ys[n]*Math.cos(a);
			Za = zs[n];

			//-- rotate the resulting point in the y-z-plane
			Xb = Xa;
			Yb = Ya*Math.cos(b)-Za*Math.sin(b);
			Zb = Ya*Math.sin(b)+Za*Math.cos(b);

			//-- rotate the resulting point in the z-x-plane
			xd[n] = Xb*Math.cos(c)+Zb*Math.sin(c);
			yd[n] = Yb;
			zd[n] = -Xb*Math.sin(c)+Zb*Math.cos(c);
		}
	}
}

