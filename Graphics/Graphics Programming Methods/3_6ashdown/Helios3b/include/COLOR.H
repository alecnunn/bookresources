////////////////////////////////////////////////////////////
//
//  COLOR.H - Color Model Classes Include File
//
//  Version:    1.03B
//
//  History:    94/08/23 - Version 1.00A release.
//              94/11/17 - Revised ColorRGB::SetColor
//                         comments.
//              94/11/26 - Added Spectra::Limit and
//                         Spectra::Mult functions.
//              94/12/16 - Version 1.01A release.
//              95/02/05 - Version 1.02A release.
//              95/06/13 - Added IntRGB class.
//                       - Added CO_INT_SHIFT, CO_INT_MULT
//                         and CO_MONO_SHIFT definitions.
//                       - Changed C_BlueWeight, C_RedWeight
//                         and C_GreenWeight constants from
//                         floating point to integer data
//                         type.
//                       - Modified SetMono and SetPseudo
//                         functions to modify RGB class
//                         members.
//              95/07/20 - Revised grayscale color weights.
//              95/07/21 - Version 1.02B release.
//              95/10/10 - Changed CO_INT_MULT definition
//                         from INT_MAX to SHRT_MAX.
//              96/02/14 - Version 1.02C release.
//              96/04/01 - Version 1.03A release.
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

#ifndef _COLOR_H
#define _COLOR_H

#include <limits.h>
#include "general.h"

// Grayscale color weights
//
// NOTE: these weights are equivalent to 0.2125 (red),
//       0.7154 (green), and 0.0721 (blue) in the floating
//       point range of [0.0, 1.0].
//
//       These weights are specified in ITU-R Recommendation
//       BT.709, "Basic Parameter Values for the Studio and
//       for International Programme Exchange (1990),
//       [formerly CCIR Rec. 709], ITU, 1211 Geneva 20,
//       Switzerland. They are representative of the color
//       phosphors used in contemporary CRTs.

static const int C_RedWeight = 54;
static const int C_GreenWeight = 183;
static const int C_BlueWeight = 18;

// Grayscale color shift value
static const int CO_MONO_SHIFT = 8;

// Integer color multiplier
static const float CO_INT_MULT = (float) SHRT_MAX;

// Integer color shift value
static const int CO_INT_SHIFT = 7;

class Spectra   // Average spectral radiant exitance
{
  private:
    float red_band;
    float green_band;
    float blue_band;

  public:
    float GetBlueBand() { return blue_band; }
    float GetGreenBand() { return green_band; }
    float GetRedBand() { return red_band; }
    void Limit()
    {
      red_band = max((float) 0.0, red_band);
      green_band = max((float) 0.0, green_band);
      blue_band = max((float) 0.0, blue_band);
    }
    void Reset()
    { red_band = green_band = blue_band = (float) 0.0; }
    void SetBlueBand( float b ) { blue_band = b; }
    void SetGreenBand( float g ) { green_band = g; }
    void SetRedBand( float r ) { red_band = r; }

    Spectra &Add( Spectra &a )  // Add color
    {
      red_band += a.red_band;
      green_band += a.green_band;
      blue_band += a.blue_band;

      return *this;
    }

    Spectra &Subtract( Spectra &a )     // Subtract color
    {
      red_band -= a.red_band;
      green_band -= a.green_band;
      blue_band -= a.blue_band;

      return *this;
    }


    Spectra &Mult( Spectra &m )    // Multiply color
    {
      red_band *= m.red_band;
      green_band *= m.green_band;
      blue_band *= m.blue_band;

      return *this;
    }

    // Multiply colors
    friend Spectra Mult( Spectra &s1, Spectra &s2 )
    {
      Spectra temp;     // Temporary spectrum

      temp.red_band = s1.red_band * s2.red_band;
      temp.green_band = s1.green_band * s2.green_band;
      temp.blue_band = s1.blue_band * s2.blue_band;

      return temp;
    }

    // Blend colors
    friend Spectra Blend( Spectra &s1, Spectra &s2, double
         alpha )
    {
      Spectra temp;     // Temporary spectrum

      // Linear interpolation
      temp.red_band = s1.red_band + (s2.red_band -
          s1.red_band) * (float) alpha;
      temp.green_band = s1.green_band + (s2.green_band -
          s1.green_band) * (float) alpha;
      temp.blue_band = s1.blue_band + (s2.blue_band -
          s1.blue_band) * (float) alpha;

      return temp;
    }

    double GetMaxColor()        // Get maximum color
    {
      float maximum = (float) 0.0;

      maximum = max(maximum, red_band);
      maximum = max(maximum, green_band);
      maximum = max(maximum, blue_band);

      return (double) maximum;
    }

    void Scale( double value )  // Scale color
    {
      red_band *= (float) value;
      green_band *= (float) value;
      blue_band *= (float) value;
    }
};

class ColorRGB  // 24-bit RGB color model
{
  private:
    BYTE red;
    BYTE green;
    BYTE blue;

  public:
    BYTE GetBlue() { return blue; }
    BYTE GetGreen() { return green; }
    BYTE GetRed() { return red; }
    void SetBlue( BYTE b ) { blue = b; }
    void SetGreen( BYTE g ) { green = g; }
    void SetRed( BYTE r ) { red = r; }

    // Set 24-bit RGB color
    void SetColor( Spectra &c )
    {
      red = (BYTE) (c.GetRedBand() * (float) UCHAR_MAX);
      green = (BYTE) (c.GetGreenBand() * (float) UCHAR_MAX);
      blue = (BYTE) (c.GetBlueBand() * (float) UCHAR_MAX);
    }

    // Change 24-bit RGB color to grayscale
    void SetMono()
    {
      red = green = blue = (BYTE) (((int) red * C_RedWeight
          + (int) green * C_GreenWeight + (int) blue *
          C_BlueWeight) >> CO_MONO_SHIFT);
    }

    // Change 24-bit RGB color to pseudocolor
    void SetPseudo()
    {
      int gsv;          // Grayscale value

      SetMono();        // Change color to grayscale
      gsv = (int) red;  // Get grayscale value

      // Convert grayscale to pseudocolor
      if (gsv < 128)
      {
        red = (BYTE) 0;
        green = (BYTE) (2 * gsv);
        blue = (BYTE) (UCHAR_MAX - 2 * gsv);
      }
      else
      {
        red = (BYTE) (2 * gsv - UCHAR_MAX);
        green = (BYTE) (2 * UCHAR_MAX - 2 * gsv);
        blue = (BYTE) 0;
      }
    }
};

class IntRGB        // Integer RGB color
{
  private:
    int red;
    int green;
    int blue;

  public:
    int GetBlue() { return blue; }
    int GetGreen() { return green; }
    int GetRed() { return red; }
    void AddBlue( int b ) { blue += b; }
    void AddGreen( int g ) { green += g; }
    void AddRed( int r ) { red += r; }
    void SetBlue( int b ) { blue = b; }
    void SetGreen( int g ) { green = g; }
    void SetRed( int r ) { red = r; }

    // Set integer RGB color
    void SetColor( Spectra &c )
    {
      red = (int) (c.GetRedBand() * (float)
          CO_INT_MULT);
      green = (int) (c.GetGreenBand() * (float)
          CO_INT_MULT);
      blue = (int) (c.GetBlueBand() * (float)
          CO_INT_MULT);
    }

    // Set 24-bit RGB color
    void SetColorRGB( ColorRGB *pc )
    {
      pc->SetRed((BYTE) (red >> CO_INT_SHIFT));
      pc->SetGreen((BYTE) (green >> CO_INT_SHIFT));
      pc->SetBlue((BYTE) (blue >> CO_INT_SHIFT));
    }

    // Subtract integer RGB color
    friend IntRGB operator-( IntRGB &a, IntRGB &b )
    {
      IntRGB temp;      // Temporary integer RGB color

      temp.red = a.red - b.red;
      temp.green = a.green - b.green;
      temp.blue = a.blue - b.blue;

      return temp;
    }

    // Subtract constant from integer RGB color
    friend IntRGB operator-( IntRGB &a, int c )
    {
      IntRGB temp;      // Temporary integer RGB color

      temp.red = a.red - c;
      temp.green = a.green - c;
      temp.blue = a.blue - c;

      return temp;
    }

    // Multiply integer RGB color by constant
    friend IntRGB operator*( IntRGB &a, int c )
    {
      IntRGB temp;      // Temporary integer RGB color
      
      temp.red = a.red * c;
      temp.green = a.green * c;
      temp.blue = a.blue * c;

      return temp;
    }
};

#endif

