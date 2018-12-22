package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

/**
 * @
 */
public class ShapeShifter extends BaseHUD
{
	protected Rectangle bounds;
	
	public void paint( Graphics g )
	{
		Graphics2D g2d = (Graphics2D) g;
		
		g.setColor( Color.green );
		
		if( bounds != null )
		{
			bounds.add( bounds.getMinX()-5, bounds.getMinY()-5 );
			bounds.add( bounds.getMaxX()+5, bounds.getMaxY()+5 );
			g2d.fill( bounds  );
		}
	}
	
	public void mouseMoved( MouseEvent e )
	{
		Document doc = manager.getContext().getDocument();
		
		if( bounds != null )
			manager.repaint();
		bounds = null;
		for( Iterator iter = doc.getAllShapes().iterator(); iter.hasNext(); )
		{
			Shape element = (Shape) iter.next();
			
			if( element.contains(e.getX(), e.getY()))
			{
				System.out.println( "element contains "+e.getX()+", "+e.getY() );
				bounds = element.getBounds();
			}
			else
				System.out.println( "element does not contain "+e.getX()+", "+e.getY() );
		}
		
		if( bounds != null )
			manager.repaint();
	}
}
