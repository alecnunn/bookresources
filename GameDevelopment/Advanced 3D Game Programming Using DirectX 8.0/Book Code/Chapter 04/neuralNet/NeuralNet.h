/*******************************************************************
 *         Advanced 3D Game Programming using DirectX 8.0
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
 *   Title: NeuralNet.h
 *    Desc: code to run a neural network
 *          
 * copyright (c) 2001 by Peter A Walsh and Adrian Perez
 * See license.txt for modification and distribution information
 ******************************************************************/

#ifndef _NEURALNET_H
#define _NEURALNET_H

#include <string>
#include <vector>
#include <map>

using namespace std;

#include "file.h"

class cNeuralNet  
{
protected:
	class cNode;
	class cEdge;

public:
	string GetOutput();

	void SendInput( const char* inputName, float amt );
	
	void Load( cFile& file );
	void Run();
	void Clear();
	cNeuralNet();
	virtual ~cNeuralNet();

	void ListInputs();
	void GetInputs();

protected:


	cNode* FindNode( const char* name );

	class cNode
	{
	public:
		void Init( const char* name, float weight );

		void Clear();
		virtual void Run();

		void AddOutEdge( cNode* target, float edgeWeight );
		void SendInput( float in );

		const char* GetName() const;
		float GetTotal() const;
	protected:

		// Computes the output function given the total.
		virtual float CalcOutput();
		
		string	m_name;
		float	m_weight; // initial bias in either direction
		float	m_total;  // total of the summed inputs up to this point

		vector< cEdge >	m_outEdges;

	};

	class cEdge
	{
		cNode*	m_pSrc;
		cNode*	m_pDest;
		float	m_weight;
	public:

		cEdge( cNode* pSrc, cNode* pDst, float weight );

		void Fire( float amount );
	};
	friend class cNode;

	vector< vector< cNode* > > m_nodes;

	// maps the names of output nodes to output strings.
	map< string, string > m_inputs;
	map< string, string > m_outputs;

	string m_defaultOutput;
};


inline const char* cNeuralNet::cNode::GetName() const
{
	return m_name.c_str();
}

inline float cNeuralNet::cNode::GetTotal() const
{
	return m_total;
}


#endif // _NEURALNET_H
