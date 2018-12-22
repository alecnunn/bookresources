package bb;

import java.awt.*;
import java.awt.geom.*;

/*
This represents a resource pool.

@author naimad
*/

public class Resource extends DrawableObject {

    double resources;
    public final static double CHUNK = 5.0;

    public Resource(Vec2 pos, double totalRes) {
    	super(pos);
    	resources = totalRes;
    }

	public double extractResources() {
		resources -= CHUNK;
		return CHUNK;
	}

    public Shape createShape() {
        return new Ellipse2D.Double(-5.0, -5.0, 10.0, 10.0);
    }    

    public void updateGraphics(double time, Graphics2D g2) {
    	((Ellipse2D.Double)drawShape).setFrame(-resources/160, -resources/160, resources/80, resources/80);
    	super.updateGraphics(time, g2);
    }
}