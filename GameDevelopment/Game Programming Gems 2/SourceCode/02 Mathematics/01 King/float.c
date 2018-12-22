/* Copyright (C) Yossarian King, 2001. 
 * All rights reserved worldwide.
 *
 * This software is provided "as is" without express or implied
 * warranties. You may freely copy and compile this source into
 * applications you distribute provided that the copyright text
 * below is included in the resulting source code, for example:
 * "Portions Copyright (C) Yossarian King, 2001"
 */
/*
    float.c

    Sample code for "Floating-point Tricks -- Improving
    performance with IEEE floating-point", from Graphics
    Gems 2.

    Yossarian King / Electronic Arts Canada / April 2001
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>


/**************************************************************
    Portability

    Hopefully you can just modify stuff in this section to get
    this to work on other compilers and platforms. I've tried
    to make the code easily portable, but it's only been compiled
    with Microsoft Visual C++ and run on Windows PC's. 
 **************************************************************/

/* Message display. */
extern void     message(char *sz);
static char     szMsg[256];

/* Define this however your compiler likes it. */
#define INLINE      __inline 

/* Get cycle count -- this is extremely Pentium-specific. Any very precise and
   stable timer should work. If precision is limited you may get better results
   by increasing the NCALLS define below. */

INLINE  unsigned int    getcycle(void)
{
    __asm
    {
        cpuid
        rdtsc
    }
}

/**************************************************************
    Common structures and defines

     Stuff used throughout the code below.
 **************************************************************/

/* INTORFLOAT union for easy access to bits of a float. */
typedef union
{
    int     i;          // as integer
    float   f;          // as float
    struct              // as bit fields
    {
        unsigned int    sign:1;
        unsigned int    biasedexponent:8;
        unsigned int    significand;
    }
            bits;
}
INTORFLOAT;

/* Bias constant used for fast conversions between int and float. First element
   in INTORFLOAT union is integer -- we are storing biased exponent 23, mantissa .1, which
   is equivalent to 1.5 x 2^23. */
INTORFLOAT  bias = {((23 + 127) << 23) + (1 << 22)};

/**************************************************************
    Linear lookup for sine and cosine

    This section implements a lookup table version of sin
    and cos, both using the same table.
 **************************************************************/

#define SINTABLESIZE    256
#define TWOPISCALE      ((float)SINTABLESIZE / (2.0f * 3.14159265f))

static float    sintable[SINTABLESIZE];

/*
    Initialize the lookup table.
*/

void    triginit(void)
{
    int     i;
    double  theta;

    /* Integer range 0 to SINTABLESIZE is converted to floating
       point 0 to 2 pi range (radians for a full circle). */
    for (i = 0; i < SINTABLESIZE; ++i)
    {
        theta = (double)i/TWOPISCALE;
        sintable[i] = (float)sin(theta);
    }
}

/*
    fsin - sin calculation with lookup table.
*/

INLINE float    fsin(float theta)
{
    unsigned int    i;

#if 1   /* Use biasing trick for fast float to int conversion. */
    INTORFLOAT  fi;
    fi.f = theta * TWOPISCALE + bias.f; /* Scale to table index range and add conversion bias. */
    i = fi.i & (SINTABLESIZE - 1);         /* Mask off lower bits, assuming SINTABLESIZE is power of 2. */
#else   /* Use typecasting for slow float to int conversion. */
    i = ((unsigned int)(theta * TWOPISCALE)) & (SINTABLESIZE - 1);
#endif

    /* Return sine from table. */
    return sintable[i];
}

/*
    fcos - cos calculation with lookup table.
*/

INLINE float    fcos(float theta)
{
    unsigned int    i;

    INTORFLOAT  fi;
    fi.f = theta * TWOPISCALE + bias.f;             /* Scale to table index range and add conversion bias. */

    /* Note how adding SINTABLESIZE/4 lets us compute cos instead of sin,
       since cos(theta) = sin(theta + Pi/4). */
    i = (fi.i + (SINTABLESIZE/4)) & (SINTABLESIZE - 1);   /* Mask off lower bits, assuming SINTABLESIZE is power of 2. */

    return sintable[i];
}

/**************************************************************
    Logarithmic lookup for square root.
 **************************************************************/

#define SQRTTABLESIZE       256             /* Note: code below assumes this is 256. */
static unsigned int sqrttable[SQRTTABLESIZE];

void    sqrtinit(void)
{
    int             i;
    INTORFLOAT      fi;
    unsigned int    e0;

    /* These two tables are indexed into by the least significant bit (e0)
       of the biased exponent. exponent[] gives the biased exponent
       corresponding to e0, while biasrestore[] gives the amount to be
       added back to the exponent after dividing it by 2 in order to
       restore the bias to the correct value. */
    unsigned int    exponent[2] = { (1+127)<<23, (0+127)<<23 };
    unsigned int    biasrestore[2] = { 63<<23, 64<<23 };

    for (i = 0; i < SQRTTABLESIZE; ++i)
    {
        /* Build the floating point number for this table entry. Table index
           is 8 bits, which we use as e0m22m21...m16. To turn this into a
           floating point number we add in the correct exponent bias and set
           m15 to 1 (which is like adding 0.5, so we hit the middle of the
           range of numbers that we're estimating). */
        e0 = (i >> 7);
        fi.i = exponent[e0] | (i << 16) | (1 << 15);

        /* Compute square root. */
        fi.f = (float)sqrt((double)fi.f);

        /* Extract 23 bit mantissa of square root. */
        sqrttable[i] = fi.i & ((1 << 23) - 1);

        /* Exponent is calculated programmatically by fsqrt, so we don't
           include it in the table. However, the programmatic calculation
           divides the exponent bias by 2, so we put a bias restore into
           the table -- basically adding 63 or 64, depending on whether the
           exponent was odd or even, which we can determine from e0. */
        sqrttable[i] += biasrestore[e0];
    }
}

INLINE float    fsqrt(float f)
{
    INTORFLOAT      fi;
    unsigned int    e, n;

    /* Get raw bits of floating point f. */
    fi.f = f;
    n = fi.i;

    /* Divide exponent by 2 -- this is done in-place, no need to shift all
       the way down to 0 the least significant bits and then back up again.
       Note that we are also dividing the exponent bias (127) by 2, this
       gets corrected when we add in the sqrttable entry. */
    e = (n >> 1) & 0x3f800000;

    /* n is the table index -- we're using 1 bit from the original exponent
       (e0) plus 7 bits from the mantissa. */
    n = (n >> 16) & 0xff;

    /* Add calculated exponent to mantissa + re-biasing constant from table. */
    fi.i = e + sqrttable[n];

    /* Return floating point result. */
    return fi.f;
}

/**************************************************************
    API for general linear lookup tables.
 **************************************************************/

/* Following structure stores everything required for the lookup table
   for a particular function. */
typedef struct
{
    int     nSize;          /* Number of table entries. */
    float   indexScale;     /* For scaling table index to get float. */
    float   indexOffset;    /* And offset. */
    float   indexOffsetBias;/* Used for float to int conversion combined with offset. */
    float   *pTable;        /* Memory for table. */
}
LUT;

LUT *LUT_init(float (*pFunc)(float fArg), float fMin, float fMax, int nSize)
{
    LUT *lut;
    int i;

    lut = (LUT *)malloc(sizeof(LUT));
    lut->pTable = (float *)malloc(nSize * sizeof(float));

    lut->nSize = nSize - 1;     /* Will be used as a mask; assumes power of 2. */

    /* Given a float f, index into table is calculated as:
            i = (f - offset) * scale + bias
              = f * scale + (bias - offset * scale)
       where 'bias' is for converting float to int. */
    lut->indexScale = ((float)nSize / (fMax - fMin));
    lut->indexOffset = fMin;
    lut->indexOffsetBias = bias.f - lut->indexOffset * lut->indexScale;

    /* Fill in table. */
    for (i = 0; i < nSize; ++i)
    {
        lut->pTable[i] = pFunc((float)i / lut->indexScale + lut->indexOffset);
    }

    return lut;
}

__inline float  LUT_compute(float fArg, LUT *lut)
{
    unsigned int    i;
    INTORFLOAT      fi;

    fi.f = fArg * lut->indexScale + lut->indexOffsetBias;
    i = fi.i & lut->nSize;

    return lut->pTable[i];
}


/**************************************************************
    Other odds and ends.
 **************************************************************/

/* --------- clamping -------- */

/* Clamp with floats. */
__inline float  clampf0(float f)
{
    if (f < 0.0f)
        return 0.0f;

    return f;
}

/* Clamp with ints. */
__inline float  clampi0(float f)
{
// NOTE: here's a fast absolute value:
//    *(int *)&f &= 0x7fffffff;

#if 1   // no compares
    int s = (*(int *)&f) >> 31;
    s = ~s;
    *(int *)&f &= s;
#else
    if (*(int *)&f < 0)
        return 0.0f;
#endif

    return f;
}

/* Clampt to 1. */
__inline float  clampf1(float f)
{
    if (f > 1.0f)
        return 1.0f;

    return f;
}

__inline float  clampi1(float f)
{
    if (*(int *)&f > 1.0f)
        return 1.0f;

    return f;
}

/**************************************************************
    Benchmark code for all that stuff above.
 **************************************************************/

/* Run each test NTIMES times, make NCALLS calls to the function
   being tested. Running the test multiple times ensures the cache
   is warmed up, making many calls gives more accurate timing. */
#define NTIMES          3
#define NCALLS          1000

/* Tables to store input and output values. If values are stored in
   a scalar variable then compiler will typically optimize it into
   a register, or even optimize the calculation away entirely, which
   makes benchmarking difficult. */
float   finput[NCALLS];
float   foutput[NCALLS];

void    testtrig(void)
{
    /* Count cycles for each function. */
    unsigned int    cycles_sin[NTIMES];
    unsigned int    cycles_cos[NTIMES];
    unsigned int    cycles_fsin[NTIMES];
    unsigned int    cycles_fcos[NTIMES];

    float   maxsinerr = 0.0f;
    float   maxsintheta;
    float   maxcoserr = 0.0f;
    float   maxcostheta;
    float   ferr;

    int             j, i;

    triginit();

    /* Set tables to zero, hopefully will get tables into cache. */
    memset(finput,  0, NCALLS * sizeof(float));
    memset(foutput, 0, NCALLS * sizeof(float));

    /* Generate some random angles. */
    for (i = 0; i < NCALLS; ++i)
    {
        finput[i] = (((float)(rand() % 10000)) / 10000.0f - 0.5f) * 6.28f * 1000.0f;
    }

    /* Pentium rdtsc instruction may behave erratically on first couple calls,
       so make a couple extra calls initially. */
    getcycle();
    getcycle();
    getcycle();
    
    for (j = 0; j < NTIMES; ++j)
    {
        /* Call sin NCALLS times and see how long it took. */
        cycles_sin[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = (float)sin((double)finput[i]);
        }
        cycles_sin[j] = getcycle() - cycles_sin[j];

        cycles_cos[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = (float)cos((double)finput[i]);
        }
        cycles_cos[j] = getcycle() - cycles_cos[j];

        cycles_fsin[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = fsin(finput[i]);
        }
        cycles_fsin[j] = getcycle() - cycles_fsin[j];

        cycles_fcos[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = fcos(finput[i]);
        }
        cycles_fcos[j] = getcycle() - cycles_fcos[j];
    }

    for (j = 0; j < NTIMES; ++j)
    {
        sprintf(szMsg, "testtrig: %d: sin = %.1f cycles; fsin = %.1f cycles; cos = %.1f cycles; fcos = %.1f cycles\n",
                        j,
                        (float)cycles_sin[j]/(float)NCALLS,
                        (float)cycles_fsin[j]/(float)NCALLS,
                        (float)cycles_cos[j]/(float)NCALLS,
                        (float)cycles_fcos[j]/(float)NCALLS);
        message(szMsg);
    }

    /* Check for maximum error. */
    /* The increment in the angle going through the table is
       (2 PI/TABLESIZE). The sin curve is steepest close to 0 (well, anywhere the
       curve cuts the x axis) so an angle close to 0 will give the greatest error.
       You might expect the greatest error at (2 PI/TABLESIZE - epsilon), but the
       bias trick for floating point conversion rounds off to the nearest integer,
       so the greatest error is actually at (2 PI/TABLESIZE * 0.5). And using the
       fact that sin(a) ~= a for small angles, we can say the worst case error is
       about PI/TABLESIZE.
       
       In the test we've run here, this is obscured by the fact that we've chosen
       angles that go way outside the range 0 to 2 PI. Uncomment the next line
       to convince yourself this analysis is correct. */
    finput[0] = 3.14159f / (float)SINTABLESIZE;
    for (i = 0; i < NCALLS; ++i)
    {
        ferr = (float)fabs(fsin(finput[i]) - sin(finput[i]));
        if (ferr > maxsinerr)
        {
            maxsinerr = ferr;
            maxsintheta = finput[i];
        }

        ferr = (float)fabs(fcos(finput[i]) - cos(finput[i]));
        if (ferr > maxcoserr)
        {
            maxcoserr = ferr;
            maxcostheta = finput[i];
        }
    }

    sprintf(szMsg, "max sin error = %.6f (@theta = %.4f)", maxsinerr, maxsintheta);
    message(szMsg);

    sprintf(szMsg, "max cos error = %.6f (@theta = %.4f)", maxcoserr, maxcostheta);
    message(szMsg);
}

void    testsqrt(void)
{
    /* Count cycles for each function. */
    unsigned int    cycles_sqrt[NTIMES];
    unsigned int    cycles_fsqrt[NTIMES];

    float   maxerr = 0.0f;
    float   maxval;
    float   ferr;

    int             j, i;

    sqrtinit();

    /* Set tables to zero, hopefully will get tables into cache. */
    memset(finput,  0, NCALLS * sizeof(float));
    memset(foutput, 0, NCALLS * sizeof(float));

    /* Generate some random values to take the square root of. */
    for (i = 0; i < NCALLS; ++i)
    {
        finput[i] = (((float)(rand() % 10000)) / 10000.0f) * 10000.0f;
    }

    /* Pentium rdtsc instruction may behave erratically on first couple calls,
       so make a couple extra calls initially. */
    getcycle();
    getcycle();
    getcycle();
    
    for (j = 0; j < NTIMES; ++j)
    {
        /* Call sqrt NCALLS times and see how long it took. */
        cycles_sqrt[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = (float)sqrt((double)finput[i]);
        }
        cycles_sqrt[j] = getcycle() - cycles_sqrt[j];

        cycles_fsqrt[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = fsqrt(finput[i]);
        }
        cycles_fsqrt[j] = getcycle() - cycles_fsqrt[j];
    }

    for (j = 0; j < NTIMES; ++j)
    {
        sprintf(szMsg, "testsqrt: %d: sqrt = %.1f cycles; fsqrt = %.1f cycles\n",
                        j,
                        (float)cycles_sqrt[j]/(float)NCALLS,
                        (float)cycles_fsqrt[j]/(float)NCALLS);
        message(szMsg);
    }

    /* Check for maximum percentage error. Maximum absolute error will increase with
       larger input values, since we only have 8 bits of precision. Percentage error
       should be reasonably constant. */
    for (i = 0; i < NCALLS; ++i)
    {
        ferr = 100.0f * (float)(fabs(fsqrt(finput[i]) - sqrt(finput[i]))/sqrt(finput[i]));
        if (ferr > maxerr)
        {
            maxerr = ferr;
            maxval = finput[i];
        }
    }

    sprintf(szMsg, "max sqrt error = %.1f%% (@ %.6f)", maxerr, maxval);
    message(szMsg);
}

/*
    Test sin() implemented with LUT API.
*/

/* mysin is just like sin, but with floats. */
float   mysin(float fTheta)
{
    return (float)sin((double)fTheta);
}

void    testlutsin(void)
{
    /* Count cycles for each function. */
    unsigned int    cycles_sin[NTIMES];
    unsigned int    cycles_fsin[NTIMES];

    float   maxsinerr = 0.0f;
    float   maxsintheta;
    float   ferr;

    int     j, i;
    LUT     *lutsin;

    lutsin = LUT_init(mysin, 0.0f, 2.0f * 3.14159265f, 256);

    /* Set tables to zero, hopefully will get tables into cache. */
    memset(finput,  0, NCALLS * sizeof(float));
    memset(foutput, 0, NCALLS * sizeof(float));

    /* Generate some random angles. */
    for (i = 0; i < NCALLS; ++i)
    {
        finput[i] = (((float)(rand() % 10000)) / 10000.0f - 0.5f) * 6.28f * 1000.0f;
    }

    /* Pentium rdtsc instruction may behave erratically on first couple calls,
       so make a couple extra calls initially. */
    getcycle();
    getcycle();
    getcycle();
    
    for (j = 0; j < NTIMES; ++j)
    {
        /* Call sin NCALLS times and see how long it took. */
        cycles_sin[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = (float)sin((double)finput[i]);
        }
        cycles_sin[j] = getcycle() - cycles_sin[j];

        cycles_fsin[j] = getcycle();
        for (i = 0; i < NCALLS; ++i)
        {
            foutput[i] = LUT_compute(finput[i], lutsin);
        }
        cycles_fsin[j] = getcycle() - cycles_fsin[j];
    }

    for (j = 0; j < NTIMES; ++j)
    {
        sprintf(szMsg, "testlutsin: %d: sin = %.1f cycles; lutsin = %.1f cycles\n",
                        j,
                        (float)cycles_sin[j]/(float)NCALLS,
                        (float)cycles_fsin[j]/(float)NCALLS);
        message(szMsg);
    }

    /* Check for maximum error. */
    /* The increment in the angle going through the table is
       (2 PI/TABLESIZE). The sin curve is steepest close to 0 (well, anywhere the
       curve cuts the x axis) so an angle close to 0 will give the greatest error.
       You might expect the greatest error at (2 PI/TABLESIZE - epsilon), but the
       bias trick for floating point conversion rounds off to the nearest integer,
       so the greatest error is actually at (2 PI/TABLESIZE * 0.5). And using the
       fact that sin(a) ~= a for small angles, we can say the worst case error is
       about PI/TABLESIZE.
       
       In the test we've run here, this is obscured by the fact that we've chosen
       angles that go way outside the range 0 to 2 PI. Uncomment the next line
       to convince yourself this analysis is correct. */
    finput[0] = 3.14159f / (float)SINTABLESIZE;
    for (i = 0; i < NCALLS; ++i)
    {
        ferr = (float)fabs(LUT_compute(finput[i], lutsin) - sin(finput[i]));
        if (ferr > maxsinerr)
        {
            maxsinerr = ferr;
            maxsintheta = finput[i];
        }
    }

    sprintf(szMsg, "max lutsin error = %.6f (@theta = %.4f)", maxsinerr, maxsintheta);
    message(szMsg);
}

unsigned int    testftoi(int count)
{
    volatile float  f = 321.123f;
    volatile unsigned int    n;
    int             i;
    unsigned int    cycles;

    getcycle();
    getcycle();
    cycles = getcycle();

    /* disassemble this to see nasty "call _ftol", which
       changes FPU rounding mode, converts, changes rounding
       mode back. Bad on Pentium, worse on P3/4. */
    for (i = 0; i < count; ++i)
    {
        n = (unsigned int)f;
    }

    cycles = getcycle() - cycles;
    return cycles;
}

unsigned int    testitof(int count)
{
    volatile unsigned int   n = 123;
    volatile float          f;
    int                     i;
    unsigned int            cycles;

    getcycle();
    getcycle();
    cycles = getcycle();

    for (i = 0; i < count; ++i)
    {
        f = (float)n;
    }

    cycles = getcycle() - cycles;
    return cycles;
}

unsigned int    testfbiasi(int count)
{
    // - define or variable takes same length of time
    // use 23-n to get n bits after the binary point
//    #define FTOIBIAS        8388608.0f
    static unsigned int     ftoibias = ((23-0 + 127) << 23);    // add (1 << 22) to handle negative numbers
    volatile unsigned int    n;
    volatile float  f = -321.123456789f;
    float           ftmp;
    int             i;
    unsigned int    cycles;

    getcycle();
    getcycle();
    cycles = getcycle();

    for (i = 0; i < count; ++i)
    {
//        ftmp = f + FTOIBIAS;
        ftmp = f + *(float *)&ftoibias;
        n = (*(unsigned int *)&ftmp) & 0x003fffff;  // mask off upper 10 bits of garbage in exponent and mantissa
                                                    // Need to duplicate sign bit into upper bits to handle
                                                    // negative numbers correctly.
    }

    cycles = getcycle() - cycles;

    return cycles;
}

unsigned int    testfbiasi2(int count)
{
    // - define or variable takes same length of time
    // use 23-n to get n bits after the binary point
//    #define FTOIBIAS        8388608.0f
    INTORFLOAT   ftoibias = {((23-0 + 127) << 23) /*+ (1 << 22)*/};    // add (1 << 22) to handle negative numbers
    volatile int    n = -321;
    volatile float  f = 8388609.0f;
    INTORFLOAT   ftmp;
    int          i;
    unsigned int cycles;

    getcycle();
    getcycle();
    cycles = getcycle();

    for (i = 0; i < count; ++i)
    {
#if 0   // int to float
        ftmp.i = n;
        ftmp.i += ftoibias.i;
        ftmp.f -= ftoibias.f;
        f = ftmp.f;
#else   // float to int
        ftmp.f = f;
        ftmp.f += ftoibias.f;
        ftmp.i -= ftoibias.i;
        n = ftmp.i;
#endif
    }

    cycles = getcycle() - cycles;

    return cycles;
}

int	main(int argc, char *argv[])
{
    testtrig();
    testsqrt();
    testlutsin();
}
