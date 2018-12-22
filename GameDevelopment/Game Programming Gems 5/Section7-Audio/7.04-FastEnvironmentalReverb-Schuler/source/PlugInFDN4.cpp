// PlugInFDN4.cpp : implementation file
//

#include <fstream>
#include <vector>

#include "stdafx.h"
#include "cSoundBuffer.h"
#include "DSPdemo.h"
#include "PlugInFDN4.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "cMatrix4.h"


struct cPreset
{
    const char *name;
    unsigned matrix;
    unsigned lengths[4];
    unsigned halflife[2];
    unsigned rotation[2];
    float modulation[2];
    unsigned mix[2];
}

staticPresets[] =
{
    {
        // Long delay lengths (= many resonant modes)
        // and little coloration makes the reverb feel "distant".

        "Temple of the Ancestors",
        cDspFDN4::house6,
        { 1439, 2311, 3347, 4973 },
        { 30000, 20000 },
        { 0, 0 },
        { 60000, 0.8f },
        { 70, 30 },
    },

    {
        // Same without direct signal

        "Temple of the Ancestors (back door)",
        cDspFDN4::house6,
        { 1439, 2311, 3347, 4973 },
        { 30000, 3000 },
        { 90, 180 },
        { 60000, 0.8f },
        { 0, 30 },
    },

    {
        // The burst at 6000 samples is responsible for the echoey "open air" feeling.
        // The delay line at 2111 samples is only there to increase time density.
        // An input rotation of -45 degrees cancels any centered input
        // into this delay line, making the first echo at 2111 almost inaudible.

        "Open Stage in the Woods",
        cDspFDN4::house6,
        { 2111, 6121, 6748, 7553 },
        { 14000, 10000 },
        { -45, -45 },
        { 70000, 0.5f },
        { 40, 60 },
    },


    {
        // Medium reverb time, medium delay lengths.
        // Medium dry-wet ratio makes you feel "there".

        "Cardio Fitness at the Gym",
        cDspFDN4::house6,
        { 773, 1709, 3001, 4367 },
        { 9000, 3000 },
        { 0, 0 },
        { 20000, 0.3f },
        { 40, 60 },
    },

    {
        // Oscillator-like resonant modes at low frequencies
        // together high wet-ratio gives strong coloration.

        "U-Boat Club Party",
        cDspFDN4::house6,
        { 696, 983, 2247, 3111 },
        { 5500, 700 },
        { 0, 90 },
        { 20000, 0.3f },
        { 0, 100 },
    },

    {
        // Short delay length, short reverb time.
        // We need a short response time (purpose of the first delay line at 196 samples) and
        // a sufficient number of resonant modes (purpose of the last delay line at 1301 samples),
        // to prevent sounding metallic.

        "Christians former very small Dorm Room",
        cDspFDN4::house6,
        { 196, 417, 537, 1301 },
        { 1700, 500 },
        { 90, 180 },
        { 0, 0 },
        { 20, 80 },
    },

    {
        // Extreme delay length modulation gives pitch shift.
        // A near identity matrix (tinyrot) makes
        // each delay line like a harmonic oscillator.

        "Howling Spirits",
        cDspFDN4::tinyrot,
        { 71, 73, 79, 83 },
        { 1500, 50 },
        { 0, 0 },
        { 125000, 48 },
        { 0, 50 },
    },

    {
        // Short delay lengths and long reverb times make delay lines like oscillators.
        // A rotating feedback matrix makes the resonant modes unharmonic.

        "Leaking Biohazard Container",
        cDspFDN4::staut0,
        { 83, 89, 193, 197 },
        { 6000, 150 },
        { 0, 0 },
        { 0, 0 },
        { 0, 30 },
    },

    {
        // A nearly negative identity matrix makes each delay line
        // like a waveguide with an open ended reflection (sign change),
        // creating the hollow sound.

        "Oversized Shoe Box",
        cDspFDN4::negtiny,
        { 41, 43, 47, 53 },
        { 350, 50 },
        { 0, 180 },
        { 0, 0 },
        { 0, 70 },
    },

    {
        // Strong poles around 10 kHz - 12 kHz call for the ear doctor.

        "Ring my bell",
        cDspFDN4::negtiny,
        { 2, 2, 2, 2 },
        { 4500, 4500 },
        { 0, 0 },
        { 0, 0 },
        { 0, 30 },
    },
};


static cPreset *pPresets = staticPresets;
static unsigned nPresets = sizeof( staticPresets ) / sizeof( cPreset );

// dynamic presets
static std::vector<cPreset> vPresets;


/////////////////////////////////////////////////////////////////////////////
// CPlugInFDN4 dialog


CPlugInFDN4::CPlugInFDN4(CWnd* pParent /*=NULL*/)
    : CPlugIn(CPlugInFDN4::IDD, pParent)
{
    //{{AFX_DATA_INIT(CPlugInFDN4)
    //}}AFX_DATA_INIT


    std::fstream file( "presets.txt", std::ios::in );

    if( !!file )
    {
        char buffer[80];
        file.width( 79 );
        while( !file.eof() )
        {
            file >> buffer;
            if( !strncmp( buffer, "PRESET", 79 ) )
            {
                unsigned i = vPresets.size();
                vPresets.resize( i + 1 );

                // remain allocated till program end ...
                char *name = new char[80];
                file >> name[0];
                file.getline( name + 1, 79 );
                vPresets[i].name = name;

                file >> vPresets[i].matrix;
                file >> vPresets[i].lengths[0];
                file >> vPresets[i].lengths[1];
                file >> vPresets[i].lengths[2];
                file >> vPresets[i].lengths[3];
                file >> vPresets[i].halflife[0];
                file >> vPresets[i].halflife[1];
                file >> vPresets[i].rotation[0];
                file >> vPresets[i].rotation[1];
                file >> vPresets[i].modulation[0];
                file >> vPresets[i].modulation[1];
                file >> vPresets[i].mix[0];
                file >> vPresets[i].mix[1];
            }
        }

        // reseat presets
        pPresets = &vPresets[0];
        nPresets = vPresets.size();
    }
}


void CPlugInFDN4::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CPlugInFDN4)
    DDX_Control(pDX, IDC_SWEEPSPEED, m_Modulation[0]);
    DDX_Control(pDX, IDC_SWEEPDEPTH, m_Modulation[1]);
    DDX_Control(pDX, IDC_IN_ROTATION, m_Rotation[0]);
    DDX_Control(pDX, IDC_OUT_ROTATION, m_Rotation[1]);
    DDX_Control(pDX, IDC_HALFLIFE_LF, m_HalfLife[0]);
    DDX_Control(pDX, IDC_HALFLIFE_HF, m_HalfLife[1]);
    DDX_Control(pDX, IDC_DELAYLENGTH0, m_DelayLength[0]);
    DDX_Control(pDX, IDC_DELAYLENGTH1, m_DelayLength[1]);
    DDX_Control(pDX, IDC_DELAYLENGTH2, m_DelayLength[2]);
    DDX_Control(pDX, IDC_DELAYLENGTH3, m_DelayLength[3]);
    DDX_Control(pDX, IDC_FBMATRIX, m_FeedbackMatrix);
    DDX_Control(pDX, IDC_SELECTPRESET, m_SelectPreset);
    DDX_Control(pDX, IDC_DRYMIX, m_Mix[0]);
    DDX_Control(pDX, IDC_WETMIX, m_Mix[1]);
    DDX_Control(pDX, IDC_MUTEINPUT, m_MuteInput);
    DDX_Control(pDX, IDC_BYPASS, m_Bypass);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPlugInFDN4, CDialog)
    //{{AFX_MSG_MAP(CPlugInFDN4)
        // NOTE: the ClassWizard will add message map macros here
    ON_EN_KILLFOCUS(IDC_DELAYLENGTH0, OnInitLengthsAndParameters)
    ON_EN_KILLFOCUS(IDC_DELAYLENGTH1, OnInitLengthsAndParameters)
    ON_EN_KILLFOCUS(IDC_DELAYLENGTH2, OnInitLengthsAndParameters)
    ON_EN_KILLFOCUS(IDC_DELAYLENGTH3, OnInitLengthsAndParameters)
    ON_EN_KILLFOCUS(IDC_HALFLIFE_LF, OnSetParameters)
    ON_EN_KILLFOCUS(IDC_HALFLIFE_HF, OnSetParameters)
    ON_EN_KILLFOCUS(IDC_IN_ROTATION, OnSetParameters)
    ON_EN_KILLFOCUS(IDC_OUT_ROTATION, OnSetParameters)
    ON_EN_KILLFOCUS(IDC_DRYMIX, OnSetParameters)
    ON_EN_KILLFOCUS(IDC_WETMIX, OnSetParameters)
    ON_EN_KILLFOCUS(IDC_SWEEPSPEED, OnSetParameters)
    ON_EN_KILLFOCUS(IDC_SWEEPDEPTH, OnSetParameters)
    ON_CBN_SELENDOK(IDC_FBMATRIX, OnSetParameters)
    ON_CBN_SELENDOK(IDC_SELECTPRESET, OnSelectPreset)
    ON_BN_CLICKED(IDC_BYPASS, NotifyParameterChange)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlugInFDN4 message handlers


void CPlugInFDN4::OnInitLengthsAndParameters()
{
    CString text;
    unsigned newlengths[4] = { 0, 0, 0, 0 };

    for( unsigned i = 0; i < 4; i++ )
    {
        m_DelayLength[i].GetWindowText( text );
        newlengths[i] = atoi( text );
        if( newlengths[i] < 1 )
            newlengths[i] = 1;
        if( newlengths[i] > 16384 )
            newlengths[i] = 16384;
    }

    m_FDN4.Init( newlengths );

    OnSetParameters();
}


void CPlugInFDN4::OnSetParameters()
{
    CString text;
    float rotation[2] = { 0, 0 };
    float halflife[2] = { 0, 0 };
    float mix[2] = { 0, 0 };
    float modulation[2] = { 0, 0 };

    m_HalfLife[0].GetWindowText( text );
    halflife[0] = atof( text );
    m_HalfLife[1].GetWindowText( text );
    halflife[1] = atof( text );
    m_Rotation[0].GetWindowText( text );
    rotation[0] = atof( text );
    m_Rotation[1].GetWindowText( text );
    rotation[1] = atof( text );
    m_Mix[0].GetWindowText( text );
    mix[0] = .01f * atof( text );
    m_Mix[1].GetWindowText( text );
    mix[1] = .01f * atof( text );
    m_Modulation[0].GetWindowText( text );
    modulation[0] = atof( text );
    m_Modulation[1].GetWindowText( text );
    modulation[1] = atof( text );

    m_FDN4.SetFeedbackMatrix( m_FeedbackMatrix.GetCurSel() );
    m_FDN4.SetHalflives( halflife[0], halflife[1] );
    m_FDN4.SetInOutRotations( rotation[0], rotation[1] );
    m_FDN4.SetModulation( modulation[0], modulation[1] );
    m_FDN4.SetMixes( mix[0], mix[1] );

    NotifyParameterChange();

}


static const char *sMatrixNames[] =
{
    "Householder 0",
    "Householder 1",
    "Householder 2",
    "Householder 3",
    "Householder 4",
    "Householder 5",
    "Householder 6",
    "Householder 7",
    "Stautner 0",
    "Stautner 1",
    "Stautner 2",
    "Stautner 3",
    "Hadamard 0",
    "Hadamard 1",
    "Hadamard 2",
    "Hadamard 3",
    "Identity",
    "Tiny Rotation",
    "Little Rotation",
    "Medium Rotation",
    "Negative Identity",
    "Negative Tiny Rotation",
    "Negative Little Rotation",
    "Negative Medium Rotation",
};


static const unsigned sNumMatrixNames = sizeof( sMatrixNames ) / sizeof( const char * );


BOOL CPlugInFDN4::OnInitDialog()
{
    CDialog::OnInitDialog();

    { for( unsigned i = 0; i < sNumMatrixNames; i++ )
    {
        m_FeedbackMatrix.AddString( sMatrixNames[i] );
        m_FeedbackMatrix.SetItemData( i, i );
    } }

    { for( unsigned i = 0; i < nPresets; i++ )
    {
        m_SelectPreset.AddString( pPresets[i].name );
        m_SelectPreset.SetItemDataPtr( i, pPresets + i );
    } }

    m_SelectPreset.SetCurSel(0);
    OnSelectPreset();

    return TRUE;
}


void CPlugInFDN4::OnSelectPreset()
{
    CString text;
    unsigned i = m_SelectPreset.GetCurSel();

    m_FeedbackMatrix.SetCurSel( pPresets[i].matrix );


    text.Format( "%d", pPresets[i].lengths[0] );
    m_DelayLength[0].SetWindowText( text );
    text.Format( "%d", pPresets[i].lengths[1] );
    m_DelayLength[1].SetWindowText( text );
    text.Format( "%d", pPresets[i].lengths[2] );
    m_DelayLength[2].SetWindowText( text );
    text.Format( "%d", pPresets[i].lengths[3] );
    m_DelayLength[3].SetWindowText( text );

    text.Format( "%d", pPresets[i].halflife[0] );
    m_HalfLife[0].SetWindowText( text );
    text.Format( "%d", pPresets[i].halflife[1] );
    m_HalfLife[1].SetWindowText( text );

    text.Format( "%d", pPresets[i].rotation[0] );
    m_Rotation[0].SetWindowText( text );
    text.Format( "%d", pPresets[i].rotation[1] );
    m_Rotation[1].SetWindowText( text );

    text.Format( "%.0f", pPresets[i].modulation[0] );
    m_Modulation[0].SetWindowText( text );
    text.Format( "%.2f", pPresets[i].modulation[1] );
    m_Modulation[1].SetWindowText( text );

    text.Format( "%d", pPresets[i].mix[0] );
    m_Mix[0].SetWindowText( text );
    text.Format( "%d", pPresets[i].mix[1] );
    m_Mix[1].SetWindowText( text );

    OnInitLengthsAndParameters();
}

static cSoundBuffer<float> altbufferL;
static cSoundBuffer<float> altbufferR;



void CPlugInFDN4::Process( cSoundBuffer<float> &buffer )
{
    bool bypass = ( m_Bypass.GetCheck() != 0 );
    bool muteinput = ( m_MuteInput.GetCheck() != 0 );

    if( bypass )
    {
        if( muteinput )
        {
            altbufferL.Resize( buffer.GetSize() );
            m_FDN4.Process( altbufferL );
        }
        else
        {
            altbufferL = buffer;
            m_FDN4.Process( altbufferL );
        }
    }
    else
    {
        if( muteinput )
        {
            buffer.Zero();
            m_FDN4.Process( buffer );
        }
        else
        {
            m_FDN4.Process( buffer );
        }

    }
}



void CPlugInFDN4::ProcessStereo( cSoundBuffer<float> &bufferL, cSoundBuffer<float> &bufferR )
{
    bool bypass = ( m_Bypass.GetCheck() != 0 );
    bool muteinput = ( m_MuteInput.GetCheck() != 0 );

    if( bypass )
    {
        if( muteinput )
        {
            altbufferL.Resize( bufferL.GetSize() );
            altbufferR.Resize( bufferR.GetSize() );
            m_FDN4.ProcessStereo( altbufferL, altbufferR );
        }
        else
        {
            altbufferL = bufferL;
            altbufferR = bufferR;
            m_FDN4.ProcessStereo( altbufferL, altbufferR );
        }
    }
    else
    {
        if( muteinput )
        {
            bufferL.Zero();
            bufferR.Zero();
            m_FDN4.ProcessStereo( bufferL, bufferR );
        }
        else
        {
            m_FDN4.ProcessStereo( bufferL, bufferR );
        }
    }

    if( m_FDN4.m_sweepdivisor != 0 )
    {
        NotifyParameterChange();
    }
}


std::complex<float> CPlugInFDN4::TransferFunction( const std::complex<float> &z ) const
{
    // This function calculates the transfer function of the whole FDN module.
    // The transfer function of an FDN is

    // H(z) = y/x = c * ( D^-1 - A )^-1 * d^T ),

    // with dry/wet mix it becomes

    // H(z) = y/x = c * ( D^-1 - A )^-1 * d^T ) * wet + dry,

    // where
    // c: input matrix
    // d: output matrix
    // A: feedback matrix
    // D: delay matrix = diag( g(z) * z^-m )
    //      where g(z) is the frequency dependent gain
    //      and m is the delay length.

    // since D is diagonal, D(z)^-1 == D(z^-1)

    bool bypass = ( m_Bypass.GetCheck() != 0 );
    if( bypass )
    {
        return 1;
    }
    else
    {
        cComplexVector4 g;

        // Step 1:
        // calculate frequency dependent gain due to scalar gain and filter
        // The formula is

        // g(z) = g * alpha / ( 1 - ( 1 - alpha ) / z )

        g[0] = m_FDN4.m_g[0] * m_FDN4.m_alpha_orig[0] / ( 1.f - ( 1.f - m_FDN4.m_alpha_orig[0] ) / z );
        g[1] = m_FDN4.m_g[1] * m_FDN4.m_alpha_orig[1] / ( 1.f - ( 1.f - m_FDN4.m_alpha_orig[1] ) / z );
        g[2] = m_FDN4.m_g[2] * m_FDN4.m_alpha_orig[2] / ( 1.f - ( 1.f - m_FDN4.m_alpha_orig[2] ) / z );
        g[3] = m_FDN4.m_g[3] * m_FDN4.m_alpha_orig[3] / ( 1.f - ( 1.f - m_FDN4.m_alpha_orig[3] ) / z );

        // Step 2:
        // Construct matrices A and D^-1
        // The factor 1e-8 is just to increase numerical stability
        // (shift the float range to where the action is)

        cComplexMatrix4 A, invD;

        A = m_FDN4.m_A;
        A = 1e-8f * A;

        invD.zero();
        invD[0][0] = 1e-8f / ( std::pow( z, - m_FDN4.GetActualLength(0) ) * g[0] );
        invD[1][1] = 1e-8f / ( std::pow( z, - m_FDN4.GetActualLength(1) ) * g[1] );
        invD[2][2] = 1e-8f / ( std::pow( z, - m_FDN4.GetActualLength(2) ) * g[2] );
        invD[3][3] = 1e-8f / ( std::pow( z, - m_FDN4.GetActualLength(3) ) * g[3] );

        // Step 3:
        // Calculate the inner transfer matrix Y = ( D^-1 - A )^-1 X for vector signals X and Y.

        cComplexMatrix4 H_inner;
        H_inner = 1e-8f * inverse( invD - A );

        // Step 4:
        // Complete the calculation with input and output matrices.
        // The problem here is that with stereo inputs, or any multichannel inputs
        // for that matter, the transfer function is theoretically different for
        // any stereo angle.

        // The input is assumed to be a mix of all stereo angles, and it is assumed
        // that all stereo angles of the output are heard by the listener.
        // (So that the waves from the left and right speaker add incoherently).

        // Therefore we return the average of the absolute values of the transfer function
        // for all stereo combinations.

        cComplexVector4 cL, cR, dL, dR;

        cL = m_FDN4.m_c[0];
        cR = m_FDN4.m_c[1];

        dL[0] = m_FDN4.m_d[0][0];
        dL[1] = m_FDN4.m_d[1][0];
        dL[2] = m_FDN4.m_d[2][0];
        dL[3] = m_FDN4.m_d[3][0];

        dR[0] = m_FDN4.m_d[0][1];
        dR[1] = m_FDN4.m_d[1][1];
        dR[2] = m_FDN4.m_d[2][1];
        dR[3] = m_FDN4.m_d[3][1];

        std::complex<float> result =
            .25f * (
                std::abs(
                    dot( mul( cL, H_inner ), dL ) +
                    dot( mul( cR, H_inner ), dL ) ) +
                std::abs(
                    dot( mul( cL, H_inner ), dR ) +
                    dot( mul( cR, H_inner ), dR ) ) );

        // Step 5:
        // Finally return the result weighted with dry/wet ratio

        return fabsf( m_FDN4.m_dry ) + result * m_FDN4.m_wet;
    }
}


void CPlugInFDN4::Reset()
{
    OnInitLengthsAndParameters();
}
