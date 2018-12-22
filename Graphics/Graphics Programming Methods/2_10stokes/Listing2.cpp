//k-D Tree Face Portals with Solid Geometry
//Conor Stokes, (Company, University?)
//cstokes@tpgi.com.au

//ON CD as "Listing 2"
//Code Listing 2.
// makes the neighbor links for the leafs
void CKdTreeNode::MakeNeighbors(CommonAncestors& ancestors) {

  // is this not a leaf?
  if (m_childptr) {
		
    // create new copies of the common ancestors (3 left and 3 right
    // pointers)
    CommonAncestors leftside(ancestors);
    CommonAncestors rightside(ancestors);

    // stick this in the common ancestor nodes of the respective   
    // children on the partion axis
    leftside.left[m_axis] = this;
    rightside.right[m_axis] = this; 
		
    // recurse
    m_childptr[0].MakeNeighbors(leftside);
    m_childptr[1].MakeNeighbors(rightside);
		
    // loop through left, then right leafs that have been added with 
    // recursion
    for (int i = 0; i < m_leftleafs.size(); ++i) {
		
      for (int j = 0; j < m_rightleafs.size(); ++j) {
							
        // if the boxes of these leafs intersect (ignoring the 
        // partition axis)
        if (Intersects(m_leftleafs[i]->Box(),
                       m_rightleafs[j]->Box(), 
                       m_axis)) {

          // then add the face neighbor link
          m_leftleafs[i]->m_neighbors.push_back(m_rightleafs[j]);
          m_rightleafs[j]->m_neighbors.push_back(m_leftleafs[i]);

        }	
		
      }
		
    }

    // empty out the vectors
    m_leftleafs.clear();
    m_rightleafs.clear();

  // this is a leaf
  } else {
		
    // link this node back to its common ancestors on each axis/side
    for (int i = 0; i < 3; ++i) {

      // null pointer checks to see if this side of the leaf has a  
      // neighbor
      if (ancestors.left[i])
        ancestors.left[i]->m_leftleafs.push_back(this);

      if (ancestors.right[i])
        ancestors.right[i]->m_rightleafs.push_back(this);

    }
	
  }

}
