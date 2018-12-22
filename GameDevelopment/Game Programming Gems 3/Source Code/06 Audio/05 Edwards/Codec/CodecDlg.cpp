// CodecDlg.cpp
//
// main CDialog window

#include "stdafx.h"
#include "Codec.h"
#include "CodecDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool init = false;	// avoid some MFC initialization bugs

static void	CALLBACK EXPORT TimerCallback(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime);

CCodecDlg::CCodecDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCodecDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCodecDlg)
	//}}AFX_DATA_INIT

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCodecDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCodecDlg)
	DDX_Control(pDX, IDC_RATE_SLD, m_Rate);
	DDX_Control(pDX, IDC_DUMP, m_DumpBtn);
	DDX_Control(pDX, IDC_OSCA_LEVEL_SLD, m_OscaLevel);
	DDX_Control(pDX, IDC_OSCA_ENV_SLD, m_OscaEnv);
	DDX_Control(pDX, IDC_OSCF_VOICE_SLD, m_OscfVoice);
	DDX_Control(pDX, IDC_OSCF_MOD_SLD, m_OscfMod);
	DDX_Control(pDX, IDC_OSCF_CONST_SLD, m_OscfConst);
	DDX_Control(pDX, IDC_SAVE, m_SaveBtn);
	DDX_Control(pDX, IDC_CLEAR, m_ClearBtn);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCodecDlg, CDialog)
	//{{AFX_MSG_MAP(CCodecDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_MODULATOR, OnModulator)
	ON_BN_CLICKED(IDC_CARRIER, OnCarrier)
	ON_BN_CLICKED(IDC_OSC_SAWTOOTH, OnOscSawtooth)
	ON_BN_CLICKED(IDC_OSC_SINE, OnOscSine)
	ON_BN_CLICKED(IDC_OSC_SQUARE, OnOscSquare)
	ON_BN_CLICKED(IDC_OSC_TRI, OnOscTri)
	ON_BN_CLICKED(IDC_OSCF_RATIO_SW, OnOscfRatioSw)
	ON_EN_CHANGE(IDC_OSCF_RAT1, OnChangeOscfRat1)
	ON_EN_CHANGE(IDC_OSCF_RAT2, OnChangeOscfRat2)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_BN_CLICKED(IDC_OUT_FMMOD, OnOutFmmod)
	ON_BN_CLICKED(IDC_OUT_FMOUT, OnOutFmout)
	ON_BN_CLICKED(IDC_OUT_SPEECH, OnOutSpeech)
	ON_BN_CLICKED(IDC_SAVE_SPK, OnSaveSpk)
	ON_BN_CLICKED(IDC_BROWSE_SPK, OnBrowseSpk)
	ON_BN_CLICKED(IDC_DUMP, OnDump)
	ON_BN_CLICKED(IDC_DUMP_SPK, OnDumpSpk)
	ON_BN_CLICKED(IDC_RATE_SW, OnRateSw)
	ON_BN_CLICKED(IDC_DEEP_SW, OnDeepSw)
	ON_BN_CLICKED(IDC_OSC_PULSE, OnOscPulse)
	ON_BN_CLICKED(IDC_OSC_NOISE, OnOscNoise)
	ON_BN_CLICKED(IDC_RESIDUAL_SW, OnResidualSw)
	ON_BN_CLICKED(IDC_SHALLOW_SW, OnShallowSw)
	ON_WM_SYSCOMMAND()
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// initialize dialog

BOOL CCodecDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	soundmanager = new SoundManager(m_hWnd);

	strcpy(filename, "");
	strcpy(spkfilename, "");
	SetDlgItemText(IDC_FILENAME, "(none)");
	m_SaveBtn.EnableWindow(FALSE);
	m_DumpBtn.EnableWindow(FALSE);
	m_ClearBtn.EnableWindow(FALSE);

	speaker = new Speaker;
	player = new QVOX_Client();
	qvox = new QVOX();
	edit_channel = 1;
	rate = 50;
	m_Rate.SetPos(rate);
	ShowFMParms();
	soundmanager->AttachClient(player);
	player->Play(qvox, speaker, rate);

	SendDlgItemMessage(IDC_OUT_SPEECH,	BM_SETCHECK, BST_CHECKED);
	SendDlgItemMessage(IDC_RESIDUAL_SW,	BM_SETCHECK, BST_CHECKED);
	SetDlgItemText(IDC_OUT_FMOUT, "Residual");
	SetDlgItemText(IDC_OUT_FMMOD, "Residual");

	SetTimer(667, 20, TimerCallback);

	// Set the icon for this dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	init = true;

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CCodecDlg::OnPaint() 
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

HCURSOR CCodecDlg::OnQueryDragIcon()
{
	return (HCURSOR)m_hIcon;
}

// browse for a WAV or QVX file to load

void CCodecDlg::OnBrowse() 
{
	OPENFILENAME FileStrc;

	FileStrc.lStructSize		= sizeof(FileStrc);
	FileStrc.hwndOwner			= m_hWnd;
	FileStrc.lpstrFilter		= "Speech Files\0*.wav;*.qvx\0\0";
	FileStrc.lpstrCustomFilter	= NULL;
	FileStrc.nFilterIndex		= 1;
	FileStrc.lpstrFile			= filename;
	FileStrc.nMaxFile			= 512;
	FileStrc.lpstrFileTitle		= NULL;
	FileStrc.nMaxFileTitle		= 512; 
	FileStrc.lpstrInitialDir	= NULL;
	FileStrc.lpstrTitle			= "Choose WAV or QVX file to play";
	FileStrc.Flags				= OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER;
	FileStrc.lpstrDefExt		= "wav";
						
	if (GetOpenFileName(&FileStrc))
	{
		if (filename[0])
		{
			int	length;

			length = strlen(filename);
			if (!stricmp(filename + length - 4, ".wav"))
			{
				WavFile		wav;

				SetDlgItemText(IDC_FILENAME, filename);
				soundmanager->Pause();

				// load WAV file and convert to 8KHz
				wav.Load(filename);
				if (wav.GetRate() != 8000)
				{
					SetDlgItemText(IDC_STATUS, "Resampling ... please wait");
					SendDlgItemMessage(IDC_STATUS, WM_PAINT, 0);
					wav.SetRate(8000, 32);
					SetDlgItemText(IDC_STATUS, "");
				}

				// free old QVOX
				// create QVOX object with the file
				// attach to player
				player->Stop();
				delete qvox;
				if (wav.IsValid())
				{
					SetDlgItemText(IDC_STATUS, "Compressing ... please wait");
					SendDlgItemMessage(IDC_STATUS, WM_PAINT, 0);
					qvox = new QVOX(&wav);
					m_Rate.SetPos(rate = 50);
					player->Play(qvox, speaker);
					m_SaveBtn.EnableWindow(TRUE);
					m_DumpBtn.EnableWindow(TRUE);
					m_ClearBtn.EnableWindow(TRUE);
				}
				else
				{
					qvox = new QVOX();
					m_Rate.SetPos(rate = 50);
					player->Play(qvox, speaker);
					m_SaveBtn.EnableWindow(FALSE);
					m_DumpBtn.EnableWindow(FALSE);
					m_ClearBtn.EnableWindow(FALSE);
				}

				soundmanager->Resume();
				SetDlgItemText(IDC_STATUS, "");
			}
			else if (!stricmp(filename + length - 4, ".qvx"))
			{
				SetDlgItemText(IDC_FILENAME, filename);
				soundmanager->Pause();

				player->Stop();
				delete qvox;
				qvox = new QVOX(filename);
				m_Rate.SetPos(rate = 50);
				player->Play(qvox, speaker);
				m_SaveBtn.EnableWindow(TRUE);
				m_DumpBtn.EnableWindow(TRUE);
				m_ClearBtn.EnableWindow(TRUE);

				player->play_residual = false;
				SendDlgItemMessage(IDC_RESIDUAL_SW,	BM_SETCHECK, BST_UNCHECKED);
				SetDlgItemText(IDC_OUT_FMOUT, "FM synth output");
				SetDlgItemText(IDC_OUT_FMMOD, "FM modulator");

				soundmanager->Resume();
			}
			else
			{
				SetDlgItemText(IDC_FILENAME, "(none)");
				soundmanager->Pause();
				player->Stop();
				delete qvox;
				qvox = new QVOX();
				m_Rate.SetPos(rate = 50);
				player->Play(qvox, speaker);
				m_SaveBtn.EnableWindow(FALSE);
				m_DumpBtn.EnableWindow(FALSE);
				m_ClearBtn.EnableWindow(FALSE);
				soundmanager->Resume();
			}
		}
	}
}

// save a QVX file

void CCodecDlg::OnSave()
{
	OPENFILENAME	FileStrc;
	int				length;

	length = strlen(filename);
	if (filename[length - 4] == '.')
	{
		strcpy(filename + length - 4, ".qvx");
	}

	FileStrc.lStructSize		= sizeof(FileStrc);
	FileStrc.hwndOwner			= m_hWnd;
	FileStrc.lpstrFilter		= "Speech Files\0*.qvx\0\0";
	FileStrc.lpstrCustomFilter	= NULL;
	FileStrc.nFilterIndex		= 1;
	FileStrc.lpstrFile			= filename;
	FileStrc.nMaxFile			= 512;
	FileStrc.lpstrFileTitle		= NULL;
	FileStrc.nMaxFileTitle		= 512; 
	FileStrc.lpstrInitialDir	= NULL;
	FileStrc.lpstrTitle			= "Save As";
	FileStrc.Flags				= OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT;
	FileStrc.lpstrDefExt		= "qvx";

//	FileStrc.hInstance = AfxGetApp()->m_hInstance;
//	FileStrc.lpTemplateName = MAKEINTRESOURCE(IDD_TEMPLATE);
//	FileStrc.Flags |= OFN_ENABLETEMPLATE;
						
	if (GetSaveFileName(&FileStrc))
	{
		if (filename[0])
		{
			SetDlgItemText(IDC_FILENAME, filename);
			qvox->Save(filename);
		}
	}
}

// dump a TXT file

void CCodecDlg::OnDump()
{
	OPENFILENAME	FileStrc;
	int				length;
	char			fname[512];

	strcpy(fname, filename);
	length = strlen(fname);
	if (fname[length-4] == '.')
	{
		strcpy(fname + length - 4, ".txt");
	}

	FileStrc.lStructSize		= sizeof(FileStrc);
	FileStrc.hwndOwner			= m_hWnd;
	FileStrc.lpstrFilter		= "Dump Files\0*.txt\0\0";
	FileStrc.lpstrCustomFilter	= NULL;
	FileStrc.nFilterIndex		= 1;
	FileStrc.lpstrFile			= fname;
	FileStrc.nMaxFile			= 512;
	FileStrc.lpstrFileTitle		= NULL;
	FileStrc.nMaxFileTitle		= 512; 
	FileStrc.lpstrInitialDir	= NULL;
	FileStrc.lpstrTitle			= "Save As";
	FileStrc.Flags				= OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT;
	FileStrc.lpstrDefExt		= "txt";
						
	if (GetSaveFileName(&FileStrc))
	{
		if (fname[0])
		{
			qvox->Dump(fname);
		}
	}
}

// clear loaded file

void CCodecDlg::OnClear() 
{
	SetDlgItemText(IDC_FILENAME, "(none)");
	soundmanager->Pause();
	player->Stop();
	delete qvox;
	qvox = new QVOX();
	m_Rate.SetPos(rate = 50);
	player->Play(qvox, speaker);
	m_SaveBtn.EnableWindow(FALSE);
	m_DumpBtn.EnableWindow(FALSE);
	m_ClearBtn.EnableWindow(FALSE);
	soundmanager->Resume();
}

// select modulator/carrier parameters

void CCodecDlg::OnModulator() 
{
	StoreFMParms();
	edit_channel = 0;
	ShowFMParms();
}

void CCodecDlg::OnCarrier() 
{
	StoreFMParms();
	edit_channel = 1;
	ShowFMParms();
}

// select oscillator

void CCodecDlg::OnOscSine() 
{
	edit_osc->wave = Sine;	
}

void CCodecDlg::OnOscTri() 
{
	edit_osc->wave = Tri;
}

void CCodecDlg::OnOscSawtooth() 
{
	edit_osc->wave = Saw;
}

void CCodecDlg::OnOscSquare() 
{
	edit_osc->wave = Square;
}

void CCodecDlg::OnOscPulse() 
{
	edit_osc->wave = Pulse;	
}

void CCodecDlg::OnOscNoise() 
{
	edit_osc->wave = Noise;	
}

// oscillator ratio

void CCodecDlg::OnOscfRatioSw() 
{
	edit_osc->f_ratio_f = !edit_osc->f_ratio_f;
}

void CCodecDlg::OnChangeOscfRat1() 
{
	if (!init)	return;

	char	temp[3];

	GetDlgItemText(IDC_OSCF_RAT1, temp, 3);
	edit_osc->f_ratio_m = atoi(temp);
	if (edit_osc->f_ratio_m < 1)
	{
		edit_osc->f_ratio_m = 1;
		strcpy(temp, "1");
		SetDlgItemText(IDC_OSCF_RAT1, temp);
	}
	else if (edit_osc->f_ratio_m > 16)
	{
		edit_osc->f_ratio_m = 16;
		strcpy(temp, "16");
		SetDlgItemText(IDC_OSCF_RAT1, temp);
	}
}

void CCodecDlg::OnChangeOscfRat2() 
{
	if (!init)	return;

	char	temp[3];

	GetDlgItemText(IDC_OSCF_RAT2, temp, 3);
	edit_osc->f_ratio_d = atoi(temp);
	if (edit_osc->f_ratio_d < 1)
	{
		edit_osc->f_ratio_d = 1;
		strcpy(temp, "1");
		SetDlgItemText(IDC_OSCF_RAT2, temp);
	}
	else if (edit_osc->f_ratio_d > 16)
	{
		edit_osc->f_ratio_d = 16;
		strcpy(temp, "16");
		SetDlgItemText(IDC_OSCF_RAT2, temp);
	}
}

// store parameters from dialog

void CCodecDlg::StoreFMParms()
{
	edit_osc->f_const		= m_OscfConst.GetPos();
	edit_osc->f_voice		= m_OscfVoice.GetPos();
	edit_osc->f_mod			= m_OscfMod.GetPos();
	edit_osc->a_level		= m_OscaLevel.GetPos();
	edit_osc->a_envelope	= m_OscaEnv.GetPos();
}

// timer callback - periodically store parameters

static void	CALLBACK EXPORT TimerCallback(HWND hWnd, UINT nMsg, UINT nIDEvent, DWORD dwTime)
{
	((CCodecDlg *)CWnd::FromHandle(hWnd))->StoreFMParms();
}

// show parameters in dialog

void CCodecDlg::ShowFMParms()
{
	char	temp[3];

	edit_osc = &speaker->osc[edit_channel];

	SendDlgItemMessage(IDC_OSC_SINE,	BM_SETCHECK, edit_osc->wave == Sine		? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_OSC_TRI,		BM_SETCHECK, edit_osc->wave == Tri		? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_OSC_SAWTOOTH,BM_SETCHECK, edit_osc->wave == Saw		? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_OSC_SQUARE,	BM_SETCHECK, edit_osc->wave == Square	? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_OSC_PULSE,	BM_SETCHECK, edit_osc->wave == Pulse	? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_OSC_NOISE,	BM_SETCHECK, edit_osc->wave == Noise	? BST_CHECKED : BST_UNCHECKED);

	SendDlgItemMessage(IDC_OSCF_RATIO_SW, BM_SETCHECK, edit_osc->f_ratio_f ? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_DEEP_SW, BM_SETCHECK, speaker->deep ? BST_CHECKED : BST_UNCHECKED);
	SendDlgItemMessage(IDC_SHALLOW_SW, BM_SETCHECK, speaker->shallow ? BST_CHECKED : BST_UNCHECKED);

	sprintf(temp, "%d", edit_osc->f_ratio_m);
	SetDlgItemText(IDC_OSCF_RAT1, temp);

	sprintf(temp, "%d", edit_osc->f_ratio_d);
	SetDlgItemText(IDC_OSCF_RAT2, temp);

	m_OscfConst.SetPos(edit_osc->f_const);
	m_OscfVoice.SetPos(edit_osc->f_voice);
	m_OscfMod.SetPos(edit_osc->f_mod);
	m_OscaLevel.SetPos(edit_osc->a_level);
	m_OscaEnv.SetPos(edit_osc->a_envelope);

	if (edit_channel == 0)
	{
		SetDlgItemText(IDC_MODULATION, "Feedback");
		SendDlgItemMessage(IDC_MODULATOR, BM_SETCHECK, BST_CHECKED);
		SendDlgItemMessage(IDC_CARRIER, BM_SETCHECK, BST_UNCHECKED);
	}
	else
	{
		SetDlgItemText(IDC_MODULATION, "Modulation");
		SendDlgItemMessage(IDC_MODULATOR, BM_SETCHECK, BST_UNCHECKED);
		SendDlgItemMessage(IDC_CARRIER, BM_SETCHECK, BST_CHECKED);
	}
}

// set output mode

void CCodecDlg::OnOutFmmod() 
{
	player->testmode = 2;	
}

void CCodecDlg::OnOutFmout() 
{
	player->testmode = 1;
}

void CCodecDlg::OnOutSpeech() 
{
	player->testmode = 0;	
}

// save SPK file

void CCodecDlg::OnSaveSpk() 
{
	OPENFILENAME FileStrc;

	FileStrc.lStructSize		= sizeof(FileStrc);
	FileStrc.hwndOwner			= m_hWnd;
	FileStrc.lpstrFilter		= "SPK Files\0*.spk\0\0";
	FileStrc.lpstrCustomFilter	= NULL;
	FileStrc.nFilterIndex		= 1;
	FileStrc.lpstrFile			= spkfilename;
	FileStrc.nMaxFile			= 512;
	FileStrc.lpstrFileTitle		= NULL;
	FileStrc.nMaxFileTitle		= 512; 
	FileStrc.lpstrInitialDir	= NULL;
	FileStrc.lpstrTitle			= "Save As";
	FileStrc.Flags				= OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT;
	FileStrc.lpstrDefExt		= "spk";
						
	if (GetSaveFileName(&FileStrc))
	{
		if (spkfilename[0])
		{
			speaker->Save(spkfilename);
		}
	}
}

// load SPK file

void CCodecDlg::OnBrowseSpk() 
{
	OPENFILENAME FileStrc;

	FileStrc.lStructSize		= sizeof(FileStrc);
	FileStrc.hwndOwner			= m_hWnd;
	FileStrc.lpstrFilter		= "SPK Files\0*.spk\0\0";
	FileStrc.lpstrCustomFilter	= NULL;
	FileStrc.nFilterIndex		= 1;
	FileStrc.lpstrFile			= spkfilename;
	FileStrc.nMaxFile			= 512;
	FileStrc.lpstrFileTitle		= NULL;
	FileStrc.nMaxFileTitle		= 512; 
	FileStrc.lpstrInitialDir	= NULL;
	FileStrc.lpstrTitle			= "Choose SPK file to use";
	FileStrc.Flags				= OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_EXPLORER;
	FileStrc.lpstrDefExt		= "spk";
						
	if (GetOpenFileName(&FileStrc))
	{
		if (spkfilename[0])
		{
			speaker->Load(spkfilename);
			soundmanager->Pause();	
			player->Stop();
			player->Play(qvox, speaker, rate);
			soundmanager->Resume();
			ShowFMParms();			
		}
	}
}

// dump speaker to TXT file

void CCodecDlg::OnDumpSpk()
{
	OPENFILENAME	FileStrc;
	int				length;
	char			fname[512];

	strcpy(fname, spkfilename);
	length = strlen(fname);
	if (fname[length-4] == '.')
	{
		strcpy(fname + length - 4, ".txt");
	}

	FileStrc.lStructSize		= sizeof(FileStrc);
	FileStrc.hwndOwner			= m_hWnd;
	FileStrc.lpstrFilter		= "Dump Files\0*.txt\0\0";
	FileStrc.lpstrCustomFilter	= NULL;
	FileStrc.nFilterIndex		= 1;
	FileStrc.lpstrFile			= fname;
	FileStrc.nMaxFile			= 512;
	FileStrc.lpstrFileTitle		= NULL;
	FileStrc.nMaxFileTitle		= 512; 
	FileStrc.lpstrInitialDir	= NULL;
	FileStrc.lpstrTitle			= "Save As";
	FileStrc.Flags				= OFN_HIDEREADONLY|OFN_EXPLORER|OFN_OVERWRITEPROMPT;
	FileStrc.lpstrDefExt		= "txt";
						
	if (GetSaveFileName(&FileStrc))
	{
		if (fname[0])
		{
			speaker->Dump(fname);
		}
	}
}

// set rate

void CCodecDlg::OnRateSw() 
{
	rate = m_Rate.GetPos();
	soundmanager->Pause();
	player->Stop();
	player->Play(qvox, speaker, rate);
	soundmanager->Resume();
}

// set deep filter

void CCodecDlg::OnDeepSw() 
{
	speaker->deep = !speaker->deep;
	soundmanager->Pause();	
	player->Stop();
	player->Play(qvox, speaker, rate);
	soundmanager->Resume();
}

// set shallow filter

void CCodecDlg::OnShallowSw() 
{
	speaker->shallow = !speaker->shallow;
	soundmanager->Pause();	
	player->Stop();
	player->Play(qvox, speaker, rate);
	soundmanager->Resume();
}

// toggle residual output

void CCodecDlg::OnResidualSw() 
{
	player->play_residual = !player->play_residual;		
	if (!player->play_residual)
	{
		SetDlgItemText(IDC_OUT_FMOUT, "FM synth output");
		SetDlgItemText(IDC_OUT_FMMOD, "FM modulator");
	}
	else
	{
		SetDlgItemText(IDC_OUT_FMOUT, "Residual");
		SetDlgItemText(IDC_OUT_FMMOD, "Residual");
	}
}
