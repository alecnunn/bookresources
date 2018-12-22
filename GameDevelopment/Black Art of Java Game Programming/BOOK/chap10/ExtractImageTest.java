
// Demo of how to extract images from an image strip

import java.applet.*;
import java.awt.*;
import java.awt.image.*;

public class ExtractImageTest extends Applet {
  Image strip;        // the image strip
  Image images[];     // the constituent images
  int num_images = 0; // number of images in strip
  int width = 10;     // width of each image in strip
  int height;

  public void init() {
    Image strip = getImage(getCodeBase(),"strip.gif");

    // wait for image to load
    while (strip.getWidth(this) <0);

    // define number of images in strip
    num_images = strip.getWidth(this)/width;

    // define height of each image
    height = strip.getHeight(this);

    // define array of constituent images
    images = new Image[num_images];

    // extract constituent images
    extractImages(strip,images,num_images,width,height);
  }

/////////////////////////////////////////////////////////////////
  // Extract the constituent images from a strip.
  // There are num_images to extract, each with the
  // specified width and height.

  public void extractImages(Image strip,
			    Image images[],
			    int num_images,
			    int width,
			    int height) {
    ImageProducer source = strip.getSource();
    
    for (int i = 0; i<num_images; i++) {
      // define filter to pull image at (i*width,0) with 
      //   dimensions (width,height)
      ImageFilter extractFilter = new CropImageFilter(i*width,
						      0,
						      width,
						      height);

      // define producer from source and filter
      ImageProducer producer =  
	new FilteredImageSource(source,extractFilter);

      // extract the subimage!
      images[i] = createImage(producer);
    }
  }

  // display constituent images in a diagonal
  public void paint(Graphics g) {
    for (int i=0; i<num_images; i++) {
      g.drawImage(images[i],i*width,i*height,this);
 
    }
  }
}
