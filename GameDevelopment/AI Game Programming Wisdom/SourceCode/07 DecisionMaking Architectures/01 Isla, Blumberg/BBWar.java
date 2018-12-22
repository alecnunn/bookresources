package bb;

import java.awt.*;
import java.awt.geom.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

/*
This is the main BBWar application class, containg the "main" method.
If you want to play around with gross system parameters -- modifying the number
of cities per team, or the number or size of the resource pools, this is 
where to do it.

@author naimad
*/

public class BBWar extends JPanel {

	/*
	First, some annoying mouse nonsense.
	*/

    class MouseManager extends MouseAdapter implements MouseMotionListener {
        double lastx = 0.0;
        double lasty = 0.0;
        
        public void mousePressed(MouseEvent e) {
            lastx = e.getX();
            lasty = e.getY();
        }

        public void mouseMoved(MouseEvent e) {
            //do nothing
        }
        
        public void mouseDragged(MouseEvent e) {
			double dy = e.getY() - lasty;
            
            if ((e.getModifiers() & e.BUTTON1_MASK)!=0) {
            	double dx = e.getX() - lastx;
                translation.x(translation.x()+dx/scale);
                translation.y(translation.y()+dy/scale);
            }
            else if ((e.getModifiers() & e.BUTTON3_MASK)!=0) {
                if (dy >= 0) {
                    scale *= (1.0+(double)dy/50.0);
                }
                else {
                    scale /= (1.0+(-dy/50.0));
                }
            }
            
            lastx = e.getX();
            lasty = e.getY();
        }
    }

    int xres;
    int yres;

    Vec2 translation = new Vec2(0.0,0.0);
    double scale = 1.0;
    World world;

    /*
    The main constructor for the BBWar class.
    */
    public BBWar(int xres, int yres, World world) {
        super();
        this.xres = xres;
        this.yres = yres;
        setSize(xres, yres);

        this.world = world;
        
        MouseManager mm = new MouseManager();
        addMouseListener(mm);
        addMouseMotionListener(mm);
    }

    double time = 0.0;
    public void update() {
        time += 0.25;

        world.update(time);        
                
        repaint();

		//This sleep lets the rendering keep up with the
		//simulation loop. Otherwise we start to drop frames like
		//crazy.
        try { Thread.sleep(20); }
        catch(Exception e) {}
    }

	/*
	The central draw routine: this method gets called automatically by the 
	Java2 display update thread. We then go through all drawable objects and
	have them do their thing to the "Graphics2D" object that represents the
	drawable surface.
	*/

    Shape center = new Ellipse2D.Double(0.0,0.0, 10.0, 10.0);
    AffineTransform transform = new AffineTransform();
    public void paint(Graphics g) {
        Graphics2D g2 = (Graphics2D)g;

        g2.clearRect(0,0,getWidth(), getHeight());
        g2.translate(getWidth()/2, getHeight()/2);
        g2.scale(scale, scale);
        g2.translate(translation.x(), translation.y());
        
        world.updateGraphics(time, g2);
    }

    public final static void main(String[] s) {
        System.out.println("War!");
        
        JFrame bbwarFrame = new JFrame("BBWar!");
        bbwarFrame.getContentPane().setLayout(new BorderLayout());

        int xres = 400;
        int yres = 400;
        
        World world = new World(2);

        BBWar theGame = new BBWar(xres, yres, world);
        bbwarFrame.getContentPane().add("Center", theGame);

        bbwarFrame.setSize(xres, yres);
        bbwarFrame.setVisible(true);
        
        /*
        This is where we finally set up the two teams. Feel free to muck about with this
        code and see what happens ...
        */

		//-------------------------------TEAM 0-------------------------------------------

        Blackboard blackboard1 = new Blackboard(0);
        world.add(blackboard1);

        for (int c=0; c < 5; c++) {
        	City city1 = new City(0, new Vec2(500.0*Math.random(),1000.0*(Math.random()-0.5)), blackboard1, world);
        	world.add(city1);
        }

        DefenseTactician dtact = new DefenseTactician(0, new Vec2(600.0,10.0), blackboard1, world);
        world.add(dtact);

		//-------------------------------TEAM 1-------------------------------------------
        
        Blackboard blackboard2 = new Blackboard(1);
        world.add(blackboard2);
        
        for (int c=0; c < 5; c++) {
        	City city2 = new City(1, new Vec2(-500.0*Math.random(),1000.0*(Math.random()-0.5)), blackboard2, world);
        	world.add(city2);
        }

        DefenseTactician dtact2 = new DefenseTactician(1, new Vec2(-600.0,-10.0), blackboard2, world);
        world.add(dtact2);

        //--------------------------RESOURCES---------------------------
		//Like is many RTS games, there is a resource system. This determines where the 
		//resource pools are and how big they are.
        
        int NUM_RESOURCES = 10;
        
        for (int c=0; c < NUM_RESOURCES; c++) {
			Resource r = new Resource(new Vec2(1000.0*(Math.random()-0.5), 1000.0*(Math.random()-0.5)), 8000.0*Math.random());
			world.add(r);
        }
        
        
        //------------------------------MAIN LOOP-----------------------------
		//It's as simple as...
        
        while(true) {
            theGame.update();
        }
        
    }
}
