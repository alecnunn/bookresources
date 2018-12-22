//-----------------------------------------------------------------------------
// File: DPlayConnect_CreateForm.cs
//
// Desc: Application class for the DirectPlay samples framework.
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
using System;
using System.Collections;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectPlay;




/// <summary>
/// This form will allow you to create a new session, and set certain properties
/// of the session.
/// </summary>
public class CreateSessionForm : System.Windows.Forms.Form {
	private Peer peer;
	private ConnectWizard connectionWizard;
	private Address deviceAddress;

	private System.Windows.Forms.Button btnOK;
	private System.Windows.Forms.TextBox txtSession;
	private System.Windows.Forms.Label label1;
	private System.Windows.Forms.GroupBox groupBox1;
	private System.Windows.Forms.RadioButton notSignedRadio;
	private System.Windows.Forms.RadioButton fullSignedRadio;
	private System.Windows.Forms.RadioButton fastSignedRadio;
	private System.Windows.Forms.CheckBox migrateHostCheckBox;
	private System.Windows.Forms.CheckBox useDPNSVRCheckBox;
	private System.Windows.Forms.TextBox localPortTextBox;
	private System.Windows.Forms.Label localPortLabel;
	private System.Windows.Forms.Button btnCancel;



	/// <summary>
	/// Constructor
	/// </summary>
	public CreateSessionForm(Peer peerObject, Address addressObject, ConnectWizard connectionWizard) {
		//
		// Required for Windows Form Designer support
		//
		InitializeComponent();

		peer = peerObject;
		this.connectionWizard = connectionWizard;
		deviceAddress = addressObject;
		txtSession.Text = null;
		this.Text = connectionWizard.SampleName + " - " + this.Text;
		//Get the default session from the registry if it exists
		Microsoft.Win32.RegistryKey regKey = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("Software\\Microsoft\\DirectX\\SDK\\csDPlay");
		if (regKey != null) {
			// Get session name
			txtSession.Text = (string)regKey.GetValue("DirectPlaySessionName", null);
        
			// Get host migration option
			if (regKey.GetValue("DirectPlayMigrateHost",null) != null) {
				migrateHostCheckBox.Checked = ((int)regKey.GetValue("DirectPlayMigrateHost",1)==1);
			}
        
        
			// Get session signing option
			if (regKey.GetValue("DirectPlaySessionSigning",null) != null) {
				if ("Full" == (string)regKey.GetValue("DirectPlaySessionSigning",null))
					fullSignedRadio.Checked = true;
				else if ("Fast" == (string)regKey.GetValue("DirectPlaySessionSigning",null))
					fastSignedRadio.Checked = true;
				else
					notSignedRadio.Checked = true;
			}

			regKey.Close();
		}

		// Set default port value and hide port UI if provider doesn't use them
		Port = connectionWizard.DefaultPort;
		if (!ConnectWizard.ProviderRequiresPort(deviceAddress.ServiceProvider)) {
			localPortTextBox.Hide();
			localPortLabel.Hide();
		}
	}



	/// <summary>
	/// The port on which to host
	/// </summary>
	public int Port {
		get {
			int port = 0;

			try { 
				port = int.Parse(localPortTextBox.Text); 
			} 
			catch {}

			return port;
		}
		set { 
			if( value > 0 )
				localPortTextBox.Text = value.ToString(); 
		}
	}




	/// <summary>
	/// Clean up any resources being used.
	/// </summary>
	protected override void Dispose(bool Disposing) {
		base.Dispose(Disposing);
	}




	#region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	private void InitializeComponent() {
		this.txtSession = new System.Windows.Forms.TextBox();
		this.label1 = new System.Windows.Forms.Label();
		this.btnOK = new System.Windows.Forms.Button();
		this.btnCancel = new System.Windows.Forms.Button();
		this.groupBox1 = new System.Windows.Forms.GroupBox();
		this.fullSignedRadio = new System.Windows.Forms.RadioButton();
		this.fastSignedRadio = new System.Windows.Forms.RadioButton();
		this.notSignedRadio = new System.Windows.Forms.RadioButton();
		this.migrateHostCheckBox = new System.Windows.Forms.CheckBox();
		this.useDPNSVRCheckBox = new System.Windows.Forms.CheckBox();
		this.localPortLabel = new System.Windows.Forms.Label();
		this.localPortTextBox = new System.Windows.Forms.TextBox();
		this.groupBox1.SuspendLayout();
		this.SuspendLayout();
		// 
		// txtSession
		// 
		this.txtSession.Location = new System.Drawing.Point(96, 24);
		this.txtSession.Name = "txtSession";
		this.txtSession.Size = new System.Drawing.Size(176, 20);
		this.txtSession.TabIndex = 3;
		this.txtSession.Text = "SpaceWar";
		// 
		// label1
		// 
		this.label1.Location = new System.Drawing.Point(16, 28);
		this.label1.Name = "label1";
		this.label1.Size = new System.Drawing.Size(288, 12);
		this.label1.TabIndex = 2;
		this.label1.Text = "Session Name:";
		// 
		// btnOK
		// 
		this.btnOK.Location = new System.Drawing.Point(128, 168);
		this.btnOK.Name = "btnOK";
		this.btnOK.Size = new System.Drawing.Size(74, 27);
		this.btnOK.TabIndex = 0;
		this.btnOK.Text = "OK";
		this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
		// 
		// btnCancel
		// 
		this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
		this.btnCancel.Location = new System.Drawing.Point(208, 168);
		this.btnCancel.Name = "btnCancel";
		this.btnCancel.Size = new System.Drawing.Size(74, 27);
		this.btnCancel.TabIndex = 1;
		this.btnCancel.Text = "Cancel";
		// 
		// groupBox1
		// 
		this.groupBox1.Controls.Add(this.fullSignedRadio);
		this.groupBox1.Controls.Add(this.fastSignedRadio);
		this.groupBox1.Controls.Add(this.notSignedRadio);
		this.groupBox1.Location = new System.Drawing.Point(16, 64);
		this.groupBox1.Name = "groupBox1";
		this.groupBox1.Size = new System.Drawing.Size(112, 88);
		this.groupBox1.TabIndex = 10;
		this.groupBox1.TabStop = false;
		this.groupBox1.Text = "Session Signing";
		this.groupBox1.Visible = false;
		// 
		// fullSignedRadio
		// 
		this.fullSignedRadio.Location = new System.Drawing.Point(18, 22);
		this.fullSignedRadio.Name = "fullSignedRadio";
		this.fullSignedRadio.Size = new System.Drawing.Size(80, 16);
		this.fullSignedRadio.TabIndex = 9;
		this.fullSignedRadio.Text = "Full signed";
		// 
		// fastSignedRadio
		// 
		this.fastSignedRadio.Checked = true;
		this.fastSignedRadio.Location = new System.Drawing.Point(18, 41);
		this.fastSignedRadio.Name = "fastSignedRadio";
		this.fastSignedRadio.Size = new System.Drawing.Size(88, 16);
		this.fastSignedRadio.TabIndex = 8;
		this.fastSignedRadio.TabStop = true;
		this.fastSignedRadio.Text = "Fast signed";
		// 
		// notSignedRadio
		// 
		this.notSignedRadio.Location = new System.Drawing.Point(18, 55);
		this.notSignedRadio.Name = "notSignedRadio";
		this.notSignedRadio.Size = new System.Drawing.Size(72, 24);
		this.notSignedRadio.TabIndex = 10;
		this.notSignedRadio.Text = "Disabled";
		// 
		// migrateHostCheckBox
		// 
		this.migrateHostCheckBox.Location = new System.Drawing.Point(144, 64);
		this.migrateHostCheckBox.Name = "migrateHostCheckBox";
		this.migrateHostCheckBox.Size = new System.Drawing.Size(136, 24);
		this.migrateHostCheckBox.TabIndex = 11;
		this.migrateHostCheckBox.Text = "Enable Host Migration";
		this.migrateHostCheckBox.Visible = false;
		// 
		// useDPNSVRCheckBox
		// 
		this.useDPNSVRCheckBox.Location = new System.Drawing.Point(144, 84);
		this.useDPNSVRCheckBox.Name = "useDPNSVRCheckBox";
		this.useDPNSVRCheckBox.Size = new System.Drawing.Size(136, 24);
		this.useDPNSVRCheckBox.TabIndex = 12;
		this.useDPNSVRCheckBox.Text = "Use DPNSVR";
		this.useDPNSVRCheckBox.Visible = false;
		// 
		// localPortLabel
		// 
		this.localPortLabel.Location = new System.Drawing.Point(152, 122);
		this.localPortLabel.Name = "localPortLabel";
		this.localPortLabel.Size = new System.Drawing.Size(64, 17);
		this.localPortLabel.TabIndex = 13;
		this.localPortLabel.Text = "Local Port:";
		// 
		// localPortTextBox
		// 
		this.localPortTextBox.Location = new System.Drawing.Point(216, 120);
		this.localPortTextBox.Name = "localPortTextBox";
		this.localPortTextBox.Size = new System.Drawing.Size(56, 20);
		this.localPortTextBox.TabIndex = 14;
		this.localPortTextBox.Text = "2580";
		// 
		// CreateSessionForm
		// 
		this.AcceptButton = this.btnOK;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		this.CancelButton = this.btnCancel;
		this.ClientSize = new System.Drawing.Size(290, 200);
		this.ControlBox = false;
		this.Controls.Add(this.localPortTextBox);
		this.Controls.Add(this.localPortLabel);
		this.Controls.Add(this.useDPNSVRCheckBox);
		this.Controls.Add(this.migrateHostCheckBox);
		this.Controls.Add(this.txtSession);
		this.Controls.Add(this.label1);
		this.Controls.Add(this.groupBox1);
		this.Controls.Add(this.btnOK);
		this.Controls.Add(this.btnCancel);
		this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		this.HelpButton = true;
		this.MaximizeBox = false;
		this.MinimizeBox = false;
		this.Name = "CreateSessionForm";
		this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
		this.Text = "Create a Session";
		this.HelpRequested += new System.Windows.Forms.HelpEventHandler(this.CreateSessionForm_HelpRequested);
		this.groupBox1.ResumeLayout(false);
		this.ResumeLayout(false);

	}
	#endregion




	/// <summary>
	/// We are ready to create a session.  Ensure the data is valid
	/// then create the session
	/// </summary>
	private void btnOK_Click(object sender, System.EventArgs e) {
		ApplicationDescription dpApp;
		if ((txtSession.Text == null) || (txtSession.Text == "")) {
			MessageBox.Show(this,"Please enter a session name before clicking OK.","No sessionname",MessageBoxButtons.OK,MessageBoxIcon.Information);
			return;
		}

		Microsoft.Win32.RegistryKey regKey = Microsoft.Win32.Registry.CurrentUser.CreateSubKey("Software\\Microsoft\\DirectX\\SDK\\csDPlay");
		if (regKey != null) {
			regKey.SetValue("DirectPlaySessionName", txtSession.Text);
			if (migrateHostCheckBox.Checked)
				regKey.SetValue("DirectPlayMigrateHost",1);
			else
				regKey.SetValue("DirectPlayMigrateHost",0);

			if (fastSignedRadio.Checked)
				regKey.SetValue("DirectPlaySessionSigning","Fast");
			else if (fullSignedRadio.Checked)
				regKey.SetValue("DirectPlaySessionSigning","Full");
			else
				regKey.SetValue("DirectPlaySessionSigning","Disabled");
			regKey.Close();
		}

		dpApp = new ApplicationDescription();
		dpApp.GuidApplication = connectionWizard.ApplicationGuid;
		dpApp.SessionName = txtSession.Text;
		dpApp.Flags = 0;
    
		if (migrateHostCheckBox.Checked)
			dpApp.Flags |= SessionFlags.MigrateHost;

		if (!useDPNSVRCheckBox.Checked)
			dpApp.Flags |= SessionFlags.NoDpnServer;

		if (fastSignedRadio.Checked)
			dpApp.Flags |= SessionFlags.FastSigned;
		else if (fullSignedRadio.Checked)
			dpApp.Flags |= SessionFlags.FullSigned;

		// Specify the port number if available
		if (ConnectWizard.ProviderRequiresPort(deviceAddress.ServiceProvider)) {
			if (Port > 0)
				deviceAddress.AddComponent(Address.KeyPort, Port);
		}

		connectionWizard.SetUserInfo();
		// Host a game on deviceAddress as described by dpApp
		// HostFlags.OkToQueryForAddressing allows DirectPlay to prompt the user
		// using a dialog box for any device address information that is missing
		peer.Host(dpApp,deviceAddress,HostFlags.OkToQueryForAddressing);
		this.DialogResult = DialogResult.OK;
	}

	private void CreateSessionForm_HelpRequested(object sender, System.Windows.Forms.HelpEventArgs hlpevent) {
        
	}
}
