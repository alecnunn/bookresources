package net.tmachine.gpg.hue;

import java.awt.*;

import javax.swing.*;

/**
 * The automatically-run demo application that loads a few HUDs and lets you draw shapes
 * etc
 */
public class Demo
{
	public static void main( String[] args )
	{
		Document doc = new Document();
		//doc.addShape( new Rectangle(0, 0, 100, 100));
		
		JFrame frame = new JFrame( "Heads Up Editing Demo");
		frame.setDefaultCloseOperation( JFrame.DISPOSE_ON_CLOSE);
		
		MainRenderer display = new MainRenderer();
		
		ContextManager context = new ContextManager();
		context.setDocument(doc);
		context.setControlComponent( display );
		
		HUDManager manager = new HUDManager();
		manager.setDisplayComponent( display );
		manager.setContext(context);
		addHUDs( manager );
		
		display.setManager(manager);
		
		HUDList hudList = new HUDList( manager );
		frame.getContentPane().setLayout( new BorderLayout() );
		frame.getContentPane().add( display, BorderLayout.CENTER );
		frame.getContentPane().add( hudList, BorderLayout.WEST );
		
		frame.setSize( 600, 400 );
		frame.setVisible( true );
	}
	
	/**
	 * In this example program, simply adds some hard-coded HUDs;
	 * a typical editor would load classes from some form of config
	 * file or etc to dynamically include additional features.
	 * 
	 * @param manager Model (in MVC terminology) that holds info on the HUDs
	 */
	protected static void addHUDs( HUDManager manager )
	{
		manager.addHUD( new ShapeRenderer());
		manager.addHUD(new ShapeShifter2());
		manager.addHUD(new ShapeCreator());
	}
}