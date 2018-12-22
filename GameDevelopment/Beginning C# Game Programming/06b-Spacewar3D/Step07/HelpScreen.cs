using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.IO;


	/// <summary>
	/// Summary description for HelpScreen.
	/// </summary>
public class HelpScreen : System.Windows.Forms.Form {
	private System.Windows.Forms.Button closeButton;
	private System.Windows.Forms.TextBox helpText;
	/// <summary>
	/// Required designer variable.
	/// </summary>
	private System.ComponentModel.Container components = null;

	public HelpScreen() {
		//
		// Required for Windows Form Designer support
		//
		InitializeComponent();
		string helpFile = MediaUtilities.FindFile("Help.txt");
		if (File.Exists(helpFile)) {
			StreamReader sr;
			try {
				sr = new StreamReader(helpFile);
				helpText.Text = sr.ReadToEnd();
				sr.Close();
			}
			catch ( Exception e ) {
				helpText.Text = e.ToString();
			}
				
		}
		else {
			helpText.Text = "Unable to locate " + helpFile.ToString();
		}

	}

	/// <summary>
	/// Clean up any resources being used.
	/// </summary>
	protected override void Dispose( bool disposing ) {
		if( disposing ) {
			if(components != null) {
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
	private void InitializeComponent() {
		this.closeButton = new System.Windows.Forms.Button();
		this.helpText = new System.Windows.Forms.TextBox();
		this.SuspendLayout();
		// 
		// closeButton
		// 
		this.closeButton.BackColor = System.Drawing.Color.Black;
		this.closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
		this.closeButton.Dock = System.Windows.Forms.DockStyle.Bottom;
		this.closeButton.ForeColor = System.Drawing.Color.Red;
		this.closeButton.Location = new System.Drawing.Point(0, 615);
		this.closeButton.Name = "closeButton";
		this.closeButton.Size = new System.Drawing.Size(616, 23);
		this.closeButton.TabIndex = 0;
		this.closeButton.Text = "Close";
		this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
		// 
		// helpText
		// 
		this.helpText.BackColor = System.Drawing.Color.Black;
		this.helpText.Dock = System.Windows.Forms.DockStyle.Fill;
		this.helpText.Font = new System.Drawing.Font("Courier New", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
		this.helpText.ForeColor = System.Drawing.Color.Red;
		this.helpText.Location = new System.Drawing.Point(0, 0);
		this.helpText.Multiline = true;
		this.helpText.Name = "helpText";
		this.helpText.ReadOnly = true;
		this.helpText.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
		this.helpText.Size = new System.Drawing.Size(616, 615);
		this.helpText.TabIndex = 1;
		this.helpText.TabStop = false;
		this.helpText.Text = "";
		// 
		// HelpScreen
		// 
		this.AcceptButton = this.closeButton;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		this.CancelButton = this.closeButton;
		this.ClientSize = new System.Drawing.Size(616, 638);
		this.Controls.Add(this.helpText);
		this.Controls.Add(this.closeButton);
		this.Name = "HelpScreen";
		this.Text = "SpaceWar! Help";
		this.ResumeLayout(false);

	}
	#endregion

	private void closeButton_Click(object sender, System.EventArgs e) {
		this.Close();
	}
}