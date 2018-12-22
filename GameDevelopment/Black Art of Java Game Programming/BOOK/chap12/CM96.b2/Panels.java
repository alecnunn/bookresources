import java.util.*;
import java.awt.*;

class cmPanel extends Panel{
   void update(double dt){
   }
}

class cmDisplayPanel extends cmPanel {
   cmDisplayPanel(cmAbstractVehicle theVehicle){
      this.theVehicle=theVehicle;
      setLayout(new BorderLayout());
      makeButtons();
   }

   cmDisplayPanel(cmAbstractVehicle theVehicle,String str){
      this.theVehicle=theVehicle;
      setLayout(new BorderLayout());
      makeButtons();
      handleButtonAction(str);
   }

   protected void makeButtons(){
      Panel panel=new Panel();
      panel.add(new Button("R"));
      panel.add(new Button("C"));
      panel.add(new Button("G"));
      panel.add(new Button("-"));
      add("North",panel);
      currentDisplay="";
   }

   protected boolean handleButtonAction(String label){
      if(label.equals(currentDisplay)) {
         fWorld.debug(this,"same display");
         return true;
      }
      if(label.equals("R")){
         display(new cmRadarPanel(theVehicle,100));
      } else if(label.equals("G")){
         display(new cmGaugesPanel(theVehicle));
      } else if(label.equals("-")){
         clear();
      } else if(label.equals("C")){
         fTrackerCamera cam=new fTrackerCamera(theVehicle.getWorld(),1.2,150,20,1,0.8,theVehicle,new fAngle3d(),new fPoint3d(0,4,4));
         fPoint3d light=new fPoint3d(0,0,-1);
         light.rotateAboutXaxis(-Math.PI/4);
         light.rotateAboutYaxis(-Math.PI/4);
         light.normalize(1);
         cam.setAmbientLight(light);
         
         cmPanel pan=new cmCameraPanel(cam);
         display(pan);
      }
      currentDisplay=label;
      return true;
   }

   public synchronized boolean action(Event ev, Object arg){
      if(ev.target instanceof Button){
         return handleButtonAction((String)arg);
      }
      return false;
   }

   void clear() {
      if(theGuestPanel!=null){
         remove(theGuestPanel);
         theGuestPanel=null;
      }
   }

   void display(cmPanel panel){
      clear();
      theGuestPanel=panel;
      add("Center",panel);
      validate();
   }

   void update(double dt){
      if(theGuestPanel!=null){
         theGuestPanel.update(dt);
      }
   }

	public void paint (Graphics g) {
      if(theGuestPanel!=null){
         theGuestPanel.repaint();
      }
	}
   
   protected cmPanel theGuestPanel;
   protected cmAbstractVehicle theVehicle;
	protected String currentDisplay;
}

class cmGaugesPanel extends cmLogicPanel {
   cmGaugesPanel(cmAbstractVehicle theVehicle){
      super(0,0,100,100);
      this.theVehicle=theVehicle;
      setLayout(new GridLayout(2,2));
      add(vel=new cmVelocityGauge(theVehicle));
      add(alt=new cmAltitudeGauge(theVehicle));
      add(wep=new cmWeaponsGauge(theVehicle));
      add(new Panel());
   }

	void update(double dt){
	   vel.update(dt);
	   alt.update(dt);
	   wep.update(dt);
	}

   protected cmAbstractVehicle theVehicle;
	protected cmPanel vel,alt,wep;
}

class cmWeaponsGauge extends cmLogicPanel {
   cmWeaponsGauge(cmAbstractVehicle theVehicle){
      super(0,0,10,5);
      this.theVehicle=theVehicle;
      lastUpdated=5;
   }

   public void paint(Graphics g){
      g.setFont(largeFont);
      int sel=theVehicle.selectedWeapon;
      int nbr=theVehicle.myWeapons.size();
      int row=1;
      for(int n=0;n<nbr;n++){
         cmAbstractWeapon wep=(cmAbstractWeapon)theVehicle.myWeapons.elementAt(n);
         if(wep.getAmmo()>0){
            if(n==sel) g.setColor(Color.red); else g.setColor(Color.black);
            g.drawString(wep.getName(),dcX(0),dcY(row));
            g.setColor(Color.darkGray);
            g.drawString(" "+wep.getAmmo(),dcX(8),dcY(row));
            row++;
         }
      }

   }

	void update(double dt){
	   lastUpdated+=dt;
	   if(lastUpdated>1){
   	   repaint();
	      lastUpdated=0;
	   }
	}

   protected cmAbstractVehicle theVehicle;
   protected double lastUpdated;
}

class cmVelocityGauge extends cmLogicPanel {
   cmVelocityGauge(cmAbstractVehicle theVehicle){
      super(-1,-1,1,1);
      this.theVehicle=theVehicle;
      lastUpdated=5;
   }

	public void paint(Graphics g) {
      int vel=(int)((fVelocityVector)theVehicle.getdPosition()).getVelocity();

      g.setColor(Color.cyan);
	   g.drawString("Vel="+vel*3.6+" km/h",dcX(-1),dcY(1));

      fPoint3d v=new fPoint3d(-0.8,0,0);
      v.rotateAboutYaxis(-Math.PI*vel/50);
      g.setColor(Color.black);
      g.drawLine(dcX(0),dcY(0),dcX(v.x),dcY(v.z));
	}
	
	void update(double dt){
	   lastUpdated+=dt;
	   if(lastUpdated>1){
   	   repaint();
         lastUpdated=0;
	   }
	}

   protected cmAbstractVehicle theVehicle;
	protected double lastUpdated;
	protected int lastVel;
}

class cmAltitudeGauge extends cmLogicPanel {
   cmAltitudeGauge(cmAbstractVehicle theVehicle){
      super(-1,50, 1,-1);
      this.theVehicle=theVehicle;
      lastUpdated=5;
   }

	public void paint(Graphics g) {
      lastUpdated=0;
      int a=(int)theVehicle.getPosition().y;
      
      g.setColor(Color.cyan);
	   g.drawString("Alt="+a+" m",dcX(-1),dcY(-1));
      g.setColor(Color.black);
      g.drawLine(dcX(-.8),dcY(a),dcX(.8),dcY(a));
      g.setColor(Color.darkGray);
      g.drawLine(dcX(-.8),dcY(lastAlt),dcX(.8),dcY(lastAlt));
      lastAlt=a;
	}
	
	void update(double dt){
	   lastUpdated+=dt;
	   if(lastUpdated>1){
   	   repaint();
         lastUpdated=0;
	   }
	}

   protected cmAbstractVehicle theVehicle;
	protected double lastUpdated;
	protected int lastAlt;
}

class cmRadarPanel extends cmLogicPanel {
	cmRadarPanel (fObject host, double radius) {
	   super(-radius,-radius,radius,radius);
		theWorld=host.getWorld();
		myRadius=radius;
		theHostObj=host;
      lastUpdated=5;
		theObjects=new Vector(100,100);
      makeButtons();
   }

   protected void makeButtons(){
      setLayout(new BorderLayout());
      Panel panel=new Panel();
      panel.add(new Button("+"));
      panel.add(new Button("-"));
      add("South",panel);
   }

   public synchronized boolean action(Event ev, Object arg){
      if(ev.target instanceof Button){
         String label=(String)arg;
         if(label.equals("+")){
            setRange(myRadius*1.2);
            return true;
         } else if(label.equals("-")){
            setRange(myRadius*0.8);
            return true;
         }
      }
      return false;
   }

	public void paint(Graphics g) {
      theObjects.removeAllElements();
      //-- get the position and angle of the host      
      fPoint3d hp=theHostObj.getPosition();
      double ay=theHostObj.getAngle().y;
      //-- get all visible objects
      theWorld.getAllObjectsInRadius(theHostObj.getPosition(),myRadius,theObjects);
		   
      int nbr=theObjects.size();

      hp.negate();
               
      for(int n=0;n<nbr;n++){
         fObject obj=(fObject)theObjects.elementAt(n);
         fPoint3d pos=obj.getPosition();
         pos.plus(hp);
         pos.rotateAboutYaxis(-ay);
		            
         double rad=obj.getPolyhedronInstance().getBoundingRadius();
         g.drawOval(dcX(pos.x-rad),dcY(pos.z-rad),dcScaleX(rad*2),dcScaleY(rad*2));
      }
	}

	public void setRange(double r){
	   myRadius=r;
	   setDimensions(-r,-r,r,r);
	}

	void update(double dt){
	   lastUpdated+=dt;
	   if(lastUpdated>1){
   	   repaint();
	      lastUpdated=0;
	   }
	}

	protected Vector theObjects;
	protected fWorld theWorld;
	protected double myRadius;
	protected fObject theHostObj;
	protected double lastUpdated;
}

class cmCameraPanel extends cmPanel {
   
	cmCameraPanel (fTrackerCamera cam) {
      setLayout(new BorderLayout());
      Panel panel=new Panel();
      panel.add(new Button("L"));
      panel.add(new Button("F"));
      panel.add(new Button("U"));
      panel.add(new Button("D"));
      panel.add(new Button("B"));
      panel.add(new Button("R"));
      add("South",panel);
		theCamera=cam;
	}
	
   public synchronized boolean action(Event ev, Object arg){
      if(ev.target instanceof Button){
         String com=(String)arg;
         if(com.equals("B")){
            theCamera.setRelativePosition(new fPoint3d(0,4,-4));
            theCamera.setRelativeAngle(new fAngle3d(0,Math.PI,0));
            return true;
         }
         if(com.equals("F")){
            theCamera.setRelativePosition(new fPoint3d(0,4,4));
            theCamera.setRelativeAngle(new fAngle3d(0,0,0));
            return true;
         }
         if(com.equals("L")){
            theCamera.setRelativePosition(new fPoint3d(4,4,0));
            theCamera.setRelativeAngle(new fAngle3d(0,Math.PI/2,0));
            return true;
         }
         if(com.equals("R")){
            theCamera.setRelativePosition(new fPoint3d(-4,4,0));
            theCamera.setRelativeAngle(new fAngle3d(0,-Math.PI/2,0));
            return true;
         }
         if(com.equals("D")){
            fAngle3d agl=theCamera.getRelativeAngle();
            agl.x+=Math.PI/6;
            theCamera.setRelativeAngle(agl);
            return true;
         }
         if(com.equals("U")){
            fAngle3d agl=theCamera.getRelativeAngle();
            agl.x-=Math.PI/6;
            theCamera.setRelativeAngle(agl);
            return true;
         }

      }
      return false;
   }

   private Image buffer;
   private Graphics bufferG;
   private Dimension bufferDim;

   public final void update(Graphics g){
      if (getPeer()==null){
         fWorld.debug(this,"No peer. Bailing out of update.");
      }

      Dimension d=size();
      if( d==null || d.width==0 || d.height==0 ){
         fWorld.debug(this," Bogus dimension. Bailing out of update.");
         return;
      }

      if( buffer==null || d.width != bufferDim.width || d.height != bufferDim.height){
         buffer = createImage(d.width,d.height);
         bufferG = buffer.getGraphics();
         bufferDim = d;
      }
     
      bufferG.clearRect(0,0,d.width,d.height);
      theCamera.setScreenSize(d.width,d.height);
      theCamera.paint(bufferG);
      g.drawImage(buffer,0,0,null);
   }

	public void update (double dt) {
	   super.update(dt);
		theCamera.update(dt);
		if(getPeer()!=null){
     		update(getGraphics());
		} else{
         fWorld.debug(this,"No peer!! Bailing out of update.");
		}
	}

	protected fTrackerCamera theCamera;
}

class cmLogicPanel extends cmPanel {
   protected Dimension myDim;
   Font smallFont,mediumFont,largeFont;
   private Image buffer;
   private Graphics bufferG;
   
   cmLogicPanel(){
      xmin=ymin=0;
      xmax=ymax=1;
      calcScale();
   }
   
   cmLogicPanel(double xmin0,double ymin0,double xmax0,double ymax0){
      xmin=xmin0; ymin=ymin0; xmax=xmax0; ymax=ymax0;
      calcScale();
   }

	public final void update (Graphics g) {
		Dimension d=size();
		if( (myDim.width != d.width) || (myDim.height != d.height) ){

		   calcScale();

         smallFont =new Font("TimesRoman",Font.PLAIN,myDim.height/20);
			mediumFont=new Font("TimesRoman",Font.PLAIN,myDim.height/15);
			largeFont =new Font("TimesRoman",Font.PLAIN,myDim.height/7);
			g.setFont(mediumFont);
		}
	   g.clearRect(0,0,myDim.width,myDim.height);
      paint(g);
	}

   public void setDimensions(double xmin,double ymin,double xmax,double ymax){
      this.xmin=xmin;
      this.ymin=ymin;
      this.xmax=xmax;
      this.ymax=ymax;
      calcScale();
   }
	
   private void calcScale(){
      myDim=size();
      xscale=myDim.width /(xmax-xmin);
      yscale=myDim.height/(ymax-ymin);
   }

   public int dcX(double logicX){
      return (int)((logicX-xmin)*xscale);
   }

   public int dcY(double logicY){
      return (int)((logicY-ymin)*yscale);
   }

   public int dcScaleX(double sx){
      return (int)(Math.abs(sx*xscale));
   }

   public int dcScaleY(double sy){
      return (int)(Math.abs(sy*yscale));
   }

   protected double xmin,ymin,xmax,ymax;
   protected double xscale,yscale;
}

