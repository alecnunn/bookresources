//
// Clipper class for OpenPTC 1.0 C++ Implementation
// Copyright (c) 1999 Glenn Fiedler (ptc@gaffer.org)
// This source code is licensed under the GNU Lesser GPL
// See http://www.gnu.org/copyleft/lesser.html for details
//

// include files
#include <math.h>
#include "Core/Area.h"
#include "Core/Clipper.h"

// using directive
#ifdef __COMPILER_SUPPORTS_NAMESPACES__
using namespace ptc;
#endif




Area PTCAPI Clipper::clip(const Area &area,const Area &clip)
{
    // get in coordinates
    int left   = area.left();
    int top    = area.top();
    int right  = area.right();
    int bottom = area.bottom();

    // get clip coordinates
    const int clip_left   = clip.left();
    const int clip_top    = clip.top();
    const int clip_right  = clip.right();
    const int clip_bottom = clip.bottom();

    // clip left
    if (left<clip_left)  left = clip_left;
    if (left>clip_right) left = clip_right;
    
    // clip top
    if (top<clip_top)    top = clip_top;
    if (top>clip_bottom) top = clip_bottom;

    // clip right
    if (right<clip_left)  right = clip_left;
    if (right>clip_right) right = clip_right;
    
    // clip bottom
    if (bottom<clip_top)    bottom = clip_top;
    if (bottom>clip_bottom) bottom = clip_bottom;

    // return clipped area
    return Area(left,top,right,bottom);
}




void PTCAPI Clipper::clip(const Area &source,const Area &clip_source,Area &clipped_source,
                              const Area &destination,const Area &clip_destination,Area &clipped_destination)
{
    // expand source area to floating point
    float source_left   = (float) source.left();
    float source_top    = (float) source.top();
    float source_right  = (float) source.right();
    float source_bottom = (float) source.bottom();

    // setup clipped source area
    float clipped_source_left   = source_left;
    float clipped_source_top    = source_top;
    float clipped_source_right  = source_right;
    float clipped_source_bottom = source_bottom;

    // perform clipping on floating point source area
    clip(clipped_source_left,clipped_source_top,clipped_source_right,clipped_source_bottom,clip_source);

    // check for early source area clipping exit
    if (clipped_source_left==clipped_source_right || clipped_source_top==clipped_source_bottom)
    {
        // clipped area is zero
        clipped_source = Area(0,0,0,0);
        clipped_destination = Area(0,0,0,0);
        return;
    }

    // calculate deltas in source clip
    const float source_delta_left   = clipped_source_left   - source_left;
    const float source_delta_top    = clipped_source_top    - source_top;
    const float source_delta_right  = clipped_source_right  - source_right;
    const float source_delta_bottom = clipped_source_bottom - source_bottom;

    // calculate ratio of source area to destination area
    const float source_to_destination_x = (float) destination.width()  / (float) source.width();
    const float source_to_destination_y = (float) destination.height() / (float) source.height();

    // expand destination area to floating point
    float destination_left   = (float) destination.left();
    float destination_top    = (float) destination.top();
    float destination_right  = (float) destination.right();
    float destination_bottom = (float) destination.bottom();

    // calculate adjusted destination area
    float adjusted_destination_left   = destination_left   + source_delta_left   * source_to_destination_x;
    float adjusted_destination_top    = destination_top    + source_delta_top    * source_to_destination_y;
    float adjusted_destination_right  = destination_right  + source_delta_right  * source_to_destination_x;
    float adjusted_destination_bottom = destination_bottom + source_delta_bottom * source_to_destination_y;

    // setup clipped destination area
    float clipped_destination_left   = adjusted_destination_left;
    float clipped_destination_top    = adjusted_destination_top;
    float clipped_destination_right  = adjusted_destination_right;
    float clipped_destination_bottom = adjusted_destination_bottom;

    // perform clipping on floating point destination area
    clip(clipped_destination_left,clipped_destination_top,clipped_destination_right,clipped_destination_bottom,clip_destination);

    // check for early destination area clipping exit
    if (clipped_destination_left==clipped_destination_right || clipped_destination_top==clipped_destination_bottom)
    {
        // clipped area is zero
        clipped_source = Area(0,0,0,0);
        clipped_destination = Area(0,0,0,0);
        return;
    }

    // calculate deltas in destination clip
    const float destination_delta_left   = clipped_destination_left   - adjusted_destination_left;
    const float destination_delta_top    = clipped_destination_top    - adjusted_destination_top;
    const float destination_delta_right  = clipped_destination_right  - adjusted_destination_right;
    const float destination_delta_bottom = clipped_destination_bottom - adjusted_destination_bottom;

    // calculate ratio of destination area to source area
    const float destination_to_source_x = 1 / source_to_destination_x;
    const float destination_to_source_y = 1 / source_to_destination_y;

    // calculate adjusted source area
    const float adjusted_source_left   = clipped_source_left   + destination_delta_left   * destination_to_source_x;
    const float adjusted_source_top    = clipped_source_top    + destination_delta_top    * destination_to_source_y;
    const float adjusted_source_right  = clipped_source_right  + destination_delta_right  * destination_to_source_x;
    const float adjusted_source_bottom = clipped_source_bottom + destination_delta_bottom * destination_to_source_y;

    // assign adjusted source to clipped source
    clipped_source_left   = adjusted_source_left;
    clipped_source_top    = adjusted_source_top;
    clipped_source_right  = adjusted_source_right;
    clipped_source_bottom = adjusted_source_bottom;

    // round clipped areas to integer coordinates
    round(clipped_source_left,clipped_source_top,clipped_source_right,clipped_source_bottom);
    round(clipped_destination_left,clipped_destination_top,clipped_destination_right,clipped_destination_bottom);

    // construct clipped area rectangles from rounded floating point areas
    clipped_source = Area((int)clipped_source_left,(int)clipped_source_top,(int)clipped_source_right,(int)clipped_source_bottom);
    clipped_destination = Area((int)clipped_destination_left,(int)clipped_destination_top,(int)clipped_destination_right,(int)clipped_destination_bottom);
}




void Clipper::clip(float &left,float &top,float &right,float &bottom,const Area &clip)
{
    // get floating point clip area
    const float clip_left   = (float) clip.left();
    const float clip_top    = (float) clip.top();
    const float clip_right  = (float) clip.right();
    const float clip_bottom = (float) clip.bottom();

    // clip floating point area against floating point clip area
    Clipper::clip(left,top,right,bottom,clip_left,clip_top,clip_right,clip_bottom);
}




void Clipper::clip(float &left,float &top,float &right,float &bottom,
                float clip_left,float clip_top,float clip_right,float clip_bottom)
{
    // clip left
    if (left<clip_left)  left = clip_left;
    if (left>clip_right) left = clip_right;
    
    // clip top
    if (top<clip_top)    top = clip_top;
    if (top>clip_bottom) top = clip_bottom;

    // clip right
    if (right<clip_left)  right = clip_left;
    if (right>clip_right) right = clip_right;
    
    // clip bottom
    if (bottom<clip_top)    bottom = clip_top;
    if (bottom>clip_bottom) bottom = clip_bottom;
}




void Clipper::round(float &left,float &top,float &right,float &bottom)
{
    // calculate floating point floors of area
    const float left_floor   = (float) floor(left);
    const float top_floor    = (float) floor(top);
    const float right_floor  = (float) floor(right);
    const float bottom_floor = (float) floor(bottom);

    // calculate floating point ceils of area
    const float left_ceil   = (float) ceil(left);
    const float top_ceil    = (float) ceil(top);
    const float right_ceil  = (float) ceil(right);
    const float bottom_ceil = (float) ceil(bottom);

    // get fractional components of area
    const float left_fraction   = left   - left_floor;
    const float top_fraction    = top    - top_floor;
    const float right_fraction  = right  - right_floor;
    const float bottom_fraction = bottom - bottom_floor;

    // round left
    if (left_fraction<=0.5f) left = left_floor;
    else left = left_ceil;

    // round top
    if (top_fraction<=0.5f) top = top_floor;
    else top = top_ceil;

    // round right
    if (right_fraction<0.5f) right = right_floor;
    else right = right_ceil;

    // round bottom
    if (bottom_fraction<0.5f) bottom = bottom_floor;
    else bottom = bottom_ceil;
}
