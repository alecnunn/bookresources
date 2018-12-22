import java.awt.*;
import java.io.*;

public class fWirePolygon extends fIndexingPolygon {

	public fWirePolygon(int indices[],int n){
		super(indices,n);
	}

	public fWirePolygon(InputStream is) throws IOException {
		super(is);
	}

	public void paint(Graphics g,int x[],int y[]){
		copyIndexedPoints(x,y);
		g.setColor(Color.black);
		g.drawPolygon(ourScratchPoly);
	}

   public void clipAndPaint(Graphics g,fProjectedPoints p,fGenericCamera cam){
      paint(g,p.x,p.y);
   }

	public fIndexingPolygon makeClone(){
      int i[];
      System.arraycopy(myIndices,0,i=new int[nbrIndices],0,nbrIndices);
      return new fWirePolygon(i,nbrIndices);
	}
}
