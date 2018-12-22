// System Mesh class

#pragma once

#include "ObjectAdapter.h"


namespace System
{
    /// A triangle mesh.
    /// Create a mesh instance using the Display::createMesh factory method.
    /// This class contains no methods because to render this mesh, you simply select it 
    /// with a display using Display::selectMesh, then call the Display::renderMesh method.

	class Mesh : public ObjectAdapter
	{
	public:

		// todo: some methods for querying mesh information would be handy here, eg. number of verts, faces, edges etc...
	};
}
