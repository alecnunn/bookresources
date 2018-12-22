package net.tmachine.gpg.hue;

import java.awt.event.*;

import javax.print.*;
import javax.swing.*;

/**
 * All input for the viewport is directed to this class, enabling it to keep track
 * of cursor position, current button and keystates, etc.
 * <p>
 * 
 */
public class ContextManager implements MouseListener, MouseMotionListener
{
	protected int mx, my;
	protected JComponent controlSurface;
	protected Document currentDocument;
	
	/**
	 * The Document is a simple MVC Model containing data on what shapes there are,
	 * where they are, etc; it has all the data that would be "saved" if there were a load/save
	 * feature in this demo!
	 * 
	 * @param d
	 */
	public void setDocument( Document d )
	{
		currentDocument = d;
	}
	
	/**
	 * The Document is a simple MVC Model containing data on what shapes there are,
	 * where they are, etc; it has all the data that would be "saved" if there were a load/save
	 * feature in this demo!
	 * 
	 */
	public Document getDocument()
	{
		return currentDocument;
	}
	
	/**
	 * Informs this class which component it has to listen to to get input events etc so that
	 * it can correctly maintain it's context
	 * 
	 * @param c Generally the main renderer component (i.e. this class will listen to key and
	 * mouse events that happen on the main renderer)
	 */
	public void setControlComponent( JComponent c )
	{
		if( controlSurface != null )
		{
			controlSurface.removeMouseListener( this );
			controlSurface.removeMouseMotionListener( this );
		}
		controlSurface = c;
		
		controlSurface.addMouseListener( this );
		controlSurface.addMouseMotionListener( this );
	}
	
	public void mouseClicked( MouseEvent e )
	{
	}
	/** Automatically updates its knowledge of the mouse cursor position */
	public void mouseDragged( MouseEvent e )
	{
		setRelativeCursorPosition(e.getX(), e.getY());
	}
	public void mouseEntered( MouseEvent e )
	{
	}
	public void mouseExited( MouseEvent e )
	{
	}
	/** Automatically updates its knowledge of the mouse cursor position */
	public void mouseMoved( MouseEvent e )
	{
		setRelativeCursorPosition(e.getX(), e.getY());
	}
	public void mousePressed( MouseEvent e )
	{
	}
	public void mouseReleased( MouseEvent e )
	{
	}
	
	/**
	 * 
	 * @return the (x,y) co-ords of the cursor in the same co-ord system
	 * used by the painting/rendering logic
	 */
	public int[] getCursorPosition()
	{
		return new int[] { mx, my };
	}
	
	/**
	 * Sets cursor position ignoring any special transformations performed on
	 * the rendering surface (this method then automatically applies those
	 * transformations to the incoming co-ords before storing them)
	 * <P>
	 * Needed because Sun's windowing system for Java is incapable of
	 * maintaining a single consistent co-ordinate space for rendering and
	 * events.
	 * <P>
	 * TODO: doesn't currently do anything special; transformed-painting-surface
	 * support isn't added yet
	 * 
	 * @param x
	 * @param y
	 */
	public void setRelativeCursorPosition( int x, int y)
	{
		mx = x;
		my = y;
		//System.out.println( "cursor position set to ("+x+", "+y+")");
	}
}