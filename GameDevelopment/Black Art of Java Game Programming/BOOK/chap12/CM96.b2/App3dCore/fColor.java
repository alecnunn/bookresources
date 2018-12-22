import java.awt.*;
import java.io.*;
/**
 * Holds RGB and can return different colors for
 * different intenisties.
 */
public class fColor extends Object {
	public int r,g,b;
	protected Color myBaseColor;

   /**
    * construct a color with the RGB supplied.
    */
	public fColor(int r0,int g0,int b0){
		r=r0; g=g0; b=b0;
		myBaseColor=new Color(r,g,b);
	}
	/**
	 * constructs a color from a stream
	 */
   public fColor(InputStream is) throws IOException{
		fromString(is);
	}

   /**
    * returns a "Color" for the intensity
    */
	public void setIntensity(double i){
   	int ii=(int)(i*64);
		int rr=r+ii, gg=g+ii, bb=b+ii;
		if(rr>255) rr=255; else if(rr<0) rr=0;
		if(gg>255) gg=255; else if(gg<0) gg=0;
		if(bb>255) bb=255; else if(bb<0) bb=0;
		myBaseColor=new Color(rr,gg,bb);
	}

   /**
    * returns the base color
    */
	public Color getColor(){
	   return myBaseColor;
	}
   /**
    * read the color from a stream
    */   
	public void fromString(InputStream is) throws IOException{
		//-- make a stream tokenizer
		StreamTokenizer stream = new StreamTokenizer (is);
		stream.commentChar('#');

		//-- read the RGB triple
		stream.nextToken(); r=(int)stream.nval;
		stream.nextToken(); g=(int)stream.nval;
	   stream.nextToken(); b=(int)stream.nval;
	   myBaseColor=new Color(r,g,b);
	}
   /**
    * make a string representation
    */
   public String toString(){
      return new String(" "+r+" "+g+" "+b+" ");
   }
   public fColor makeClone(){
      return new fColor(r,g,b);
   }
}
