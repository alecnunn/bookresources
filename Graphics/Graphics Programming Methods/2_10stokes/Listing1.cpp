//k-D Tree Face Portals with Solid Geometry
//Conor Stokes, (Company, University?)
//cstokes@tpgi.com.au

//ON CD as "Listing 2"
//Code Listing 1.
// makes bounding boxes for the nodes
void CKdTreeNode::MakeBoxes(const CVec3& minbound, 
                            const CVec3& maxbound) {

  // set node box
  m_nodebox = CAAB(minbound, maxbound); 

  // if this isn't a leaf
  if (m_childptr) {

    // copy the min and max
    CVec3 leftmax(maxbound);
    CVec3 rightmin(minbound);
	
    // set the min and max in the axis of the partition
    // to the partition
    leftmax[m_axis] = m_partition;
    rightmin[m_axis] = m_partition;
	
    // make the nodes for the children
    m_childptr[0].MakeBoxes(minbound, leftmax);
    m_childptr[1].MakeBoxes(rightmin, maxbound);

  }
}

