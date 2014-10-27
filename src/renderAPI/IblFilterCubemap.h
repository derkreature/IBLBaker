//--------------------------------------------------------------------------------------
//
// Based on code from AMDCubeMapGen
// https://code.google.com/p/cubemapgen/
// under the New BSD License.
// Copyright(c) 2005, ATI Research, Inc.
// All rights reserved.
// Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met :
// 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and / or other materials provided with the distribution.
// 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING,
// BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
// IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY,
// OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//--------------------------------------------------------------------------------------
// (C) 2005 ATI Research, Inc., All rights reserved.
//--------------------------------------------------------------------------------------

#ifndef INCLUDED_BB_FILTER_CUBEMAP
#define INCLUDED_BB_FILTER_CUBEMAP

#include <IblTextureImage.h>

namespace Ibl
{
enum CubemapFixupType
{
    CP_FIXUP_NONE = -1,
    CP_FIXUP_AVERAGE_LINEAR = 0,
    CP_FIXUP_AVERAGE_HERMITE,
    CP_FIXUP_PULL_HERMITE,
    CP_FIXUP_PULL_LINEAR,
};

//used to index cube faces
#define CP_FACE_X_POS 0
#define CP_FACE_X_NEG 1
#define CP_FACE_Y_POS 2
#define CP_FACE_Y_NEG 3
#define CP_FACE_Z_POS 4
#define CP_FACE_Z_NEG 5

#define CP_EDGE_LEFT   0
#define CP_EDGE_RIGHT  1
#define CP_EDGE_TOP    2
#define CP_EDGE_BOTTOM 3

#define CP_CORNER_NNN  0
#define CP_CORNER_NNP  1
#define CP_CORNER_NPN  2
#define CP_CORNER_NPP  3
#define CP_CORNER_PNN  4
#define CP_CORNER_PNP  5
#define CP_CORNER_PPN  6
#define CP_CORNER_PPP  7

struct CPCubeMapNeighbor
{
    uint8_t m_Face;    //index of neighboring face
    uint8_t m_Edge;    //edge in neighboring face that abuts this face
};


//---------------------------------------------------
// Source: AMDCubemapGen.
// Fixup cube edges
// average texels on cube map faces across the edges.
//---------------------------------------------------
template <typename T>
void
fixupCubeEdges (Ibl::TextureImagePtr cubemap, 
                int32_t mipId, 
                CubemapFixupType fixupType, 
                float fixupWidth)
{

CPCubeMapNeighbor sg_CubeNgh[6][4] =
{
    //XPOS face
    {{CP_FACE_Z_POS, CP_EDGE_RIGHT },
     {CP_FACE_Z_NEG, CP_EDGE_LEFT  },
     {CP_FACE_Y_POS, CP_EDGE_RIGHT },
     {CP_FACE_Y_NEG, CP_EDGE_RIGHT }},
    //XNEG face
    {{CP_FACE_Z_NEG, CP_EDGE_RIGHT },
     {CP_FACE_Z_POS, CP_EDGE_LEFT  },
     {CP_FACE_Y_POS, CP_EDGE_LEFT  },
     {CP_FACE_Y_NEG, CP_EDGE_LEFT  }},
    //YPOS face
    {{CP_FACE_X_NEG, CP_EDGE_TOP },
     {CP_FACE_X_POS, CP_EDGE_TOP },
     {CP_FACE_Z_NEG, CP_EDGE_TOP },
     {CP_FACE_Z_POS, CP_EDGE_TOP }},
    //YNEG face
    {{CP_FACE_X_NEG, CP_EDGE_BOTTOM},
     {CP_FACE_X_POS, CP_EDGE_BOTTOM},
     {CP_FACE_Z_POS, CP_EDGE_BOTTOM},
     {CP_FACE_Z_NEG, CP_EDGE_BOTTOM}},
    //ZPOS face
    {{CP_FACE_X_NEG, CP_EDGE_RIGHT  },
     {CP_FACE_X_POS, CP_EDGE_LEFT   },
     {CP_FACE_Y_POS, CP_EDGE_BOTTOM },
     {CP_FACE_Y_NEG, CP_EDGE_TOP    }},
    //ZNEG face
    {{CP_FACE_X_POS, CP_EDGE_RIGHT  },
     {CP_FACE_X_NEG, CP_EDGE_LEFT   },
     {CP_FACE_Y_POS, CP_EDGE_TOP    },
     {CP_FACE_Y_NEG, CP_EDGE_BOTTOM }}
};

//The 12 edges of the cubemap, (entries are used to index into the neighbor table)
// this table is used to average over the edges.
int32_t sg_CubeEdgeList[12][2] = {
   {CP_FACE_X_POS, CP_EDGE_LEFT},
   {CP_FACE_X_POS, CP_EDGE_RIGHT},
   {CP_FACE_X_POS, CP_EDGE_TOP},
   {CP_FACE_X_POS, CP_EDGE_BOTTOM},

   {CP_FACE_X_NEG, CP_EDGE_LEFT},
   {CP_FACE_X_NEG, CP_EDGE_RIGHT},
   {CP_FACE_X_NEG, CP_EDGE_TOP},
   {CP_FACE_X_NEG, CP_EDGE_BOTTOM},

   {CP_FACE_Z_POS, CP_EDGE_TOP},
   {CP_FACE_Z_POS, CP_EDGE_BOTTOM},
   {CP_FACE_Z_NEG, CP_EDGE_TOP},
   {CP_FACE_Z_NEG, CP_EDGE_BOTTOM}
};

int32_t sg_CubeCornerList[6][4] = {
   { CP_CORNER_PPP, CP_CORNER_PPN, CP_CORNER_PNP, CP_CORNER_PNN }, // XPOS face
   { CP_CORNER_NPN, CP_CORNER_NPP, CP_CORNER_NNN, CP_CORNER_NNP }, // XNEG face
   { CP_CORNER_NPN, CP_CORNER_PPN, CP_CORNER_NPP, CP_CORNER_PPP }, // YPOS face
   { CP_CORNER_NNP, CP_CORNER_PNP, CP_CORNER_NNN, CP_CORNER_PNN }, // YNEG face
   { CP_CORNER_NPP, CP_CORNER_PPP, CP_CORNER_NNP, CP_CORNER_PNP }, // ZPOS face
   { CP_CORNER_PPN, CP_CORNER_NPN, CP_CORNER_PNN, CP_CORNER_NNN }  // ZNEG face
};


   Ibl::PixelBox face0 = cubemap->getPixelBox(0, mipId);
   int32_t nChannels = (int32_t)(face0.getNumChannels());
   uint32_t size = (uint32_t)(face0.size().x);

   T* edgeStartPtr = nullptr;
   T* neighborEdgeStartPtr = nullptr;

   //number of texels inward towards cubeface center to apply fixup to
   int32_t fixupDist = 0;

   // note that if functionality to filter across the three texels for each corner, then 
   T * cornerPtr[8][3];      //indexed by corner and face idx
   T * faceCornerPtrs[4];    //corner pointers for face
   int32_t cornerNumPtrs[8];         //indexed by corner and face idx

   //if there is no fixup, or fixup width = 0, do nothing
   if((fixupType == CP_FIXUP_NONE) ||
      (fixupWidth == 0)  )
   {
      return;
   }

   //special case 1x1 cubemap, average face colors
   if (size == 1)
   {
      //iterate over channels
       for (int32_t k = 0; k<nChannels; k++)
      {
         double  accum = 0.0f;
         //iterate over faces to accumulate face colors
         for (int32_t faceId = 0; faceId < 6; faceId++)
         {
            Ibl::PixelBox face = cubemap->getPixelBox(faceId, mipId);
            T* ptr = (T*)(face.data);
            accum += ptr[k];
         }

         //compute average over 6 face colors
         accum /= 6.0;

         //iterate over faces to distribute face colors
         for (uint32_t faceId = 0; faceId < 6; faceId++)
         {
            Ibl::PixelBox face = cubemap->getPixelBox(faceId, mipId);
            T* ptr = (T*)(face.data);
            ptr[k] = (T)(accum);
         }
      }

      return;
   }

   //iterate over corners
   for (uint32_t iCorner = 0; iCorner < 8; iCorner++)
   {
      cornerNumPtrs[iCorner] = 0;
   }

   // Iterate over faces to collect list of corner texel pointers
   for (uint32_t faceId = 0; faceId < 6; faceId++)
   {
      Ibl::PixelBox face = cubemap->getPixelBox(faceId, mipId);

      // The 4 corner pointers for this face
      T* ptr = (T*)(face.data);

      faceCornerPtrs[0] = &ptr[0];
      faceCornerPtrs[1] = &ptr[( (size - 1) * nChannels) ];
      faceCornerPtrs[2] = &ptr[( (size) * (size - 1) * nChannels) ];
      faceCornerPtrs[3] = &ptr[( (((size) * (size - 1)) + (size - 1)) * nChannels )];

      // Iterate over face corners to collect cube corner pointers
      for (int32_t i = 0; i < 4; i++)
      {
         int32_t corner = sg_CubeCornerList[faceId][i];
         cornerPtr[corner][ cornerNumPtrs[corner] ] = faceCornerPtrs[i];
         cornerNumPtrs[corner]++;
      }
   }

   // iterate over corners to average across corner tap values
   for (int32_t cornerId = 0; cornerId < 8; cornerId++)
   {
      for (int32_t k = 0; k < nChannels; k++)
      {             
         double cornerTapAccum = 0.0;

         // iterate over corner texels and average results
         for(int32_t i = 0; i < 3; i++ )
         {
            cornerTapAccum += *(cornerPtr[cornerId][i] + k);
         }

         // divide by 3 to compute average of corner tap values
         cornerTapAccum *= (1.0 / 3.0);

         // iterate over corner texels and average results
         for(int32_t i = 0; i < 3; i++)
         {
            *(cornerPtr[cornerId][i] + k) = (T)(cornerTapAccum);
         }
      }
   }   


   //maximum width of fixup region is one half of the cube face size
   fixupDist = (int32_t)Ibl::minValue( (float)(fixupWidth), (float)(size / 2.0f));

   //iterate over the twelve edges of the cube to average across edges
   for(int32_t i = 0; i < 12; i++)
   {
      int32_t face = sg_CubeEdgeList[i][0];
      int32_t edge = sg_CubeEdgeList[i][1];

      const CPCubeMapNeighbor& neighborInfo = sg_CubeNgh[face][edge];
      int32_t neighborFace = neighborInfo.m_Face;
      int32_t neighborEdge = neighborInfo.m_Edge;

      Ibl::PixelBox faceBox = cubemap->getPixelBox(face, mipId);
      Ibl::PixelBox neighborBox = cubemap->getPixelBox(neighborFace, mipId);

      edgeStartPtr = (T*)faceBox.data;
      neighborEdgeStartPtr = (T*)neighborBox.data;

      int32_t edgeWalk = 0;
      int32_t neighborEdgeWalk = 0;

      //amount to pointer to sample taps away from cube face
      int32_t edgePerpWalk = 0;
      int32_t neighborEdgePerpWalk = 0;

      //Determine walking pointers based on edge type
      // e.g. CP_EDGE_LEFT, CP_EDGE_RIGHT, CP_EDGE_TOP, CP_EDGE_BOTTOM
      switch(edge)
      {
         case CP_EDGE_LEFT:
            // no change to faceEdgeStartPtr  
            edgeWalk = nChannels * size;
            edgePerpWalk = nChannels;
         break;
         case CP_EDGE_RIGHT:
            edgeStartPtr += (size - 1) * nChannels;
            edgeWalk = nChannels * size;
            edgePerpWalk = -nChannels;
         break;
         case CP_EDGE_TOP:
            // no change to faceEdgeStartPtr  
            edgeWalk = nChannels;
            edgePerpWalk = nChannels * size;
         break;
         case CP_EDGE_BOTTOM:
            edgeStartPtr += (size) * (size - 1) * nChannels;
            edgeWalk = nChannels;
            edgePerpWalk = -(int32_t)(nChannels * size);
         break;
      }

      //For certain types of edge abutments, the neighbor edge walk needs to 
      //  be flipped: the cases are 
      // if a left   edge mates with a left or bottom  edge on the neighbor
      // if a top    edge mates with a top or right edge on the neighbor
      // if a right  edge mates with a right or top edge on the neighbor
      // if a bottom edge mates with a bottom or left  edge on the neighbor
      //Seeing as the edges are enumerated as follows 
      // left   =0 
      // right  =1 
      // top    =2 
      // bottom =3            
      // 
      //If the edge enums are the same, or the sum of the enums == 3, 
      //  the neighbor edge walk needs to be flipped
      if( (edge == neighborEdge) || ((edge + neighborEdge) == 3) )
      {   //swapped direction neighbor edge walk
         switch(neighborEdge)
         {
            case CP_EDGE_LEFT:  //start at lower left and walk up
               neighborEdgeStartPtr += (size - 1) * (size) *  nChannels;
               neighborEdgeWalk = -(int32_t)(nChannels * size);
               neighborEdgePerpWalk = nChannels;
            break;
            case CP_EDGE_RIGHT: //start at lower right and walk up
               neighborEdgeStartPtr += ((size - 1)*(size) + (size - 1)) * nChannels;
               neighborEdgeWalk = -(int32_t)(nChannels * size);
               neighborEdgePerpWalk = -nChannels;
            break;
            case CP_EDGE_TOP:   //start at upper right and walk left
               neighborEdgeStartPtr += (size - 1) * nChannels;
               neighborEdgeWalk = -nChannels;
               neighborEdgePerpWalk = (nChannels * size);
            break;
            case CP_EDGE_BOTTOM: //start at lower right and walk left
               neighborEdgeStartPtr += ((size - 1)*(size) + (size - 1)) * nChannels;
               neighborEdgeWalk = -nChannels;
               neighborEdgePerpWalk = -(int32_t)(nChannels * size);
            break;
         }            
      }
      else
      { //swapped direction neighbor edge walk
         switch (neighborEdge)
         {
            case CP_EDGE_LEFT: //start at upper left and walk down
               //no change to neighborEdgeStartPtr for this case since it points 
               // to the upper left corner already
               neighborEdgeWalk = nChannels * size;
               neighborEdgePerpWalk = nChannels;
            break;
            case CP_EDGE_RIGHT: //start at upper right and walk down
               neighborEdgeStartPtr += (size - 1) * nChannels;
               neighborEdgeWalk = nChannels * size;
               neighborEdgePerpWalk = -nChannels;
            break;
            case CP_EDGE_TOP:   //start at upper left and walk left
               //no change to neighborEdgeStartPtr for this case since it points 
               // to the upper left corner already
               neighborEdgeWalk = nChannels;
               neighborEdgePerpWalk = (nChannels * size);
            break;
            case CP_EDGE_BOTTOM: //start at lower left and walk left
               neighborEdgeStartPtr += (size) * (size - 1) * nChannels;
               neighborEdgeWalk = nChannels;
               neighborEdgePerpWalk = -(int32_t)(nChannels * size);
            break;
         }
      }


      //Perform edge walk, to average across the 12 edges and smoothly propagate change to 
      //nearby neighborhood

      //step ahead one texel on edge
      edgeStartPtr += edgeWalk;
      neighborEdgeStartPtr += neighborEdgeWalk;

      // note that this loop does not process the corner texels, since they have already been
      //  averaged across faces across earlier
      for(int32_t j=1; j< (int32_t)(size - 1); j++)       
      {             
         //for each set of taps along edge, average them
         // and rewrite the results into the edges
         for(int32_t k = 0; k<nChannels; k++)
         {             
            double edgeTap, neighborEdgeTap, avgTap;  //edge tap, neighborEdgeTap and the average of the two
            double edgeTapDev, neighborEdgeTapDev;

            edgeTap = *(edgeStartPtr + k);
            neighborEdgeTap = *(neighborEdgeStartPtr + k);

            //compute average of tap intensity values
            avgTap = 0.5f * (edgeTap + neighborEdgeTap);

            //propagate average of taps to edge taps
            (*(edgeStartPtr + k)) = (T)(avgTap);
            (*(neighborEdgeStartPtr + k)) = (T)(avgTap);

            edgeTapDev = edgeTap - avgTap;
            neighborEdgeTapDev = neighborEdgeTap - avgTap;

            //iterate over taps in direction perpendicular to edge, and 
            //  adjust intensity values gradualy to obscure change in intensity values of 
            //  edge averaging.
            for(int32_t iFixup = 1; iFixup < fixupDist; iFixup++)
            {
               //fractional amount to apply change in tap intensity along edge to taps 
               //  in a perpendicular direction to edge 
               double fixupFrac = (double)(fixupDist - iFixup) / (double)(fixupDist); 
               double fixupWeight;

               switch (fixupType)
               {
                  case CP_FIXUP_PULL_LINEAR:
                  {
                     fixupWeight = fixupFrac;
                  }
                  break;

                  case CP_FIXUP_PULL_HERMITE:
                  {
                     //hermite spline interpolation between 1 and 0 with both pts derivatives = 0 
                     // e.g. smooth step
                     // the full formula for hermite interpolation is:
                     //              
                     //                  [  2  -2   1   1 ][ p0 ] 
                     // [t^3  t^2  t  1 ][ -3   3  -2  -1 ][ p1 ]
                     //                  [  0   0   1   0 ][ d0 ]
                     //                  [  1   0   0   0 ][ d1 ]
                     // 
                     // Where p0 and p1 are the point locations and d0, and d1 are their respective derivatives
                     // t is the parameteric coordinate used to specify an interpoltion point on the spline
                     // and ranges from 0 to 1.
                     //  if p0 = 0 and p1 = 1, and d0 and d1 = 0, the interpolation reduces to
                     //
                     //  p(t) =  - 2t^3 + 3t^2
                     fixupWeight = ((-2.0 * fixupFrac + 3.0) * fixupFrac * fixupFrac);
                  }
                  break;
                  case CP_FIXUP_AVERAGE_LINEAR:
                  {
                     fixupWeight = fixupFrac;

                     //perform weighted average of edge tap value and current tap
                     // fade off weight linearly as a function of distance from edge
                     edgeTapDev = 
                        (*(edgeStartPtr + (iFixup * edgePerpWalk) + k)) - avgTap;
                     neighborEdgeTapDev = 
                        (*(neighborEdgeStartPtr + (iFixup * neighborEdgePerpWalk) + k)) - avgTap;
                  }
                  break;

                  case CP_FIXUP_AVERAGE_HERMITE:
                  {
                     fixupWeight = ((-2.0 * fixupFrac + 3.0) * fixupFrac * fixupFrac);

                     //perform weighted average of edge tap value and current tap
                     // fade off weight using hermite spline with distance from edge
                     //  as parametric coordinate
                     edgeTapDev = 
                        (*(edgeStartPtr + (iFixup * edgePerpWalk) + k)) - avgTap;
                     neighborEdgeTapDev = 
                        (*(neighborEdgeStartPtr + (iFixup * neighborEdgePerpWalk) + k)) - avgTap;
                  }
                  break;
               }

               // vary intensity of taps within fixup region toward edge values to hide changes made to edge taps
               *(edgeStartPtr + (iFixup * edgePerpWalk) + k) -= (T)(fixupWeight * edgeTapDev);
               *(neighborEdgeStartPtr + (iFixup * neighborEdgePerpWalk) + k) -= (T)(fixupWeight * neighborEdgeTapDev);
            }
         }

         edgeStartPtr += edgeWalk;
         neighborEdgeStartPtr += neighborEdgeWalk;
      }        
   }
}
}

#endif