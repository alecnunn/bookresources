// DSPdemoDlg.cpp : implementation file
//

#include "stdafx.h"
#include "DSPdemo.h"
#include "DSPdemoDlg.h"
#include "WaveFile.h"

#include <math.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDSPdemoDlg dialog

CDSPdemoDlg::CDSPdemoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CDSPdemoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDSPdemoDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_TimerID = 0;
	m_Playing = false;
	m_CurrentEffect = 0;
}

void CDSPdemoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDSPdemoDlg)
	DDX_Control(pDX, IDC_SCROLLFREQ, m_ScrollFreq);
	DDX_Control(pDX, IDC_CHECKSTEREO, m_CheckStereo);
	DDX_Control(pDX, IDC_EDITPROPERTIES, m_EditProperties);
	DDX_Control(pDX, IDC_EDITSAMPLERATE, m_EditSamplerate);
	DDX_Control(pDX, IDC_TRANSFERGRAPH, m_TransferGraph);
	DDX_Control(pDX, IDC_CHECKLOWRANGE, m_CheckLowRange);
	DDX_Control(pDX, IDC_EFFECTAREA, m_EffectArea);
	DDX_Control(pDX, IDC_INPUTCLICK, m_InputPathButton);
	DDX_Control(pDX, IDSTOP, m_StopButton);
	DDX_Control(pDX, IDPLAY, m_PlayButton);
	DDX_Control(pDX, IDC_VOLUMEEDIT, m_VolumeEdit);
	DDX_Control(pDX, IDC_VOLUMESLIDER, m_VolumeSlider);
	DDX_Control(pDX, IDC_INPUTPATH, m_InputPath);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDSPdemoDlg, CDialog)
	//{{AFX_MSG_MAP(CDSPdemoDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_INPUTCLICK, OnInputClick)
	ON_BN_CLICKED(IDPLAY, OnPlay)
	ON_BN_CLICKED(IDSTOP, OnStop)
	ON_WM_TIMER()
	ON_WM_HSCROLL()
	ON_EN_KILLFOCUS(IDC_VOLUMEEDIT, OnKillfocusVolumeEdit)
	ON_EN_KILLFOCUS(IDC_EDITSAMPLERATE, OnKillfocusEditSamplerate)
	ON_EN_KILLFOCUS(IDC_INPUTPATH, OnKillfocusInputPath)
	ON_BN_CLICKED(IDC_CHECKLOWRANGE, OnCheckLowRange)
	ON_BN_CLICKED(IDC_RESET, OnReset)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDSPdemoDlg::UpdateTransferGraph()
{
	if( !IsIconic() )
	{
		CDC dc;
		CDC *dcget;
		CBrush background;
		CPen graphpen;
		CPen gridpen;
		CBitmap bitmap;
		CFont font;
		CRect rect;
		CRgn rgn;

		bitmap.Attach( m_TransferGraph.GetBitmap() );
		dcget = GetDC();
		dc.CreateCompatibleDC( dcget );
		CBitmap *oldbitmap = dc.SelectObject( &bitmap );

		background.CreateStockObject( WHITE_BRUSH );
		graphpen.CreatePen( PS_SOLID, 1, 0x000000b5 );
		gridpen.CreatePen( PS_DOT, 1, 0x00e0e0e0 );
		
		m_TransferGraph.GetClientRect( &rect );
		dc.FillRect( rect, &background );

		font.CreateStockObject( DEFAULT_GUI_FONT );
		dc.SelectObject( font );
		CSize textextent = dc.GetTextExtent( "000" );

		int i = 0;
		int padding = textextent.cy;
		int graphwidth = rect.Width() - 2 * padding - textextent.cx;
		int graphheight = rect.Height() - 2 * padding - textextent.cy;
		int graphorgx = textextent.cx + padding;
		int graphorgy = padding;

		bool lowrange = ( m_CheckLowRange.GetCheck() != 0 );

		SCROLLINFO si;
		m_ScrollFreq.GetScrollInfo( &si );

		double xbase = 0.001 * si.nPos;
		double xprop = 0.001 * si.nPage / graphwidth;
		double pi2 = 8 * atan( 1 );
		double volume = exp( m_VolumeSlider.GetPos() * log( 10 ) / 20 );

		dc.SelectObject( &gridpen );


		static const double freqsH[] = { 10, 32, 100, 320, 1000, 3200, 10000, 32000, 0 };
		static const CString labelsxH[] = { "10", "32", "100", "320", "1k", "3.2k", "10k", "32k" };

		static const double freqsL[] = 
			{	5, 6, 7, 8,
				10, 12, 15, 20, 25, 30, 35, 40, 50, 60, 70, 80,
				100, 120, 150, 200, 250, 300, 350, 400, 500, 600, 700, 800,
				1000, 1200, 1500, 2000, 2500, 3000, 3500, 4000, 5000, 6000, 7000, 8000,
				10000, 12000, 15000, 20000, 30000, 0 };

		static const CString labelsxL[] = 
			{	"5", "6", "7", "8",
				"10", "12", "15", "20", "25", "30", "35", "40", "50", "60", "70", "80",
				"100", "120", "150", "200", "250", "300", "350", "400", "500", "600", "700", "800",
				"1k", "1k2", "1k5", "2k", "2k5", "3k", "3k5", "4k", "5k", "6k", "7k", "8k",
				"10k", "12k", "15k", "20k", "25k", "30k" };
		
		const double *freqs = lowrange ? freqsL : freqsH;
		const CString *labelsx = lowrange ? labelsxL : labelsxH;

		dc.SetTextAlign( TA_NOUPDATECP + TA_CENTER + TA_TOP );

		for( i = 0; freqs[i] != 0; ++i )
		{
			double x = ( log( freqs[i] / m_Samplerate ) - xbase ) / xprop;
			if( x >= 0 && x <= graphwidth ) {
				dc.MoveTo( graphorgx + x, graphorgy );
				dc.LineTo( graphorgx + x, graphorgy + graphheight );
				dc.TextOut( graphorgx + x, graphorgy + graphheight + padding / 2, labelsx[i] );
			}
		}

		dc.SetTextAlign( TA_NOUPDATECP + TA_RIGHT + TA_BOTTOM );
		dc.TextOut( graphorgx + graphwidth, graphorgy + graphheight, "[Hz]" );

		static double decibels[] = { 40, 20, -0.0001, -20, -40, 0 };
		static CString labelsy[] = { "40", "20", "0", "-20", "-40" };
		dc.SetTextAlign( TA_NOUPDATECP + TA_RIGHT + TA_TOP );

		for( i = 0; decibels[i] != 0; ++i )
		{
			double y = graphheight * ( 0.5 - 0.25 * decibels[i] / 20  );
			dc.MoveTo( graphorgx, graphorgy + y );
			dc.LineTo( graphorgx + graphwidth, graphorgy + y );
			dc.TextOut( graphorgx - padding / 2, graphorgy + y - textextent.cy / 2, labelsy[i] );
		}

		dc.SetTextAlign( TA_NOUPDATECP + TA_LEFT + TA_TOP );
		dc.TextOut( graphorgx, graphorgy - textextent.cy / 2, "[db]" );

		rgn.CreateRectRgn( graphorgx, graphorgy, graphorgx + graphwidth, graphorgy + graphheight );
		dc.SelectClipRgn( &rgn );
		dc.SelectObject( &graphpen );

		if( m_CurrentEffect ) 
		{
			float x1 = 0;

			float y1 = 
				graphheight * ( 0.5 - 0.25 * log( volume *
					std::abs( m_CurrentEffect->TransferFunction(
						std::exp( std::complex<float>( 0, pi2 * exp( xbase ) ) )
					) )
				) / log( 10 ) );

			for( float x = 0.25; x < graphwidth; x += 0.25 ) 
			{
				float x2 = x;

				float y2 = 
					graphheight * ( 0.5 - 0.25 * log( volume *
						std::abs( m_CurrentEffect->TransferFunction( 
							std::exp( std::complex<float>( 0, pi2 * exp( xprop * x + xbase ) ) )
						) )
					 ) / log( 10 ) );
		
				dc.MoveTo( graphorgx + x1, graphorgy + y1 );
				dc.LineTo( graphorgx + x2, graphorgy + y2 );
		
				x1 = x2;
				y1 = y2;
			}

		}
		else 
		{
			double y = graphheight * ( 0.5 - 0.25 * log( volume ) / log( 10 ) );
			dc.MoveTo( graphorgx, graphorgy + y );
			dc.LineTo( graphorgx + graphwidth, graphorgy + y );
		}
		
		rgn.DeleteObject();
		font.DeleteObject();
		graphpen.DeleteObject();
		gridpen.DeleteObject();
		background.DeleteObject();

		dc.SelectObject( oldbitmap );
		dc.DeleteDC();
		dcget->DeleteDC();
		bitmap.Detach();
		
		m_TransferGraph.Invalidate( false );
	}
}

/////////////////////////////////////////////////////////////////////////////
// CDSPdemoDlg message handlers

BOOL CDSPdemoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	m_VolumeSlider.SetRange( -90, 30, true );
	m_VolumeSlider.ClearTics();
	m_VolumeSlider.SetTic( -60 );
    m_VolumeSlider.SetTic( -30 );
	m_VolumeSlider.SetTic( -0 );
	m_VolumeSlider.SetPos( 0 );
	m_VolumeEdit.SetWindowText( "0" );
	
	

	CRect rect;
	m_EffectArea.GetWindowRect( rect );
	ScreenToClient( rect );
	m_PlugInFDN4.CreateFromTemplate( this );
	m_PlugInFDN4.SetWindowPos( 0, rect.left, rect.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
	m_PlugInFDN4.ShowWindow( SW_SHOW );
	m_CurrentEffect = &m_PlugInFDN4;
	
	// Load a bitmap into the display area
	// for the transfer-function graph
	CBitmap bitmap;
	m_TransferGraph.GetClientRect( rect );
	bitmap.CreateCompatibleBitmap( GetDC(), rect.Width(), rect.Height() );
	m_TransferGraph.SetBitmap( bitmap );
	bitmap.Detach();

	m_Samplerate = 44100;
	m_SoundOut.SetSamplerate( 44100 );
	m_EditSamplerate.SetWindowText( "44100" );
	
	m_GraphUpdateTimer = SetTimer( 2, 100, 0 );
	m_TransferGraphNeedsUpdate = false;
	
	SCROLLINFO si;
	si.cbSize = sizeof( si );
	si.fMask = SIF_POS | SIF_PAGE | SIF_RANGE;
	si.nMin = -9085;
	si.nMax = -693;
	si.nPage = 6908;
	si.nPos = 0;
	m_ScrollFreq.SetScrollInfo( &si, true );
	
	UpdateTransferGraph();
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CDSPdemoDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CDSPdemoDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDSPdemoDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CDSPdemoDlg::OnAbout() 
{
	CAboutDlg dialog;
	dialog.DoModal();
}

void CDSPdemoDlg::OnDestroy() 
{
	OnStop();
	if( m_GraphUpdateTimer ) KillTimer( m_GraphUpdateTimer );
	CDialog::OnDestroy();
}

void CDSPdemoDlg::OnInputClick() 
{
	CString path;
	m_InputPath.GetWindowText( path );
	CFileDialog dialog( true, 0, path, 0, 0, this );
	dialog.DoModal();
	path = dialog.GetPathName();
	m_InputPath.SetWindowText( path );

	OnKillfocusInputPath();
}


void CDSPdemoDlg::OnPlay() 
{
	m_InputIsStereo = ( m_CheckStereo.GetCheck() != 0 );
	CString text;
	m_EditSamplerate.GetWindowText( text );
	m_Samplerate = atoi( text );
		
	CString path;
	m_InputPath.GetWindowText( path );
	if( m_DspDiskRead.Open( path ) == false ) {
		AfxMessageBox( "Couldn't open the input file." );
		return;
	}

	m_DspDiskRead.Seek( m_DataOffset );

	m_SoundOut.Init( m_hWnd, m_InputIsStereo );
	
	m_TimerID = SetTimer( 1, m_SoundOut.QueryUpdateInterval(), 0 );
	if( m_TimerID == 0 ) {
		AfxMessageBox( "Couldn't install a system timer needed for continuous playback." );
		return;
	}

	// alrighty!
	m_PlayButton.EnableWindow( false );
	m_InputPath.EnableWindow( false );
	m_InputPathButton.EnableWindow( false );
	m_CheckStereo.EnableWindow( false );
	m_StopButton.EnableWindow( true );
	m_SoundOut.SetSamplerate( m_Samplerate );
	m_SoundOut.Play();
	
	if( m_SoundOut.IsPlaying() ) 
	{
		m_Playing = true;
		if( m_CurrentEffect ) m_CurrentEffect->Reset();
	}
	else
		OnStop();
}

void CDSPdemoDlg::OnStop() 
{
	m_SoundOut.Stop();
	m_SoundOut.Release();

	if( m_TimerID ) { KillTimer( m_TimerID ); m_TimerID = 0; }
	m_DspDiskRead.Close();

	m_InputPath.EnableWindow( true );
	m_InputPathButton.EnableWindow( true );
	m_CheckStereo.EnableWindow( true );
	m_PlayButton.EnableWindow( true );
	m_StopButton.EnableWindow( false );
	
	m_Playing = false;
}

void CDSPdemoDlg::OnTimer(UINT nIDEvent) 
{
	switch( nIDEvent ) {
	case 1: {
					
		unsigned long bufsize = m_SoundOut.QueryUpdateSize();
		
		if( m_DspDiskRead.QueryEOF() ) 
			OnStop();
		
		else if( bufsize ) 
		{		
			if( m_InputIsStereo ) 
			{
				cSoundBuffer< float > bufferL( bufsize );
				cSoundBuffer< float > bufferR( bufsize );
				m_DspDiskRead.ProcessStereo( bufferL, bufferR );
				if( m_CurrentEffect ) 
					m_CurrentEffect->ProcessStereo( bufferL, bufferR );
				
				m_Dsp0Pole.SetParameterA( exp( m_VolumeSlider.GetPos() * log( 10 ) / 20 ) );
				m_Dsp0PoleR.SetParameterA( exp( m_VolumeSlider.GetPos() * log( 10 ) / 20 ) );
				m_Dsp0Pole.Process( bufferL );
				m_Dsp0PoleR.Process( bufferR );
			
				m_SoundOut.UpdateStereo( bufferL, bufferR );
			} 
			else 
			{
				cSoundBuffer< float > buffer( bufsize );
				m_DspDiskRead.Process( buffer );					
				if( m_CurrentEffect ) 
					m_CurrentEffect->Process( buffer );

				m_Dsp0Pole.SetParameterA( exp( m_VolumeSlider.GetPos() * log( 10 ) / 20 ) );
				m_Dsp0Pole.Process( buffer );

				m_SoundOut.Update( buffer );
			}
		}

		else if( !m_SoundOut.IsPlaying() ) 
			OnStop();
		
	}
	break;
	case 2:
	{
		if( m_TransferGraphNeedsUpdate ) {
			UpdateTransferGraph();
			m_TransferGraphNeedsUpdate = false;
		}
		else 
			if( m_CurrentEffect ) m_CurrentEffect->OkToPostNotify();
	}
	break;
	default:
		CDialog::OnTimer(nIDEvent);
	}
}

void CDSPdemoDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if( pScrollBar == (CScrollBar *) &m_VolumeSlider ) 
	{
		CString text;
		text.Format( "%d", m_VolumeSlider.GetPos() );
		m_VolumeEdit.SetWindowText( text );
		m_TransferGraphNeedsUpdate = true;
	}
	else if( pScrollBar == (CScrollBar *) &m_ScrollFreq )
	{
		SCROLLINFO si;
		m_ScrollFreq.GetScrollInfo( &si );

		switch( nSBCode ) 
		{
		case SB_THUMBTRACK:
			m_ScrollFreq.SetScrollPos( nPos, false );
			m_TransferGraphNeedsUpdate = true;
			break;
		case  SB_PAGELEFT:
			m_ScrollFreq.SetScrollPos( si.nPos - si.nPage );
			m_TransferGraphNeedsUpdate = true;
			break;
		case  SB_PAGERIGHT:
			m_ScrollFreq.SetScrollPos( si.nPos + si.nPage );
			m_TransferGraphNeedsUpdate = true;
			break;
		}
	} 
	else
		CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CDSPdemoDlg::OnKillfocusVolumeEdit() 
{
	if( m_VolumeEdit.GetWindowTextLength() > 0 ) {
		CString text;
		m_VolumeEdit.GetWindowText( text );
		m_VolumeSlider.SetPos( atoi( text ) );
		text.Format( "%d", m_VolumeSlider.GetPos() );
		m_VolumeEdit.SetWindowText( text );	
	} else
		m_VolumeSlider.SetPos( 0 );	
}



LRESULT CDSPdemoDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	// TODO: Add your specialized code here and/or call the base class

	if( message == WM_USER ) {
		if( wParam == IDC_EFFECTAREA ) {
			if( m_CurrentEffect )
			{
				// && ((NMHDR*)lParam)->hwndFrom == m_CurrentEffect->m_hWnd ) {
				// switch( ((NMHDR*)lParam)->code ) {
				// case (WM_USER + 1)://PLUGIN_PARAMETERCHANGE:
				UpdateTransferGraph();
				return true;
				// }
			}
		}
	}

	if( message == WM_COMMAND && wParam == 1055 )
	{
		int a = 0;
		return TRUE;
	}
		
	return CDialog::WindowProc(message, wParam, lParam);
}

void CDSPdemoDlg::OnKillfocusEditSamplerate() 
{
	CString text;
	m_EditSamplerate.GetWindowText( text );
	if( atoi( text ) != m_Samplerate ) {
		m_Samplerate = atoi( text );
		if( m_Samplerate < 500 ) m_Samplerate = 500;
		if( m_Samplerate > 96000 ) m_Samplerate = 96000;
		text.Format( "%d", m_Samplerate );
		m_EditSamplerate.SetWindowText( text );
		m_TransferGraphNeedsUpdate = true;

		if( m_Playing ) {
			KillTimer( m_TimerID );
			m_SoundOut.Stop();
			m_SoundOut.Release();
			m_SoundOut.SetSamplerate( m_Samplerate );
			m_SoundOut.Init( m_hWnd, m_InputIsStereo );
			m_SoundOut.Play();
			m_TimerID = SetTimer( 1, m_SoundOut.QueryUpdateInterval(), 0 );
		} else
			m_SoundOut.SetSamplerate( m_Samplerate );
	}
}


void CDSPdemoDlg::OnKillfocusInputPath() 
{
	CString path;
	CWaveFile wfile;
	m_InputPath.GetWindowText( path );

	if( path.GetLength() > 0 ) 
	{
		if( wfile.Open( path, CFile::modeRead | CFile::shareDenyNone | CFile::typeBinary, 0 ) ) 
		{
			if( wfile.ReadWaveHeader() ) 
			{
				bool canplay = ( 
					( wfile.GetChannels() == 1 || wfile.GetChannels() == 2 ) && 
					wfile.GetBitsPerSample() == 16
				);

				CString text;
				text.Format( 
					"PCM wave format\r\n"
					"Samplerate %d\r\n"
					"Channels %d\r\n"
					"Bits per Sample %d\r\n"
					"I %s play this this file",
					wfile.GetSamplerate(),
					wfile.GetChannels(),
					wfile.GetBitsPerSample(),
					canplay ? "can" : "can not"
				);

				m_EditProperties.SetWindowText( text );

				if( canplay ) 
				{
					m_CheckStereo.SetCheck( wfile.GetChannels() == 2 );
					m_CheckStereo.EnableWindow( true );
					CString text;
					text.Format( "%d", wfile.GetSamplerate() );
					m_EditSamplerate.SetWindowText( text );
					OnKillfocusEditSamplerate();
					m_EditSamplerate.EnableWindow( true );
					m_PlayButton.EnableWindow( true );
					m_DataOffset = wfile.GetPositionOfDataBody();
				}
				else
				{
					m_CheckStereo.EnableWindow( false );
					m_EditSamplerate.EnableWindow( false );
					m_PlayButton.EnableWindow( false );
				}
			}
			else
			{
				m_EditProperties.SetWindowText(
					"This file has no valid format header.\r\n"
					"If this is a raw 16-bit, signed PCM file, "
					"set the playback rate and whether it "
					"is mono or stereo manually."
				);

				m_CheckStereo.EnableWindow( true );
				m_EditSamplerate.EnableWindow( true );
				m_PlayButton.EnableWindow( true );
				m_DataOffset = 0;
			}
		}
		else
		{
			m_EditProperties.SetWindowText( "The file could not be opened." );
			m_CheckStereo.EnableWindow( false );
			m_EditSamplerate.EnableWindow( false );
			m_PlayButton.EnableWindow( false );
		}
	}
	else
	{
		m_EditProperties.SetWindowText( "The path is empty." );
		m_CheckStereo.EnableWindow( false );
		m_EditSamplerate.EnableWindow( false );
		m_PlayButton.EnableWindow( false );
	}
}


void CDSPdemoDlg::OnCheckLowRange() 
{
	bool zoomed = m_CheckLowRange.GetCheck() != 0;

	SCROLLINFO si;
	m_ScrollFreq.GetScrollInfo( &si );
	int midpos = si.nPos + si.nPage / 2;
	si.cbSize = sizeof( si );
	si.fMask = SIF_POS | SIF_PAGE;
	si.nPage = zoomed ? 694 : 6908;
	si.nPos = midpos - si.nPage / 2;
	m_ScrollFreq.SetScrollInfo( &si, true );
	UpdateTransferGraph();
}

void CDSPdemoDlg::OnReset() 
{
	// TODO: Add your control notification handler code here
	if( m_CurrentEffect ) m_CurrentEffect->Reset();
}
