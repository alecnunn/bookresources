////////////////////////////////////////////////////////////
//
//  PARSE.CPP - Environment Data File Parser Class
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/07/14 - Added /Ow optimization pragma for
//                         ReadSurface function (compiler
//                         bug workaround).
//              95/07/21 - Version 1.02B release.
//              96/02/14 - Version 1.02C release.
//              96/02/27 - Modified ParseFile to calculate
//                         environment extents.
//              96/04/01 - Version 1.03A release.
//              96/11/29 - Changed sscanf parameters in
//                         ParsePatches and ParseElements
//                         from WORD to int.
//                       - Changed indices in ParseVertices
//                         and ParseSurfaces from WORD to
//                         int.
//                       - Changed sscanf parameters in
//                         ParseElements, ParsePatches and
//                         ReadVector from "%1s" to "%1c"
//                         to prevent buffer overflow.
//              97/04/01 - Version 1.03B release.
//
//  Compilers:  Microsoft Visual C/C++ Professional V5.0
//              Borland C++ Version 5.0
//
//  Author:     Ian Ashdown, P.Eng.
//              byHeart Consultants Limited
//              620 Ballantree Road
//              West Vancouver, B.C.
//              Canada V7S 1W3
//              Tel. (604) 922-6148
//              Fax. (604) 987-7621
//
//  Copyright 1994-2003 byHeart Consultants Limited
//
//  The following source code has been derived from:
//
//    Ashdown, I. 1994. Radiosity: A Programmer's
//    Perspective. New York, NY: John Wiley & Sons.
//
//    (Available on CD-ROM from www.helios32.com.)
//
//  It may be freely copied, redistributed, and/or modified
//  for personal use ONLY, as long as the copyright notice
//  is included with all source code files.
//
////////////////////////////////////////////////////////////

#include <string.h>
#include "error.h"
#include "parse.h"

// File path separator (MS-DOS specific)
static const char PathSeparator[] = "\\";

// Data file keywords
static const char EndFileStr[] = "END_FILE";
static const char CommentStr[] = "COMMENT";

// Parse world file
BOOL Parse::ParseFile( char *fname, char *fpath, Environ
    *pe )
{
  char *pefp;           // Entity file path pointer
  Instance *pinst;      // Instance pointer
  Instance *pinsthd;    // Instance list head pointer

  penv = pe;    // Save environment pointer

  // Delete previous environment (if any)
  penv->DeleteEnv();

  pinst = pinsthd = NULL;

  // Initialize environment statistics
  penv->num_inst = (WORD) 0;
  penv->num_surf = (WORD) 0;
  penv->num_patch = (WORD) 0;
  penv->num_elem = (WORD) 0;
  penv->num_vert = (WORD) 0;

  // Build file path string
  pefp = ent_buff;
  if (*fpath != '\0')
  {
    strcpy(ent_buff, fpath);
    if (fpath[strlen(ent_buff) - 1] != *PathSeparator)
      strcat(ent_buff, PathSeparator);
    pefp += strlen(ent_buff);
  }

  if (ifile.Open(fname) != TRUE)    // Open instance file
  {
    sprintf(msg_buff, "Could not open world file %s",
        fname);
    ReportError(msg_buff);
    return FALSE;
  }

  ReadLine(ifile);      // Read world name

  for ( ; ; )
  {
    ReadLine(ifile);    // Read entity file name

    // Check for end of file
    if (strcmp(line_buff, EndFileStr) == 0)
      break;

    // Build full entity file name
    strcpy(pefp, line_buff);

    // Read entity file
    if ((pinst = ParseEntityFile()) == NULL)
    {
      ifile.Close();
      return FALSE;
    }

    // Read 3-D transformation matrix
    ReadTransform();

    // Transform entity into instance
    TransformInstance(pinst);

    // Link instance to instance list
    pinst->SetNext(pinsthd);
    pinsthd = pinst;
    penv->num_inst++;
  }

  ifile.Close();
  penv->pinsthd = pinsthd;

  penv->CalcExtents();  // Calculate environment extents
  
  return TRUE;
}

// Parse entity data file
Instance *Parse::ParseEntityFile()
{
  BOOL status;              // Return status
  Instance *pinst;          // Instance pointer
  Surface3 *ps;             // Surface pointer
  Vertex3 *pv;              // Vertex pointer

  pinst = NULL;
  ps = NULL;
  pv = NULL;

  surf_cnt = patch_cnt = elem_cnt = vert_cnt = (WORD) 0;

  // Open entity file
  if (efile.Open(ent_buff) != TRUE)
  {
    sprintf(msg_buff, "Could not open entity file %s",
        ent_buff);
    ReportError(msg_buff);
    return NULL;
  }

  ReadLine(efile);      // Read file description

  pv = ParseVertices();
  ps = ParseSurfaces();
  status = ParsePatches();

  if (status == TRUE)
    status = ParseElements();

  // Delete temporary pointer arrays
  delete [] pv_array;
  delete [] ps_array;
  delete [] pp_array;

  // Create new entity
  if (status == TRUE)
    pinst = new Instance(pv, ps);

  efile.Close();
  return pinst;
}

// Parse vertices
Vertex3 *Parse::ParseVertices()
{
  int v_index;              // Vertex pointer array index
  Vertex3 *pv;              // Vertex pointer
  Vertex3 *pvhd;            // Vertex list head ptr

  pv = pvhd = NULL;

  ReadLine(efile);      // Read vertex section header

  // Build vertex linked list
  for ( ; ; )
  {
    // Read vertex vector
    if ((pv = ReadVertex()) == NULL)
      break;

    // Link vertex to vertex list
    pv->SetNext(pvhd);
    pvhd = pv;
    penv->num_vert++;
    vert_cnt++;
  }

  // Build vertex pointer array
  pv = pvhd;
  pv_array = new VertexPtr[vert_cnt];
  v_index = vert_cnt - (WORD) 1;
  while (pv != NULL)
  {
    pv_array[v_index--] = pv;
    pv = pv->GetNext();
  }
  return pvhd;
}

// Parse surfaces
Surface3 *Parse::ParseSurfaces()
{
  int s_index;              // Surface pointer array index
  Surface3 *ps;             // Surface pointer
  Surface3 *pshd;           // Surface list head ptr

  ps = pshd = NULL;

  ReadLine(efile);      // Read surface section header

  // Build surface linked list
  for ( ; ; )
  {
    // Read surface identifier
    if ((ps = ReadSurface()) == NULL)
      break;

    // Link surface to surface list
    ps->SetNext(pshd);
    pshd = ps;
    penv->num_surf++;
    surf_cnt++;
  }

  // Build surface pointer array
  ps = pshd;
  ps_array = new SurfacePtr[surf_cnt];
  s_index = surf_cnt - (WORD) 1;
  while (ps != NULL)
  {
    ps_array[s_index--] = ps;
    ps = ps->GetNext();
  }
  return pshd;
}

// NOTE: The Microsoft Visual C++ Version 1.5 compiler
//       generates no code for the statement:
//
//         emit.SetBlueBand(eblue);
//
//       in the following function when the /O2 option
//       (Generate Fast Code) is specified unless aliasing
//       across function calls is assumed.

#pragma optimize( "w", on )

// Read surface identifier
Surface3 *Parse::ReadSurface()
{
  char start[2], end[2];        // Vector separators
  float ered, egreen, eblue;    // Exitance components
  float rred, rgreen, rblue;    // Reflectance components
  Spectra reflect;              // Spectral reflectance
  Spectra emit;                 // Spectral radiant exitance

  ReadLine(efile);      // Read color vector

  if (sscanf(line_buff, "%1s %f %f %f %1s %1s %f %f %f %1s",
      start, &rred, &rgreen, &rblue, end, start, &ered,
      &egreen, &eblue, end) == 10)
  {
    // Set reflectance
    reflect.SetRedBand(rred);
    reflect.SetGreenBand(rgreen);
    reflect.SetBlueBand(rblue);

    // Set initial spectral radiant exitance
    emit.SetRedBand(ered);
    emit.SetGreenBand(egreen);
    emit.SetBlueBand(eblue);

    return new Surface3(reflect, emit);
  }
  else
    return NULL;        // Must be terminator
}

#pragma optimize( "w", off )

// Parse patch identifiers
BOOL Parse::ParsePatches()
{
  char start, end;          // List separators
  int v_index;              // Vertex pointer array index
  BOOL status = TRUE;       // Status flag
  int p_index;              // Patch pointer array index
  int s_index;              // Surface index
  int ivtx[4];              // Vertex indices
  Patch3 *pp;               // Patch pointer
  Patch3 *pphd;             // Patch head pointer
  PatchList *ppl = NULL;    // Patch list pointer
  PatchList *pplnext;       // Next patch list pointer
  Vertex3 *pv[4];           // Vertex pointers array

  ReadLine(efile);      // Read patch section header

  for ( ; ; )
  {
    ReadLine(efile);    // Read patch identifier

    if (sscanf(line_buff, "%d %1c %d %d %d %d %1c",
        &s_index, &start, &ivtx[0], &ivtx[1], &ivtx[2],
        &ivtx[3], &end) == 7)
    {
      // Validate surface index
      if (s_index >= surf_cnt)
      {
        sprintf(msg_buff,
            "Entity file: %s\nPatch # %u\nSurface index "
            "error: %s", ent_buff, patch_cnt + 1,
            line_buff);
        ReportError(msg_buff);
        status = FALSE;
        break;
      }

      // Validate vertex array indices
      for (v_index = 0; v_index < 4; v_index++)
      {
        if (ivtx[v_index] >= vert_cnt)
        {
          sprintf(msg_buff,
              "Entity file: %s\nPatch # %u\nVertex index "
              "error: %s", ent_buff, patch_cnt + 1,
              line_buff);
          ReportError(msg_buff);
          status = FALSE;
          break;
        }
      }

      if (status == FALSE)
        break;

      // Get vertex pointers
      for (v_index = 0; v_index < 4; v_index++)
        pv[v_index] = pv_array[ivtx[v_index]];

      // Update surface patch linked list
      pp = new Patch3(pv, ps_array[s_index]);
      pphd = ps_array[s_index]->GetPatchPtr();
      pp->SetNext(pphd);
      ps_array[s_index]->SetPatchPtr(pp);

      // Determine whether triangle or quadrilateral
      if (ivtx[2] != ivtx[3])
        pp->SetQuad();
    }
    else
      break;

    // Link patch to temporary patch list
    ppl = new PatchList(pp, ppl);
    penv->num_patch++;
    patch_cnt++;
  }

  // Build patch pointer array and delete patch list
  pp_array = new PatchPtr[patch_cnt];
  p_index = patch_cnt - (WORD) 1;
  while (ppl != NULL)
  {
    pp_array[p_index--] = ppl->GetPatchPtr();
    pplnext = ppl->GetNext();
    delete ppl;
    ppl = pplnext;
  }

  return status;
}

// Parse element identifiers
BOOL Parse::ParseElements()
{
  char start, end;          // List separators
  int nvert;                // Number of vertices
  int v_index;              // Vertex pointer array index
  BOOL status = TRUE;       // Status flag
  int p_index;              // Patch array index
  int ivtx[4];              // Vertex indices
  Element3 *pe;             // Element pointer
  Element3 *pehd;           // Element head pointer
  ElemList *pel;            // Element list pointer
  ElemList *pelhd;          // Element list head pointer
  Vertex3 *pv[4];           // Vertex pointers array

  ReadLine(efile);      // Read element section header

  for ( ; ; )
  {
    ReadLine(efile);    // Read element identifier

    if (sscanf(line_buff, "%d %1c %d %d %d %d %1c",
        &p_index, &start, &ivtx[0], &ivtx[1], &ivtx[2],
        &ivtx[3], &end) == 7)
    {
      // Validate patch index
      if (p_index >= patch_cnt)
      {
        sprintf(msg_buff,
            "Entity file: %s\nElement # %u\nPatch index "
            "error: %s", ent_buff, elem_cnt + 1,
            line_buff);
        ReportError(msg_buff);
        status = FALSE;
        break;
      }

      // Validate vertex array indices
      for (v_index = 0; v_index < 4; v_index++)
      {
        if (ivtx[v_index] >= vert_cnt)
        {
          sprintf(msg_buff,
              "Entity file: %s\nElement # %u\nVertex index "
              "error: %s", ent_buff, elem_cnt + 1,
              line_buff);
          ReportError(msg_buff);
          status = FALSE;
          break;
        }
      }

      if (status == FALSE)
        break;

      // Get vertex pointers
      for (v_index = 0; v_index < 4; v_index++)
        pv[v_index] = pv_array[ivtx[v_index]];

      // Update patch element linked list
      pe = new Element3(pv, pp_array[p_index]);
      pehd = pp_array[p_index]->GetElementPtr();
      pe->SetNext(pehd);
      pp_array[p_index]->SetElementPtr(pe);
      penv->num_elem++;
      elem_cnt++;

      // Determine whether triangle or quadrilateral
      if (ivtx[2] != ivtx[3])
      {
        nvert = 4;
        pe->SetQuad();
      }
      else
        nvert = 3;

      for (v_index = 0; v_index < nvert; v_index++)
      {
        // Update vertex element linked list
        pelhd = pv[v_index]->GetElemListPtr();
        pel = new ElemList(pe, pelhd);
        pv[v_index]->SetElemListPtr(pel);
      }
    }
    else
      break;
  }

  return status;
}

void Parse::ReadTransform()
{
  double sx, sy, sz;    // Scaling parameters
  double rx, ry, rz;    // Rotation parameters
  double tx, ty, tz;    // Translation parameters

  // Read transformation vectors
  ReadVector(ifile, &sx, &sy, &sz);
  ReadVector(ifile, &rx, &ry, &rz);
  ReadVector(ifile, &tx, &ty, &tz);
  
  // Convert rotation angles to radians
  rx = DegToRad(rx);
  ry = DegToRad(ry);
  rz = DegToRad(rz);

  // Calculate vertex transformation matrix
  tm.SetScale(sx, sy, sz);
  tm.SetRotation(rx, ry, rz);
  tm.SetTranslation(tx, ty, tz);
  tm.BuildTransform();
}

// Read vertex identifier
Vertex3 *Parse::ReadVertex()
{
  double xval, yval, zval;      // Vertex coordinates

  if ((ReadVector( efile, &xval, &yval, &zval)) == TRUE)
    return new Vertex3(Point3(xval, yval, zval));
  else
    return NULL;
}

// Read vector
BOOL Parse::ReadVector( WinText &file, double *px, double
    *py, double *pz )
{
  float x, y, z;        // Temporary variables
  char start, end;      // Data separators

  ReadLine(file);       // Read vector

  if (sscanf(line_buff, "%1c %f %f %f %1c", &start, &x, &y,
      &z, &end) == 5)
  {
    *px = x;
    *py = y;
    *pz = z;

    return TRUE;
  }
  else
    return FALSE;
}

// Transform entity into instance
void Parse::TransformInstance( Instance *pinst )
{
  Element3 *pe;         // Element pointer
  Patch3 *pp;           // Patch pointer
  Surface3 *ps;         // Surface pointer
  Vertex3 *pv;          // Vertex pointer

  // Transform vertex co-ordinates
  pv = pinst->GetVertPtr();
  while (pv != NULL)
  {
    tm.Transform(pv->GetPosnPtr());
    pv = pv->GetNext();
  }

  // Calculate patch attributes
  ps = pinst->GetSurfPtr();
  while (ps != NULL)
  {
    pp = ps->GetPatchPtr();
    while (pp != NULL)
    {
      // Calculate element attributes
      pe = pp->GetElementPtr();
      while (pe != NULL)
      {
        pe->CalcArea();
        pe->CalcNormal();
        pe = pe->GetNext();
      }

      pp->CalcArea();
      pp->CalcCenter();
      pp->CalcNormal();
      pp = pp->GetNext();
    }
    ps = ps->GetNext();
  }

  // Calculate vertex normals
  pv = pinst->GetVertPtr();
  while (pv != NULL)
  {
    pv->CalcNormal();
    pv = pv->GetNext();
  }
}

// Read next line from file
void Parse::ReadLine( WinText &file )
{
  for ( ; ; )
  {
    file.GetLine(line_buff, MaxLine);

    // Skip comment lines
    if (strncmp(line_buff, CommentStr, strlen(CommentStr))
        != 0)
      break;
  }
}

