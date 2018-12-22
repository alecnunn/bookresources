/* Copyright (C) Marco Tombesi, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Marco Tombesi, 2001"
 */
bool GetPhysiqueWeights(INode *pNode, INode *pRoot,
                        Modifier *pMod, BoneData_t *BD)
{
   // create a Physique Export Interface for given Physique Modifier
   IPhysiqueExport *phyInterface = (IPhysiqueExport*)
         pMod->GetInterface(I_PHYINTERFACE);
   if(phyInterface)
   {
      // create a ModContext Export Interface for the specific
      // node of the Physique Modifier
      IPhyContextExport *modContextInt = (IPhyContextExport*)
            phyInterface->GetContextInterface(pNode);

      // needed by vertex interface (only Rigid supported by now)
      modContextInt->ConvertToRigid(TRUE);

      // more than a single bone per vertex
      modContextInt->AllowBlending(TRUE);
      if(modContextInt)
      {
         int totalVtx = modContextInt->GetNumberVertices();
         for(int i = 0; i < totalVtx; i++)
         {
            IPhyVertexExport *vtxInterface = (IPhyVertexExport*)
                  modContextInt->GetVertexInterface(i);
            if(vtxInterface)
            {
               int vtxType = vtxInterface->GetVertexType();
               if(vtxType == RIGID_TYPE)
               {
                  INode *boneNode = ((IPhyRigidVertex*)vtxInterface)
                        -> GetNode();
                  int boneIdx = GetBoneIndex(pRoot, boneNode);
                     Insert
                  // Build vertex data
                  MRCweight_hdr wdata;
                  wdata.vertIdx = i;
                  wdata.weight = 1.0f;

                  //Insert into proper bonedata
                  BD[boneIdx].weightsVect.push_back(wdata);

                  // update vertexWeightCnt for that bone
                  BD[boneIdx].Hdr.vertexCnt
                        = BD[boneIdx].weightsVect.size();
               }
               else if(vtxType == RIGID_BLENDED_TYPE)
               {
                  IPhyBlendedRigidVertex *vtxBlendedInt =
                        (IPhyBlendedRigidVertex*)vtxInterface;
                  for(int j = 0; j < vtxBlendedInt->GetNumberNodes()
                        ;j++)
                  {
                     INode *boneNode = vtxBlendedInt->GetNode(j);
                     int boneIdx = GetBoneIndex(pRoot, boneNode);

                     // Build vertex data
                     MRCweight_hdr wdata;
                     wdata.vertIdx = i;
                     wdata.weight = vtxBlendedInt->GetWeight(j);

                     // check vertex existence for this bone
                     bool notfound = true;
                     for (int v=0; notfound
                           && v < BD[boneIdx].weightsVect.size(); v++)
                     {
                        // update found vert weight data for that bone
                        if ( BD[boneIdx].weightsVect[v].vertIdx
                                 == wdata.vertIdx )
                        {
                           BD[boneIdx].weightsVect[v].weight
                                 += wdata.weight;
                           notfound = false;
                        }
                     }

                     if (notfound)
                     {
                        // Add a new vertex weight data into proper
                        // bonedata
                        BD[boneIdx].weightsVect.push_back(wdata);

                        // update vertexWeightCnt for that bone
                        BD[boneIdx].Hdr.vertexCnt
                              = BD[boneIdx].weightsVect.size();
                     }
                  }
               }
            }
         }
         phyInterface->ReleaseContextInterface(modContextInt);
      }
      pMod->ReleaseInterface(I_PHYINTERFACE, phyInterface);
   }
   return true;
}
