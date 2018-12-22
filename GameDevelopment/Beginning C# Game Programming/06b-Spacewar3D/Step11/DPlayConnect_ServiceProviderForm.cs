//-----------------------------------------------------------------------------
// File: DPlayConnect_ServiceProviderForm.cs
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
/// This form allows you to choose a service provider and username for your 
/// sample
/// </summary>
public class ChooseServiceProviderForm : System.Windows.Forms.Form
{
    private Peer peer;
    private ConnectWizard connectionWizard;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.ListBox lstSP;
    private System.Windows.Forms.Button btnOK;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.TextBox txtUser;
    private System.Windows.Forms.Button btnCancel;




    /// <summary>
    /// Constructor
    /// </summary>
    public ChooseServiceProviderForm(Peer peerObject, ConnectWizard connectionWizard)
    {
        //
        // Required for Windows Form Designer support
        //
        InitializeComponent();
        peer = peerObject;
        this.connectionWizard = connectionWizard;
        this.Text = connectionWizard.SampleName + " - " + this.Text;
        // Fill up our listbox with the service providers
        ServiceProviderInformation[] serviceProviders = peer.GetServiceProviders(false);
        foreach (ServiceProviderInformation info in serviceProviders)
            lstSP.Items.Add(info);

        txtUser.Text = null;
        //Get the default username from the registry if it exists
        Microsoft.Win32.RegistryKey regKey = Microsoft.Win32.Registry.CurrentUser.OpenSubKey("Software\\Microsoft\\DirectX\\SDK\\csDPlay");
        if (regKey != null)
        {
            try
            {
                txtUser.Text = (string)regKey.GetValue("DirectPlayUserName", null);
                lstSP.SelectedIndex = (int)regKey.GetValue("DirectPlaySPIndex", 0);
                regKey.Close();
            }
            catch
            {
                txtUser.Text = null;
                lstSP.SelectedIndex = 0;
            }
        }
        else
            lstSP.SelectedIndex = 0;

        if ((txtUser.Text == null) || (txtUser.Text == ""))
        {
            txtUser.Text = SystemInformation.UserName;
        }
    }




    /// <summary>
    /// Clean up any resources being used.
    /// </summary>
    protected override void Dispose(bool Disposing)
    {
        base.Dispose(Disposing);
    }




#region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
        this.lstSP = new System.Windows.Forms.ListBox();
        this.txtUser = new System.Windows.Forms.TextBox();
        this.btnOK = new System.Windows.Forms.Button();
        this.btnCancel = new System.Windows.Forms.Button();
        this.label1 = new System.Windows.Forms.Label();
        this.label2 = new System.Windows.Forms.Label();
        this.SuspendLayout();
        // 
        // lstSP
        // 
        this.lstSP.Location = new System.Drawing.Point(12, 63);
        this.lstSP.Name = "lstSP";
        this.lstSP.Size = new System.Drawing.Size(324, 147);
        this.lstSP.TabIndex = 1;
        this.lstSP.DoubleClick += new System.EventHandler(this.lstSP_DoubleClick);
        // 
        // txtUser
        // 
        this.txtUser.Location = new System.Drawing.Point(13, 20);
        this.txtUser.MaxLength = 30;
        this.txtUser.Name = "txtUser";
        this.txtUser.Size = new System.Drawing.Size(326, 20);
        this.txtUser.TabIndex = 5;
        this.txtUser.Text = "";
        // 
        // btnOK
        // 
        this.btnOK.DialogResult = System.Windows.Forms.DialogResult.OK;
        this.btnOK.Location = new System.Drawing.Point(181, 215);
        this.btnOK.Name = "btnOK";
        this.btnOK.Size = new System.Drawing.Size(74, 27);
        this.btnOK.TabIndex = 2;
        this.btnOK.Text = "OK";
        this.btnOK.Click += new System.EventHandler(this.btnOK_Click);
        // 
        // btnCancel
        // 
        this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
        this.btnCancel.Location = new System.Drawing.Point(260, 215);
        this.btnCancel.Name = "btnCancel";
        this.btnCancel.Size = new System.Drawing.Size(74, 27);
        this.btnCancel.TabIndex = 3;
        this.btnCancel.Text = "Cancel";
        this.btnCancel.Click += new System.EventHandler(this.btnCancel_Click);
        // 
        // label1
        // 
        this.label1.Location = new System.Drawing.Point(11, 44);
        this.label1.Name = "label1";
        this.label1.Size = new System.Drawing.Size(376, 19);
        this.label1.TabIndex = 0;
        this.label1.Text = "Please pick your service provider:";
        // 
        // label2
        // 
        this.label2.Location = new System.Drawing.Point(10, 5);
        this.label2.Name = "label2";
        this.label2.Size = new System.Drawing.Size(327, 12);
        this.label2.TabIndex = 4;
        this.label2.Text = "User Name:";
        // 
        // ChooseServiceProviderForm
        // 
        this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
        this.AcceptButton = this.btnOK;
        this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
        this.CancelButton = this.btnCancel;
        this.ClientSize = new System.Drawing.Size(343, 246);
        this.ControlBox = false;
        this.Controls.AddRange(new System.Windows.Forms.Control[] {
                                                                        this.txtUser,
                                                                        this.label2,
                                                                        this.btnCancel,
                                                                        this.btnOK,
                                                                        this.lstSP,
                                                                        this.label1});
        this.MaximizeBox = false;
        this.MinimizeBox = false;
        this.Name = "ChooseServiceProviderForm";
        this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
        this.Text = "Choose Service Provider";
        this.ResumeLayout(false);

    }
#endregion




    /// <summary>
    /// They don't want to run this sample
    /// </summary>
    private void btnCancel_Click(object sender, System.EventArgs e)
    {
        this.Dispose();
    }




    /// <summary>
    /// Same as clicking ok
    /// </summary>
    private void lstSP_DoubleClick(object sender, System.EventArgs e)
    {
        // Call the ok button click handler
        object[] parameters = new object[] { sender, e };
        this.BeginInvoke(new System.EventHandler(btnOK_Click), parameters);
    }




    /// <summary>
    /// Select this username and service provider if valid, then continue the wizard
    /// </summary>
    private void btnOK_Click(object sender, System.EventArgs e)
    {

        if ((txtUser.Text == null) || (txtUser.Text == ""))
        {
            MessageBox.Show(this,"Please enter a username before clicking OK.","No Username",MessageBoxButtons.OK,MessageBoxIcon.Information);
            return;
        }
        try
        {
            connectionWizard.ServiceProvider = ((ServiceProviderInformation)lstSP.SelectedItem).Guid;
            connectionWizard.Username = txtUser.Text;
        }
        catch // We assume if we got here there was no selected item.
        {
            MessageBox.Show(this,"Please select a service provider before clicking OK.","No Service Provider",MessageBoxButtons.OK,MessageBoxIcon.Information);
            return;
        }

        Microsoft.Win32.RegistryKey regKey = Microsoft.Win32.Registry.CurrentUser.CreateSubKey("Software\\Microsoft\\DirectX\\SDK\\csDPlay");
        if (regKey != null)
        {
            regKey.SetValue("DirectPlayUserName", txtUser.Text);
            regKey.SetValue("DirectPlaySPIndex", lstSP.SelectedIndex);
            regKey.Close();
        }
        this.Dispose();
    }
}
