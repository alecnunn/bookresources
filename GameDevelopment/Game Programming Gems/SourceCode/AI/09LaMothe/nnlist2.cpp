/* Copyright (C) Andre' LaMothe, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andre' LaMothe, 2000"
 */
// HEBBIAN NET SIMULATOR /////////////////////////////////////////////////////////

// INCLUDES //////////////////////////////////////////////////////////////////////

#ifdef _WIN32
#include <conio.h>
#include <io.h>
#else
#include <ctype.h>
#endif
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <math.h>
#include <fcntl.h>

// DEFINES ///////////////////////////////////////////////////////////////////////

#define MAX_INPUTS		16	// maximum number of inputs
#define MAX_OUTPUTS		16	// maximum number of outputs

#define ACTF_STEP		0	// use a binary step activation function fs(x)
#define ACTF_LINEAR		1	// use a linear activation function fl(s)
#define ACTF_EXP		2	// use an inverse exponential activation function fe(x)

// MACROS ////////////////////////////////////////////////////////////////////////

// used to retrieve the i,jth element of a linear, row major, matrix

#define MAT(mat,width,i,j) (mat[((width)*i)+(j)])

// GLOBALS ///////////////////////////////////////////////////////////////////////

float	input_xi[MAX_INPUTS],	// holds that input values
		input_i[MAX_INPUTS],	// holds a single input vector
		output_i[MAX_OUTPUTS],	// holds a single output vector 
		input_act[MAX_OUTPUTS],	// holds the summed input activations 
		output_yi[MAX_OUTPUTS],	// holds the output values
		bias_bi[MAX_OUTPUTS],	// holds the bias weights bi
		alpha = (float)1.0,		// needed for exponential activation function
		*weight_matrix = NULL;	// dynamically allocated weight matrix
		
int		num_inputs,				// number of inputs in heb net
		num_outputs,			// number of outputs in heb net
		activation_func = ACTF_STEP;	// type of activation function to use

// FUNCTIONS /////////////////////////////////////////////////////////////////////

void Train_Net(void)
{
// this function is resposible for training the neural net using hebbian learning

// ask the user for another input/ouptput vector pair and then add the vectors contribution to
// the weight matrix and bias

printf("\nHebbian Training System.");
printf("\nTo train neural net you will enter each input/output vector pair");
printf("\nan element at a time.");

printf("\n\nInput vectors have %d components each and outputs have %d\n",num_inputs, num_outputs);

while(1)
	{
	// get the input vector
	printf("\nEnter input vector elements\n");

	int index;
	for (index=0; index<num_inputs; index++)
		{
		printf("Input Vector Element[%d]=?",index);
		scanf("%f",&input_i[index]);
		} // end for

	printf("\nNow enter associated output vector elements\n");

	// now get the output vector (note there might only be one neuron in this net

	for (index=0; index<num_outputs; index++)
		{
		printf("Output Vector Element[%d]=?",index);
		scanf("%f",&output_i[index]);
		} // end for

	// train the net with new vector, note we process one neuron at a time

	for (int index_j=0; index_j<num_outputs; index_j++)
		{
		for (int index_i=0; index_i<num_inputs; index_i++)
			{
			// hebb learning alg. wi=wi+input*ouput, b=b+output
			
			MAT(weight_matrix,num_outputs,index_i, index_j) += (input_i[index_i]*output_i[index_j]);
			bias_bi[index_j] += output_i[index_i];

			} // end for index_i
		} // end for index_j

	printf("\nDo you wish to enter another input/output pair Y or N?");
	char ans[8];
	scanf("%s",ans);
	if (toupper(ans[0])!='Y')
		break;

	} // end while

} // end Train_Net

//////////////////////////////////////////////////////////////////////////////////

void Run_Net(void)
{
// this function is responsible for running the net, it allows the user to enter test
// vectors and then computes the response of the network

printf("\nNetwork Simulation System.");
printf("\nYou will enter in test input vectors and the input will be processed by the net.");
printf("\nAll inputs must have %d elements\n",num_inputs);

while(1)
	{
	// get the input vector
	printf("\nEnter input vector elements\n");

	for (int index=0; index<num_inputs; index++)
		{
		printf("Input Vector Element[%d]=?",index);
		scanf("%f",&input_i[index]);
		} // end for

	// now process the input by performing a matrix mutiply
	// each weight vector is stored as a column in the weight matrix, so to process
	// the input for each neurode, we simply must perform a dot product, and then input
	// the result to the activation function, this is the basis of the parallel 
	// processing a neural net performs, all outputs are independent of the others

	// loop thru the columns (outputs, neurodes)
	int index_j;
	for (index_j=0; index_j<num_outputs; index_j++)
		{
		// now compute a dot product with the input vector and the column

		input_act[index_j] = (float)0.0; // reset activation

		for (int index_i=0; index_i<num_inputs; index_i++)
			{
			input_act[index_j] = input_act[index_j] + 
				(MAT(weight_matrix,num_outputs,index_i, index_j) * input_i[index_i]);
			} // end for index_i
		
		// add in bias term
		input_act[index_j] = input_act[index_j] + bias_bi[index_j];

		// now compute output based on activation function

		if (activation_func==ACTF_STEP)
			{
			// perform step activation
			if (input_act[index_j]>=(float)0.0)
				output_yi[index_j] = (float)1.0;
			else
				output_yi[index_j] = (float)-1.0;

			} // end if
		else
		if (activation_func==ACTF_LINEAR)
			{
			// perform linear activation
			output_yi[index_j] = input_act[index_j];
			}
		else
			{
			// must be exponential activation
			output_yi[index_j] =(float)(1/(1+exp(-input_act[index_j]*alpha)));
	
			} // end else exp

		} // end for index_j

	// now that ouputs have been computed print everything out

	printf("\nNet inputs were:\n[");
	for (index_j=0; index_j<num_outputs; index_j++)
		printf("%2.2f, ",input_act[index_j]);
	printf("]\n");
	
	printf("\nFinal Outputs after activation functions are:\n[");
	for (index_j=0; index_j<num_outputs; index_j++)
		printf("%2.2f, ",output_yi[index_j]);
	printf("]\n");

	printf("\nDo you wish to enter another test input Y or N?");
	char ans[8];
	scanf("%s",ans);
	if (toupper(ans[0])!='Y')
		break;
 
	} // end while

} // end Run_Net

//////////////////////////////////////////////////////////////////////////////////

void Print_Net(void)
{
// this function prints out the current weight matrix and biases along with the specifics
// about the net

printf("\nThe Hebb Net has %d inputs and %d outputs",num_inputs, num_outputs);
printf("\nThe weight matrix is %dX%d",num_inputs, num_outputs);
printf("\nThe W[i,j]th element refers to the weight from the ith to jth neurode\n");

for (int index_i = 0; index_i<num_inputs;index_i++)
	{
	printf("\n|");
	for (int index_j=0; index_j<num_outputs; index_j++)
		{
		// data is in row major form
		printf(" %2.2f ",MAT(weight_matrix,num_outputs,index_i,index_j));

		} // end for index_j

	printf("|");
	
} // end for index_row

printf("\n\nBias weights for the net are:\n[");

for (int index_j=0; index_j<num_outputs; index_j++)
	printf("%2.2f, ",bias_bi[index_j]);

printf("]\n\n");

} // end Print_Net

//////////////////////////////////////////////////////////////////////////////////

void Reset_Net(void)
{
// clear out all the matrices
memset(weight_matrix,0,num_inputs*num_outputs*sizeof(float));
memset(bias_bi,0,MAX_OUTPUTS*sizeof(float));

} // end Reset_Net

// MAIN //////////////////////////////////////////////////////////////////////////

void main(void)
{
float FORCE_FP_LINK=(float)1.0; // needed for bug in VC++ fp lib link

printf("\nHebbian Neural Network Simulator.\n");

// querry user for parmaters of network

printf("\nEnter number of inputs?");
scanf("%d",&num_inputs);

printf("\nEnter number of Neurons (outputs)?");
scanf("%d",&num_outputs);

printf("\nSelect Activation Function (Hebbian usually uses Step)\n0=Step, 1=Linear, 2=Exponential?");
scanf("%d",&activation_func);

// test for exponential, get alpha is needed
if (activation_func == ACTF_EXP)
	{
	printf("\nEnter value for alpha (decimals allowed)?");
	scanf("%f",&alpha);
	} // end if

// allocate weight matrix it is mxn where m is the number of inputs and n is the
// number of outputs
weight_matrix = new float[num_inputs*num_outputs];

// clear out matrices
Reset_Net();

// enter main event loop

int	sel=0,
	done=0;

while(!done)
	{
	printf("\nHebb Net Main Menu\n");
	printf("\n1. Input Training Vectors into Neural Net.");
	printf("\n2. Run Neural Net.");
	printf("\n3. Print Out Weight Matrix and Biases.");
	printf("\n4. Reset Weight Matrix and Biases.");
	printf("\n5. Exit Simulator.");
	printf("\n\nSelect One Please?");
	scanf("%d",&sel);

	// what was the selection
	switch(sel)
		{
	
		case 1: // Input Training Vectors into Neural Net
			{
			Train_Net();
				
			} break;
	
		case 2: // Run Neural Net
			{
			Run_Net();
			} break;
	
		case 3: // Print Out Weight Matrix and Biases
			{
			Print_Net();
			} break;
	
		case 4: // Reset Weight Matrix and Biases
			{
			Reset_Net();
			} break;
	
		case 5: // Exit Simulator
			{
			// set exit flag
			done=1;

			} break;
	
		default:break;

		} // end swtich

	} // end while

// free up resources

delete [] weight_matrix;

} // end main
