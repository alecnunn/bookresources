/*! 
@mainpage Stochastic Iteration method for Non-diffuse Global Illumination
@htmlonly
<h3>Graphics Programming Methods<br>
Stochastic Iteration for Non-diffuse Global Illumination<br>
<a href='mailto:szirmay@iit.bme.hu'>L. Szirmay-Kalos</a>,
<a href='mailto:benedek@iit.bme.hu'>B. Benedek</a><br>
2003.<br>
</h3>

<p>This on-line documentation was extracted from the code related to the method 
<b>Stochastic Iteration method for Non-diffuse Global Illumination</b> published in this book,
and is intented to help you browsing through the source. Check the links at the top of this page
for detailed information on Class Hierarchy, file or compound members, etc.</p>
@endhtmlonly
*/
 
#include <stdio.h>

#include "scene.hpp"
#include "StIter.hpp"

int main(int argc, char* argv[])
{
	//process command line parameters
	if (argc==1) { 
		printf("Usage: %s filename.sce [number_of_iterations [number_of_samples_per_light [power%% importance%%]]]\n", argv[0]); }
	else {
		//! Total number of iterations
		long nIteration = 3000000;
		if (argc>2) nIteration=atol(argv[2]);

		//! samples per lightsources
		long samples = 2;
		if (argc>3) samples=atol(argv[3]);

		float power=0.8f;
		float importance=0.1f;
		if (argc>5) {
			power=(float)(atol(argv[4])/100.0f);
			importance=(float)(atol(argv[5])/100.0f);
		}


	//Load the scene
		Scene scene(argv[1], samples);

	//Initialize the rendering algorithm
		StIter stiter(&scene, nIteration, power, importance);

	//Render the scene using Stochastic Iteration
		stiter.render();

	//Save the result
		stiter.saveImage();
	}
	return 0;
}