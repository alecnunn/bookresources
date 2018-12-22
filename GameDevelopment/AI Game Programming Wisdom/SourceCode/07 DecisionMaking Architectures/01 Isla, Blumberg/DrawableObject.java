package bb;

import java.awt.*;
import java.awt.geom.*;

/*
The abstract base class for all objects that can be drawn, whether they are
agents or resource pools or whatever.

@author naimad
*/

public abstract class DrawableObject {
    
    Shape drawShape = null;
    Vec2 pos = new Vec2();
    
    public DrawableObject(Vec2 pos) {
    	this.pos.set(pos);
        drawShape = createShape();
    }
    
    AffineTransform trans = new AffineTransform();
    public void updateGraphics(double time, Graphics2D g2) {
        g2.translate(pos.x(), pos.y());
        drawShape(time, g2);
        g2.translate(-pos.x(), -pos.y());
    }
    
    public void drawShape(double time, Graphics2D g2) {
        g2.fill(drawShape);
    }
    
    public abstract Shape createShape();
    
}