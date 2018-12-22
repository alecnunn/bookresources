package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.geom.*;
import java.util.*;

/**
 * 
 */
public class Document
{
	protected ArrayList shapes;

	public Document()
	{
		shapes = new ArrayList();
	}

	public ArrayList getAllShapes()
	{
		return shapes;
	}
	
	/**
	 * Converts all added shapes to Area instances if they are not already, since Sun's 2D
	 * API was badly designed and prevents you from applying AffineTransforms to most
	 * shapes.
	 * <p>
	 * Without the auto-conversion, it is hard to write decent GUIs because you
	 * are <b>not allowed</b> to use the AffineTransform class, so you cannot do rotations
	 * etc without implementing it all by hand, and creating large switch statements to deal
	 * with all possible shapes (which all have different API's)
	 * 
	 * @param s
	 */
	public void addShape( Shape s )
	{
		if( s instanceof Area )
			shapes.add( s );
		else
			shapes.add( new Area(s) );
	}
	
	public ArrayList getShapesAtPoint( int[] point )
	{
		ArrayList hits = new ArrayList();
		
		for( Iterator iter = getAllShapes().iterator(); iter.hasNext(); )
		{
			Shape element = (Shape) iter.next();

			if( element.contains(point[0], point[1]) )
			{
				//System.out.println( "element contains " + cursorPosition[ 0 ] + ", " + cursorPosition[ 1 ] );
				hits.add( element );
			}
			else
				;//System.out.println( "element does not contain "+ cursorPosition[ 0 ] + ", " + cursorPosition[ 1 ] );
		}
		
		return hits;
	}
}