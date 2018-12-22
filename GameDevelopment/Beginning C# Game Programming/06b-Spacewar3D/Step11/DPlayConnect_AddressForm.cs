//-----------------------------------------------------------------------------
// File: DPlayConnect_AddressForm.cs
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
/// Summary description for AddressForm.
/// </summary>
public class AddressForm : System.Windows.Forms.Form
{
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.Button okButton;
    private System.Windows.Forms.Button cancelButton;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.TextBox hostnameTextBox;
    private System.Windows.Forms.TextBox portTextBox;

	/// <summary>
	/// Required designer variable.
	/// </summary>
	private System.ComponentModel.Container components = null;



    /// <summary>
    /// Constructor
    /// </summary>
    public AddressForm( int defaultPort )
    {
        //
        // Required for Windows Form Designer support
        //
        InitializeComponent();

        // Set default port value
        Port = defaultPort;
    }




    /// <summary>
    /// The remote port on which to connect
    /// </summary>
    public int Port
    {
        get
        {
            int port = 0;

            try 
            { 
                port = int.Parse(portTextBox.Text); 
            } 
            catch {}

            return port;
        }
        set 
        { 
            if( value > 0 )
                portTextBox.Text = value.ToString(); 
        }
    }




    /// <summary>
    /// Remote hostname
    /// </summary>
    public string Hostname
    {
        get 
        { return hostnameTextBox.Text; }
        set 
        { hostnameTextBox.Text = value; }
    }



	/// <summary>
	/// Clean up any resources being used.
	/// </summary>
	protected override void Dispose( bool disposing )
	{
		if( disposing )
		{
			if(components != null)
			{
				components.Dispose();
			}
		}
		base.Dispose( disposing );
	}

	#region Windows Form Designer generated code
	/// <summary>
	/// Required method for Designer support - do not modify
	/// the contents of this method with the code editor.
	/// </summary>
	private void InitializeComponent()
	{
		this.label1 = new System.Windows.Forms.Label();
		this.okButton = new System.Windows.Forms.Button();
		this.cancelButton = new System.Windows.Forms.Button();
		this.label2 = new System.Windows.Forms.Label();
		this.label3 = new System.Windows.Forms.Label();
		this.hostnameTextBox = new System.Windows.Forms.TextBox();
		this.portTextBox = new System.Windows.Forms.TextBox();
		this.SuspendLayout();
		// 
		// label1
		// 
		this.label1.Location = new System.Drawing.Point(16, 16);
		this.label1.Name = "label1";
		this.label1.Size = new System.Drawing.Size(248, 48);
		this.label1.TabIndex = 0;
		this.label1.Text = "Please enter the optional components of the remote session address. If set blank," +
			" DirectPlay will attempt to search the local network.";
		// 
		// okButton
		// 
		this.okButton.Location = new System.Drawing.Point(104, 144);
		this.okButton.Name = "okButton";
		this.okButton.Size = new System.Drawing.Size(72, 24);
		this.okButton.TabIndex = 1;
		this.okButton.Text = "&OK";
		this.okButton.Click += new System.EventHandler(this.okButton_Click);
		// 
		// cancelButton
		// 
		this.cancelButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
		this.cancelButton.Location = new System.Drawing.Point(184, 144);
		this.cancelButton.Name = "cancelButton";
		this.cancelButton.Size = new System.Drawing.Size(72, 24);
		this.cancelButton.TabIndex = 2;
		this.cancelButton.Text = "&Cancel";
		this.cancelButton.Click += new System.EventHandler(this.cancelButton_Click);
		// 
		// label2
		// 
		this.label2.Location = new System.Drawing.Point(32, 80);
		this.label2.Name = "label2";
		this.label2.Size = new System.Drawing.Size(100, 16);
		this.label2.TabIndex = 3;
		this.label2.Text = "Hostname";
		// 
		// label3
		// 
		this.label3.Location = new System.Drawing.Point(160, 80);
		this.label3.Name = "label3";
		this.label3.Size = new System.Drawing.Size(56, 16);
		this.label3.TabIndex = 4;
		this.label3.Text = "Port";
		// 
		// hostnameTextBox
		// 
		this.hostnameTextBox.Location = new System.Drawing.Point(32, 96);
		this.hostnameTextBox.Name = "hostnameTextBox";
		this.hostnameTextBox.Size = new System.Drawing.Size(120, 20);
		this.hostnameTextBox.TabIndex = 5;
		this.hostnameTextBox.Text = "";
		// 
		// portTextBox
		// 
		this.portTextBox.Location = new System.Drawing.Point(160, 96);
		this.portTextBox.Name = "portTextBox";
		this.portTextBox.Size = new System.Drawing.Size(56, 20);
		this.portTextBox.TabIndex = 6;
		this.portTextBox.Text = "2580";
		// 
		// AddressForm
		// 
		this.AcceptButton = this.okButton;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		this.CancelButton = this.cancelButton;
		this.ClientSize = new System.Drawing.Size(264, 176);
		this.Controls.Add(this.portTextBox);
		this.Controls.Add(this.hostnameTextBox);
		this.Controls.Add(this.label3);
		this.Controls.Add(this.label2);
		this.Controls.Add(this.cancelButton);
		this.Controls.Add(this.okButton);
		this.Controls.Add(this.label1);
		this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
		this.MaximizeBox = false;
		this.MinimizeBox = false;
		this.Name = "AddressForm";
		this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
		this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
		this.Text = "Remote Address";
		this.ResumeLayout(false);

	}
	#endregion

    private void okButton_Click(object sender, System.EventArgs e)
    {
        DialogResult = DialogResult.OK;
    }

    private void cancelButton_Click(object sender, System.EventArgs e)
    {
        DialogResult = DialogResult.Cancel;
    }

}
