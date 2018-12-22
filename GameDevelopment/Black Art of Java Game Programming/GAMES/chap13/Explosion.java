import java.awt.*;    

/////////////////////////////////////////////////////////////////
// 
// Explosion sprite class: contains lines that blow apart
//
/////////////////////////////////////////////////////////////////

public class Explosion extends ContainerSprite {
  // center of explosion
  protected int locx,locy;

  // size of lines of explosion
  static final int EXP_SIZE = 13;

  // how long explosion lasts
  protected int max_updates = 0;

  // updates so far                    
  protected int updates;                           

  // the constituent lines in the container
  static final int ex[][] = { {0,0} , {-EXP_SIZE,EXP_SIZE}, 
			      {0,0}, {-EXP_SIZE,EXP_SIZE}};
  static final int ey[][] = { {-EXP_SIZE,EXP_SIZE}, {0,0}, 
			      {-EXP_SIZE,EXP_SIZE}, {0,0} };

  // the constituent edge sprites in container
  MoveablePolygon edges[] = new MoveablePolygon[ex.length];

  // size of explosion
  protected float size;

  // is explosion over?
  protected boolean done = false;

/////////////////////////////////////////////////////////////////
// Explosion constructor: color, position, #updates
/////////////////////////////////////////////////////////////////

  public Explosion(Color color, int x, int y, int u) {

    // define edge sprites of explosion

    for (int i=0; i<edges.length; i++) {
      edges[i] = new MoveablePolygon(ex[i],ey[i],ex[i].length,
				     x,y,color);
      edges[i].restore();            // show all edges
      
      // add each edge to container
      addSprite(edges[i]);
    }
    
    // initialize how long the explosion lasts 
    max_updates = u;

    // initialize counter
    updates = 0;

    // set explosion at the given coordinates
    locx = x;
    locy = y;

    // restore explosion sprite
    restore();

    // set size
    size = 1.0f;

    // reset boolean
    done = false;
  }

/////////////////////////////////////////////////////////////////
// update explosion or suspend if it reaches maximum size
//   (assumes 4 edges in the container, which move apart
//    along x,y coordinates.)
/////////////////////////////////////////////////////////////////

  public void update() {
    if (updates++ < max_updates) {
      // blow the lines apart!
      size *= 1.7f;               // increase size

      int isize = (int)size;      // cast size to int
      edges[0].setPosition(locx + isize,locy );
      edges[1].setPosition(locx ,locy + isize);
      edges[2].setPosition(locx - isize,locy);
      edges[3].setPosition(locx,locy - isize);

    }
    else 
      done = true;

  }

/////////////////////////////////////////////////////////////////
// return explosion status
/////////////////////////////////////////////////////////////////

  public boolean isDone() {
    return done;
  }
}





