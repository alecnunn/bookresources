package net.tmachine.gpg.hue;

import java.awt.*;
import java.awt.event.*;

/**
 * The base type for all HUD implementations; contains the major callbacks needed by the
 * heads-up-editing system.
 */
public interface HUD extends MouseListener, MouseMotionListener
{
	/**
	 * Callback when added to a HUD manager to give the HUD a reference
	 * it can use to control or delegate to other HUDs etc
	 * 
	 * @param am
	 */
	public void setManager( HUDManager am );
	
	/**
	 * If this HUD is visible, it will be asked to paint to the supplied display surface
	 * during repaints
	 * 
	 * @param g
	 */
	public void paint( Graphics g );
	
	/**
	 * Each HUD should have a unique user-readable name; must be unique, since it is
	 * used as a key in Maps
	 * 
	 * @return unique name of this HUD class
	 */
	public String getName();
	
	/** Used by the HUD-manager to inform this HUD when it has been made
	 * visible or hidden
	 * @param b true if HUD is now visible, false otherwise
	 */
	public void isVisible( boolean b );
	
	/**
	 * Simply returns the value of the visibility flag
	 * 
	 * @return
	 */
	public boolean isVisible();
	
	/**
	 * Used by the HUD-manager to inform the HUD it is no longer active, i.e. even if
	 * it is visible it will NOT receive input events
	 * 
	 * @param b
	 */
	public void isActive( boolean b );
	
	/**
	 * Simply returns the value of the active flag
	 * @return
	 */
	public boolean isActive();
}