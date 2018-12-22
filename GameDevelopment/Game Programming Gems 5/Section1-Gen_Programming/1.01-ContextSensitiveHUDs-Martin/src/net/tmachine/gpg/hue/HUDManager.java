package net.tmachine.gpg.hue;

import java.awt.event.*;
import java.util.*;

import javax.swing.*;

/**
 * The main Model and Controller (in MVC terms) for managing the HUDs system.
 * <P>
 * Keeps track of all HUDS, and contains the preferred methods for changing their
 * state (allowing you to hook into just one place to capture state changes etc, e.g.
 * for propagation to listeners)
 */
public class HUDManager implements MouseListener, MouseMotionListener
{
	protected JComponent display;
	protected ContextManager context;
	
	/** HUD instances that are currently listening for any input events */
	protected ArrayList inputListeningHUDs;
	
	protected LinkedList activeHUDGroupsStack;
	
	protected HashMap allHUDs;
	
	/**
	 * You MUST call setDisplayComponent before using this new instance
	 *
	 * @see #setDisplayComponent(JComponent)
	 */
	public HUDManager()
	{
		allHUDs = new HashMap();
		inputListeningHUDs = new ArrayList();
		activeHUDGroupsStack = new LinkedList();
	}
	
	/**
	 * Callback for HUDs to instruct the manager that they have changed their paint state
	 * and need a repaint of the main renderer
	 */
	public void repaint()
	{
		display.repaint();
	}
	
	/**
	 * The context that HUDs can query to find out e.g the most recent cursor co-ordinates
	 * 
	 * @return
	 */
	public ContextManager getContext()
	{
		return context;
	}
	
	public void setContext( ContextManager cm )
	{
		context = cm;
	}
	
	/**
	 * Informs this class which display component is the main renderer
	 * 
	 * @param c
	 */
	public void setDisplayComponent( JComponent c )
	{
		if( display != null )
		{
			display.removeMouseListener( this );
			display.removeMouseMotionListener( this );
		}
		display = c;
		
		display.addMouseListener( this );
		display.addMouseMotionListener( this );
	}
	
	/**
	 * Adds the HUD to the set of current HUDs, and sets the HUD's reference to it's parent
	 * manager to point to this instance
	 * 
	 * @param h
	 */
	public void addHUD( HUD h )
	{
		h.isVisible( true );
		allHUDs.put( h.getName(), h );
		h.setManager(this);
		inputListeningHUDs.add( h );
	}
	
	/**
	 * Fetches all HUDs, regardless of status
	 * 
	 * @return
	 */
	public ArrayList getAllHUDs()
	{
		ArrayList al = new ArrayList();
		al.addAll( allHUDs.values() );
		return al;
	}
	
	/**
	 * Fetches only HUDs that are visible, i.e are being painted to the main renderer
	 * 
	 * @return
	 */
	public ArrayList getVisibleHUDs()
	{
		ArrayList al = getAllHUDs();
		for( Iterator iter = al.iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			if( element.isVisible() )
				;
			else
				iter.remove();
		}
		return al;
	}
	
	/**
	 * Renders the supplied HUD invisible, i.e it doesn't participate in repaints
	 * @see #showHUD(HUD)
	 */
	public void hideHUD( HUD h )
	{
		h.isVisible(false);
	}
	
	/**
	 * Renders the supplied HUD visible, i.e it's paint callback is invoked on all repaints
	 * 
	 * @see #hideHUD(HUD)
	 */
	public void showHUD( HUD h )
	{
		h.isVisible( true );
	}
	
	/**
	 * De-activates the HUD, so that it gets NO input events, and cannot respond as a tool/control
	 * Inactive HUDs can still paint, if they are also visible.
	 * 
	 * @see #activateHUD(HUD)
	 */
	public void deActivateHUD( HUD h )
	{
		h.isActive( false );
	}
	
	/**
	 * Activates the HUD; since a HUD cannot be invisible and active, it is implicitly made
	 * visible at the same time; active HUDs receive input events, e.g mouse movement
	 * and keyboard presses.
	 * 
	 * @see #deActivateHUD(HUD)
	 * @see #showHUD(HUD)
	 */
	public void activateHUD( HUD h )
	{
		if( ! h.isVisible() )
			h.isVisible( true );
		h.isActive( true );
	}
	
	/**
	 * Only returns HUDs that are both active and visible (assumes that invisible HUDs, which
	 * cannot give feedback on what the user is doing, should not be counted active - but leaves
	 * them "isActive() == true" so that as soon as they are made visible again they will
	 * automatically become active)
	 * 
	 * @return
	 */
	public ArrayList getActiveHUDs()
	{
		ArrayList huds = (ArrayList) inputListeningHUDs.clone();
		
		for( Iterator iter = huds.iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			if( ! element.isActive()
			|| ! element.isVisible() )
				iter.remove();
		}
		return huds;
	}
	
	/**
	 * Enables a HUD to temporarily mask out all currently active HUD's in favour of one
	 * in particular; once that HUD has completed it's action, it should call exitExclusiveHUD
	 * to re-enable all previously active HUDs.
	 * <P>
	 * This is a convenient way of making tools that e.g. need to engage in a multiple-stage
	 * dialog with the user within the edit window, without risking propagating input events
	 * to other HUDS.
	 * <P>
	 * This is typically used for complex actions, such as creating a 3D object: the gesture
	 * that triggers the create action within the main renderer will mask out everything else
	 * because it needs to exclusively capture the next few clicks (e.g. one drag to set size
	 * in one plane, followed by a click or release, then a second drag or move followed by
	 * a click to set the size in the perpendicular plane, since a 3D object cannot easily be
	 * sized using a 2D mouse cursor without this multiple-stage interaction).
	 * 
	 * @param h the HUD to make active whilst all others are masked out
	 * @see #exitExclusiveActiveHUD()
	 */
	public void setExclusiveActiveHUD( HUD h )
	{
		if( ! allHUDs.containsKey(h.getName()))
			throw new IllegalArgumentException( "Specified HUD is not a member of this HUD manager, HUD = "+h );
		
		activeHUDGroupsStack.addLast( inputListeningHUDs );
		inputListeningHUDs = new ArrayList();
		inputListeningHUDs.add( h );
		
		repaint();
	}
	
	/**
	 * Restores all the previously active HUDs to their active state
	 * 
	 * @see #setExclusiveActiveHUD(HUD)
	 */
	public void exitExclusiveActiveHUD()
	{
		if( activeHUDGroupsStack.size() < 1 )
			throw new UnsupportedOperationException( "Cannot invoke this method; there was no exclusive HUD already set");
		
		inputListeningHUDs = (ArrayList) activeHUDGroupsStack.removeLast();
		
		repaint();
	}
	
	/** Delegates to all the currently active HUDs that are listening to
	 * inputs
	 */
	public void mouseClicked( MouseEvent e )
	{
		for( Iterator iter = getActiveHUDs().iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.mouseClicked(e);
		}
	}
	
	/** Delegates to all the currently active HUDs that are listening to
	 * inputs
	 */
	public void mouseDragged( MouseEvent e )
	{
		for( Iterator iter = getActiveHUDs().iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.mouseDragged(e);
		}
		repaint();
	}
	
	/** Delegates to all the currently active HUDs that are listening to
	 * inputs
	 */
	public void mouseEntered( MouseEvent e )
	{
		for( Iterator iter = getActiveHUDs().iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.mouseEntered(e);
		}
	}
	
	/** Delegates to all the currently active HUDs that are listening to
	 * inputs
	 */
	public void mouseExited( MouseEvent e )
	{
		for( Iterator iter = getActiveHUDs().iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.mouseExited(e);
		}
	}
	
	/** Delegates to all the currently active HUDs that are listening to
	 * inputs
	 */
	public void mouseMoved( MouseEvent e )
	{
		for( Iterator iter = getActiveHUDs().iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.mouseMoved(e);
		}
		
		repaint();
	}
	
	/** Delegates to all the currently active HUDs that are listening to
	 * inputs
	 */
	public void mousePressed( MouseEvent e )
	{
		for( Iterator iter = getActiveHUDs().iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.mousePressed(e);
		}
	}
	
	/** Delegates to all the currently active HUDs that are listening to
	 * inputs
	 */
	public void mouseReleased( MouseEvent e )
	{
		for( Iterator iter = getActiveHUDs().iterator(); iter.hasNext(); )
		{
			HUD element = (HUD) iter.next();
			element.mouseReleased(e);
		}
	}
}