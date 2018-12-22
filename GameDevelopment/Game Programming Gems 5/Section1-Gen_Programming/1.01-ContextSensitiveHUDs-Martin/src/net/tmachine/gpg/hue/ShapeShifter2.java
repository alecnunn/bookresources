package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.event.*;
import java.awt.geom.*;
import java.util.*;

/**
 * Simple HUD which displays a "your mouse cursor is currently floating over THIS shape"
 * selection-box style thing, and includes a tool which will allow you to drag shapes around
 * the document, even multiple shapes at once (if they overlap and can be dragged in a
 * single gesture).
 * <P>
 * Not very user-friendly, but a basic example.
 * <P>
 * Note how it uses the ContextManager to find out the cursor position, and uses that to
 * decide which shapes are currently under the cursor; this is instead of listening to all
 * input events. It would be possible to split this HUD into it's rendering and tool components
 * and have the input part ignoring the cursor position inside/outside shapes (using some
 * abstracted concept of "selected shape" in the ContextManager) and have the display
 * part not needing any information on input events, instead deriving cursor co-ords indirectly
 * from the context, as it does now.
 */
public class ShapeShifter2 extends BaseHUD
{
	protected ArrayList movingObjects;
	/** Co-ords where a drag-translate was started */
	protected int sx, sy;
	/** Co-ords where a drag-translate was last updated (so we can work out how far to shift it) */
	protected int lastx, lasty;
	
	public ShapeShifter2()
	{
		name = "Shape Shifter";
		movingObjects = new ArrayList();
	}
	
	public void paint( Graphics g )
	{
		Graphics2D g2d = (Graphics2D) g;
		Document doc = manager.getContext().getDocument();
		int[] cursorPosition = manager.getContext().getCursorPosition();
		
		ArrayList floatedObjects = doc.getShapesAtPoint(cursorPosition);
		
		g.setColor( Color.green );
		
		for( Iterator iter = floatedObjects.iterator(); iter.hasNext(); )
		{
			Shape element = (Shape) iter.next();
			
			Rectangle bounds = element.getBounds();
			bounds.add( bounds.getMinX()-5, bounds.getMinY()-5 );
			bounds.add( bounds.getMaxX()+5, bounds.getMaxY()+5 );
			g2d.draw( bounds  );
		}
	}
	
	/**
	 * Finds all shapes under the mouse cursor, and flags them internally as being "currently
	 * being moved"
	 * <P>
	 * A mouse press over an empty area will clear the "currently being moved" list
	 * 
	 * @see #mouseDragged(MouseEvent)
	 */
	public void mousePressed( MouseEvent e )
	{
		Document doc = manager.getContext().getDocument();
		int[] cursorPosition = manager.getContext().getCursorPosition();
		sx = cursorPosition[0];
		sy = cursorPosition[1];
		lastx = sx;
		lasty = sy;
		
		movingObjects = doc.getShapesAtPoint(cursorPosition);
	}
	
	/**
	 * Compares the mouse cursor position to it's last known position, and translates
	 * all "currently being moved" shapes by the same relative distance
	 * 
	 * @see #mousePressed(MouseEvent)
	 */
	public void mouseDragged( MouseEvent e )
	{
		Document doc = manager.getContext().getDocument();
		int[] cursorPosition = manager.getContext().getCursorPosition();
		
		int dx = cursorPosition[0] - lastx;
		int dy = cursorPosition[1] - lasty;
		AffineTransform translate = AffineTransform.getTranslateInstance(dx, dy);
		for( Iterator iter = movingObjects.iterator(); iter.hasNext(); )
		{
			Area element = (Area) iter.next();
			element.transform(translate);
		}
		
		lastx = cursorPosition[0];
		lasty = cursorPosition[1];
		if( movingObjects.size() > 0 )
			manager.repaint();
	}
}