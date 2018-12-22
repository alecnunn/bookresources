/* Copyright (C) Andre' LaMothe, 2000. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Andre' LaMothe, 2000"
 */
// HOPFIELD NET SIMULATOR /////////////////////////////////////////////////////////

// this simulator created based on Hebb Net software, very similar except that
// inputs act as outputs and weight matrix is always square

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

#define MAX_NEURODES	16	// maximum number of inputs/outputs

#define ACTF_STEP		0	// use a binary step activation function fs(x)
#define ACTF_LINEAR		1	// use a linear activation function fl(s)
#define ACTF_EXP		2	// use an inverse exponential activation function fe(x)

// MACROS ////////////////////////////////////////////////////////////////////////

// used to retrieve the i,jth element of a linear, row major, matrix

#define MAT(mat,width,i,j) (mat[((width)*i)+(j)])

// GLOBALS ///////////////////////////////////////////////////////////////////////

float	input_xi[MAX_NEURODES],		// holds that input values
		input_i[MAX_NEURODES],		// holds a single input vector
		output_i[MAX_NEURODES],		// holds a single output vector 
		input_act[MAX_NEURODES],	// holds the summed input activations 
		output_yi[MAX_NEURODES],	// holds the output values
		alpha = (float)1.0,			// needed for exponential activation function
		*weight_matrix = NULL;		// dynamically allocated weight matrix
		
int		num_neurodes,				// number of inputs in heb net
		activation_func = ACTF_STEP;// type of activation function to use

// FUNCTIONS /////////////////////////////////////////////////////////////////////

void Train_Net(void)
{
// this function is resposible for training the neural net using hebbian learning

// ask the user for another input/ouptput vector pair and then add the vectors contribution to
// the weight matrix and bias

printf("\nHopfield Training System.");
printf("\nTo train neural net you will enter each input vector to be recalled.");
printf("\nAll input vectors must be in bipolar form (1,-1,...1).");
printf("\nInput vectors an element at a time.");

printf("\n\nInput vectors have %d components",num_neurodes);

while(1)
	{
	// get the input vector
	printf("\nEnter input vector elements\n");

	for (int index=0; index<num_neurodes; index++)
		{
		printf("Input Vector Element[%d]=?",index);
		scanf("%f",&input_i[index]);
		} // end for

	// train the net with new vector, note we process one neuron at a time

	for (int index_j=0; index_j<num_neurodes; index_j++)
		{
		for (int index_i=0; index_i<num_neurodes; index_i++)
			{
			// use hebb learning alg. w=w+input(transpose)*input
			
			MAT(weight_matrix,num_neurodes,index_i, index_j) += (input_i[index_i]*input_i[index_j]);
			
			// test if i=j

			if (index_i==index_j)
				MAT(weight_matrix,num_neurodes,index_i, index_j) =(float)0.0;

			} // end for index_i
	
		} // end for index_j

	printf("\nDo you wish to enter another input vector Y or N?");
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

printf("\nHopfield Autoassociative Memory Simulation System.");
printf("\nYou will enter in test input vectors in binary form.");
printf("\nAll inputs must have %d elements.\n",num_neurodes);

while(1)
	{
	// get the input vector
	printf("\nEnter input vector elements\n");

	for (int index=0; index<num_neurodes; index++)
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
	for (index_j=0; index_j<num_neurodes; index_j++)
		{
		// now compute a dot product with the input vector and the column

		input_act[index_j] = (float)0.0; // reset activation

		for (int index_i=0; index_i<num_neurodes; index_i++)
			{
			input_act[index_j] = input_act[index_j] + 
				(MAT(weight_matrix,num_neurodes,index_i, index_j) * input_i[index_i]);
			} // end for index_i
		
		// now compute output based on activation function
		// note step should be used in most cases
		if (activation_func==ACTF_STEP)
			{
			// perform step activation
			if (input_act[index_j]>=(float)0.0)
				output_yi[index_j] = (float)1.0;
			else
				output_yi[index_j] = (float)0.0;

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
	for (index_j=0; index_j<num_neurodes; index_j++)
		printf("%2.2f, ",input_act[index_j]);
	printf("]\n");
	
	printf("\nFinal Outputs after activation functions are:\n[");
	for (index_j=0; index_j<num_neurodes; index_j++)
		printf("%2.2f, ",output_yi[index_j]);
	printf("]\n");

	// test if input was recalled corretly
	int bit_error=0;
	for (int index_i = 0; index_i<num_neurodes; index_i++)
		if (fabs(input_i[index_i]-output_yi[index_i])>.01)
			{
			bit_error++;
			} // end if error

	if (bit_error)
		printf("\nThere were %d bit error(s) in recall, try re-inputing the output.", bit_error);
	else
		printf("\nPerfect Recall!");

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

printf("\nThe Hopfield Net has %d neurodes/inputs/outputs",num_neurodes);
printf("\nThe weight matrix is %dX%d",num_neurodes, num_neurodes);
printf("\nThe W[i,j]th element refers to the weight from the ith to jth neurode\n");

for (int index_i = 0; index_i<num_neurodes;index_i++)
	{
	printf("\n|");
	for (int index_j=0; index_j<num_neurodes; index_j++)
		{
		// data is in row major form
		printf(" %2.2f ",MAT(weight_matrix,num_neurodes,index_i,index_j));

		} // end for index_j

	printf("|");
	
} // end for index_row

printf("\n");

} // end Print_Net

//////////////////////////////////////////////////////////////////////////////////

void Reset_Net(void)
{
// clear out all the matrices
memset(weight_matrix,0,num_neurodes*num_neurodes*sizeof(float));

} // end Reset_Net

// MAIN //////////////////////////////////////////////////////////////////////////

void main(void)
{
float FORCE_FP_LINK=(float)1.0; // needed for bug in VC++ fp lib link

printf("\nHopfield Neural Network Simulator.\n");

// querry user for parmaters of network

printf("\nEnter number of inputs (which is the same as outputs)?");
scanf("%d",&num_neurodes);

printf("\nSelect Activation Function (Hopfield usually uses Step)\n0=Step, 1=Linear, 2=Exponential?");
scanf("%d",&activation_func);

// test for exponential, get alpha is needed
if (activation_func == ACTF_EXP)
	{
	printf("\nEnter value for alpha (decimals allowed)?");
	scanf("%f",&alpha);
	} // end if

// allocate weight matrix it is mxn where m is the number of inputs and n is the
// number of outputs
weight_matrix = new float[num_neurodes*num_neurodes];

// clear out matrices
Reset_Net();

// enter main event loop

int	sel=0,
	done=0;

while(!done)
	{
	printf("\nHopfield Autoassociative Memory Main Menu\n");
	printf("\n1. Input Training Vectors into Neural Net.");
	printf("\n2. Run Neural Net.");
	printf("\n3. Print Out Weight Matrix.");
	printf("\n4. Reset Weight Matrix.");
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
	
		case 3: // Print Out Weight Matrix
			{
			Print_Net();
			} break;
	
		case 4: // Reset Weight Matrix
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
