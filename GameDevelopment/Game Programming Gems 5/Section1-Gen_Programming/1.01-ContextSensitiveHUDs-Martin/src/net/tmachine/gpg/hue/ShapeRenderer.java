package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

/**
 * Very simple renderer that simply renders every shape in the base document using
 * a unique colour for each.
 * <P>
 * Displays some very  basic instructions on how to get started iff the document is empty
 * (which is very bad OOP design, but it makes it easier to get started with the demo!)
 */
public class ShapeRenderer extends BaseHUD implements HUD
{
	public ShapeRenderer()
	{
		name = "Basic shape painter";
	}
	
	public void paint( Graphics g )
	{
		Document doc = manager.getContext().getDocument();
		Graphics2D g2d = (Graphics2D) g;
		int numShapes =  doc.getAllShapes().size();
		
		Color c = null;
		float currentHue = 0f;
		for( Iterator iter = doc.getAllShapes().iterator(); iter.hasNext(); )
		{
			Shape element = (Shape) iter.next();
			c = Color.getHSBColor( currentHue, 1f, 1f);
			g.setColor(c);
			g2d.fill(element );
			
			// Note: adding 0.111 to hue, which auto-wraps-around, causes it to cycle
			//   through hundreds of subtly different shades of a sequence of 9 very
			//   different colours
			currentHue = currentHue + 0.137f;
		}
		
		if( numShapes < 1 )
		{
			int w = manager.display.getWidth();
			int h = manager.display.getHeight();
			FontMetrics metrics = g.getFontMetrics();
			g.setColor(Color.red);
			String msg = "No shapes in document; create some shapes!";
			g.drawString( msg, (w - metrics.stringWidth(msg))/2, h/2 - 25 );
			
			msg = "Click on the A and V circles for \"Creator\" HUD, so they are full";
			g.drawString( msg, (w - metrics.stringWidth(msg))/2, h/2 );
			
			msg = ", then drag out some shapes";
			g.drawString( msg, (w - metrics.stringWidth(msg))/2, h/2 + 25 );
		}
	}
	
	public void mouseClicked( MouseEvent e )
	{
	}

	public void mouseEntered( MouseEvent e )
	{
	}

	public void mouseExited( MouseEvent e )
	{
	}

	public void mousePressed( MouseEvent e )
	{
	}

	public void mouseReleased( MouseEvent e )
	{
	}

	public void mouseDragged( MouseEvent e )
	{
	}

	public void mouseMoved( MouseEvent e )
	{
	}

}
