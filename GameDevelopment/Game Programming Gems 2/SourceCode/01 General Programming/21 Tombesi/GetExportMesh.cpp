/* Copyright (C) Marco Tombesi, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Tombesi, 2001"
 */
bool ExportMesh (INode* pNode, FILE *out)
{
   ...
   MRCmesh_hdr mHdr;
   Matrix3 tm = pNode->GetObjectTM(0);
   ObjectState os = pNode->EvalWorldState(0);
   int needDelete;
   Mesh& mesh = *(( (GeomObject*) os.obj)->GetRenderMesh (0, pNode, ...));
   ...

   // write the mesh vertices
   mHdr.vertCnt = mesh.getNumVerts();
   for(int i = 0; i < mHdr.vertCnt; i++)
   {
      Point3 pnt = mesh.getVert(i) * tm;    //premultiply in MAX
   ...
   }

   // write vertex normals
   mesh.buildNormals();
   mHdr.normCnt = mesh.getNumVerts();
   for(i = 0; i < mHdr.normCnt; i++)
   {
      Point3 norm = Normalize(mesh.getNormal(i));
   ...
   }

   // build and write faces
   mHdr.faceCnt = mesh.getNumFaces();
   for(i = 0; i < mHdr.faceCnt; i++)
   {
      MRCface_hdr fHdr;
      fHdr.vert[0] = mesh.faces[i].v[0];
      fHdr.vert[1] = mesh.faces[i].v[1];
      fHdr.vert[2] = mesh.faces[i].v[2];
      ...
   }
   ...
}
