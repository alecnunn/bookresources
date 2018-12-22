/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: NeuralNet.cpp
 *    Desc: Code to load and run a neural net.
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/


using namespace std;

#include "stdafx.h"
#include <math.h> // for atof, exp
#include <stdio.h>
#include <stdlib.h> // for atof
#include "NeuralNet.h"

int main(int argc, char* argv[])
{
	// Sorry, I don't do cout.
	printf( "Advanced 3D Game Programming using DirectX 8.0\n" );
	printf( "----------------------------------------------\n\n" );
	printf( "Neural Net Simulator\n\n");

	if( argc != 2 )
	{
		printf("Usage: neuralnet filename.nn\n");
		return 0;
	}

	printf("Using nn description file [%s]\n\n", argv[1] );

	cNeuralNet nn;
	cFile nnFile;
	nnFile.Open( argv[1] );
	nn.Load( nnFile );
	nnFile.Close();

	int done = 0;
	while( !done )
	{
		// Clear the totals
		nn.Clear();

		// List the inputs for the net from the user
		nn.ListInputs();

		// Get the inputs for the net from the user
		nn.GetInputs();

		// Run the net
		nn.Run();

		// Get the net's output.
		string output = nn.GetOutput();

		printf("\nNeural Net output was [%s]\n", output.c_str() );
		printf("\nRun Again? (y/n)\n");
		char buff[80];
		gets( buff );

		if( !(buff[0] == 'y' || buff[0] == 'Y') )
		{
			done = 1;
		}
	}
	return 0;
}


cNeuralNet::cNeuralNet()
{
	// no work needs to be done.
}

cNeuralNet::~cNeuralNet()
{
	// delete all of the nodes. (each node will get it's outgoing edges)
	int numLayers = m_nodes.size();
	for( int i=0; i<numLayers; i++ )
	{
		int layerSize = m_nodes[i].size();
		for( int j=0; j<layerSize; j++ )
		{
			delete m_nodes[i][j];
		}
	}
}


cNeuralNet::cNode* cNeuralNet::FindNode( const char *name)
{
	cNode* pCurr;

	// Search for the node.
	int numLayers = m_nodes.size();
	for( int i=0; i<numLayers; i++ )
	{
		int layerSize = m_nodes[i].size();
		for( int j=0; j<layerSize; j++ )
		{
			pCurr = m_nodes[i][j];
			if( 0 == strcmp( pCurr->GetName(), name ) )
				return pCurr;
		}
	}

	// didn't contain the node (bad)
	printf( "ERROR IN NEURAL NET FILE!\n");
	printf( "Tried to look for node named [%s]\n", name );
	printf( "but couldn't find it!\n");
	exit(0);
	return NULL;
}

void cNeuralNet::Clear()
{
	// Call clear on each of the networks.
	cNode* pCurr;

	int numLayers = m_nodes.size();
	for( int i=0; i<numLayers; i++ )
	{
		int layerSize = m_nodes[i].size();
		for( int j=0; j<layerSize; j++ )
		{
			pCurr = m_nodes[i][j];
			pCurr->Clear();
		}
	}
}

void cNeuralNet::Run()
{
	// Run each layer, running each node in each layer.
	int numLayers = m_nodes.size();
	for( int i=0; i<numLayers; i++ )
	{
		int layerSize = m_nodes[i].size();
		for( int j=0; j<layerSize; j++ )
		{
			m_nodes[i][j]->Run();
		}
	}
}

void cNeuralNet::SendInput( const char *inputTarget, float amt)
{
	// Find the node that we're sending the input to, and send it.
	FindNode( inputTarget )->SendInput( amt );
}

void cNeuralNet::cNode::Clear()
{
	// initial total is set to the bias
	m_total = m_weight; 
}

void cNeuralNet::cNode::Run()
{
	// Compute the transfer function
	float output = CalcOutput();

	// Send it to each of our children
	int size = m_outEdges.size();
	for( int i=0; i< size; i++ )
	{
		m_outEdges[i].Fire( output );
	}
}


void cNeuralNet::cNode::Init( const char *name, float weight) 
{
	m_name = string( name );
	m_weight = weight;
}

float cNeuralNet::cNode::CalcOutput()
{
	// This can use an exponential-type function
	// but for simplicity's sake we're just doing
	// flat yes/no.
	if( m_total >= 1.0f )
		return 1.0f;
	else
		return 0.f;
}

void cNeuralNet::cNode::SendInput(float in)
{
	// just add the input to the total for the network.
	m_total += in;
}

void cNeuralNet::cNode::AddOutEdge(cNode *target, float edgeWeight)
{
	// Create an edge structure
	m_outEdges.push_back( cEdge( this, target, edgeWeight) );
}


void cNeuralNet::Load(cFile &file)
{
	char buff[1024];
	char* pCurr;
	char delim[] = " \t\n\r";
	while( 1 )
	{
		file.ReadNonCommentedLine( buff, '#' );
		pCurr = strtok( buff, delim );
		if( 0 == strcmp( pCurr, "NN_BEGIN" ) )
		{
			// Read in the # of layers
			int nLayers = atoi( strtok( NULL, delim ) );
			for( int i=0; i<nLayers; i++ )
			{
				// populate the vector-of-vectors with vectors.
				vector< cNeuralNet::cNode* > garbage;
				m_nodes.push_back( garbage );
			}
			
		}
		else if( 0 == strcmp( pCurr, "NN_END" ) )
		{
			// We're done loading the network.  break from the loop
			break;
		}
		else if( 0 == strcmp( pCurr, "NEURON" ) )
		{
			int layerNum = atoi( strtok( NULL, delim ) );
			cNode* pNew = new cNode();
			char* name;
			name = strtok( NULL, delim );
			pCurr = strtok( NULL, delim );
			float val;
			sscanf(pCurr,"%f", &val );
			pNew->Init( name, val );

			// done initializing.  add it to the list
			m_nodes[layerNum].push_back( pNew );
		}
		else if( 0 == strcmp( pCurr, "INPUT" ) )
		{
			string nodeName = string( strtok( NULL, delim ) );
			
			char* pRestOfString = strtok( NULL, "\n\r" );

			char* pStrStart = strchr( pRestOfString, '\"' );
			char* pStrEnd = strrchr( pRestOfString, '\"' );

			assert( pStrStart && pStrEnd && (pStrStart!=pStrEnd) );

			m_inputs[ nodeName ] = string( pStrStart+1, pStrEnd );
		}
		else if( 0 == strcmp( pCurr, "OUTPUT" ) )
		{
			string nodeName = string( strtok( NULL, delim ) );
			
			char* pRestOfString = strtok( NULL, "\n\r" );

			char* pStrStart = strchr( pRestOfString, '\"' );
			char* pStrEnd = strrchr( pRestOfString, '\"' );

			assert( pStrStart && pStrEnd && (pStrStart!=pStrEnd) );

			m_outputs[ nodeName ] = string( pStrStart+1, pStrEnd );
		}
		else if( 0 == strcmp( pCurr, "DEFAULTOUT" ) )
		{
			char* pRestOfString = strtok( NULL, "\n\r" );

			char* pStrStart = strchr( pRestOfString, '\"' );
			char* pStrEnd = strrchr( pRestOfString, '\"' );

			assert( pStrStart && pStrEnd && (pStrStart!=pStrEnd) );

			m_defaultOutput = string( pStrStart+1, pStrEnd );
		}
		else if( 0 == strcmp( pCurr, "EDGE" ) )
		{
			char* src;
			char* dst;
			src = strtok( NULL, " \t\n\r" );
			dst = strtok( NULL, " \t\n\r" );

			pCurr = strtok( NULL, " \t\n\r" );
			float val;
			sscanf(pCurr,"%f", &val );

			FindNode( src )->AddOutEdge( FindNode( dst ), val );
		}

	}

}

cNeuralNet::cEdge::cEdge( cNode *pSrc, cNode *pDest, float weight)
: m_pSrc( pSrc )
, m_pDest( pDest )
, m_weight( weight)
{
	// all done.
}

void cNeuralNet::cEdge::Fire( float amount )
{
	// Send the signal, multiplied by the weight, 
	// to the destination node.
	m_pDest->SendInput( amount * m_weight );
}

//==========--------------------------  

void cNeuralNet::ListInputs()
{
	printf("\n");
	printf("Neural Net Inputs:\n");
	printf("------------------\n");

	map<string, string>::iterator iter;
	for( iter = m_inputs.begin(); iter != m_inputs.end(); iter++ )
	{
		printf("%s\n", (*iter).second.c_str() );
	}
}


void cNeuralNet::GetInputs()
{
	printf("\n");
	printf("Enter Inputs:\n");
	printf("-------------\n");

	map<string, string>::iterator iter;
	for( iter = m_inputs.begin(); iter != m_inputs.end(); iter++ )
	{
		printf("Enter floating point input for [%s]\n", (*iter).second.c_str() );
		char buff[80];
		gets( buff );
		float value = atof( buff );

		cNode* pNode = FindNode( (*iter).first.c_str() );

		pNode->SendInput( value );
	}
}


string cNeuralNet::GetOutput()
{
	map<string, string>::iterator iter;

	float fBest = 1.f;
	map<string, string>::iterator best = m_outputs.end();

	for( iter = m_outputs.begin(); iter != m_outputs.end(); iter++ )
	{
		// check the output.  is it the best?
		cNode* pNode = FindNode( (*iter).first.c_str() );
		if( pNode->GetTotal() > fBest )
		{
			// new best.
			fBest = pNode->GetTotal();
			best = iter;
			printf("new total: %f\n", fBest );
		}
	}
	if( best == m_outputs.end() )
	{
		return m_defaultOutput;
	}
	else
	{
		return (*best).second.c_str();
	}
}

