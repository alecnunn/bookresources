import java.awt.*;
import java.io.*;
/**
 * Wraps Java’s color
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
		myBaseColor=new Color(r,g,b);
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
	}
   /**
    * make a string representation
    */
   public String toString(){
      return new String(" "+r+" "+g+" "+b+" ");
   }
	/**
    * Makes a clone of this color.
    */
   public fColor makeClone(){
   	return new fColor(r,g,b);
   }
}

