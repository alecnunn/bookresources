using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;


	/// <summary>
	/// Spacewar Splashscreen
	/// </summary>
public class SplashScreen : System.Windows.Forms.Form {
	private MainClass mainClass;
	private System.Windows.Forms.Button helpButton;
	private System.Windows.Forms.Button startButton;
	private System.Windows.Forms.Button closeButton;
	private System.Windows.Forms.Button soloButton;
	private System.Windows.Forms.GroupBox groupBox1;
	private System.Windows.Forms.RadioButton radioButton1;
	private System.Windows.Forms.RadioButton radioButton2;
	private System.Windows.Forms.RadioButton radioButton3;
	private System.Windows.Forms.CheckBox checkBox1;
	/// <summary>
	/// Required designer variable.
	/// </summary>
	private System.ComponentModel.Container components = null;



	public SplashScreen(MainClass main) {
		//
		// Required for Windows Form Designer support
		//
		InitializeComponent();
		this.mainClass = main;
		radioButton3.Checked = true;
		checkBox1.Checked = true;
		mainClass.GameFormSize = new Size(1024,768);
		mainClass.FullScreen = true;

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
		System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(SplashScreen));
		this.helpButton = new System.Windows.Forms.Button();
		this.startButton = new System.Windows.Forms.Button();
		this.closeButton = new System.Windows.Forms.Button();
		this.soloButton = new System.Windows.Forms.Button();
		this.groupBox1 = new System.Windows.Forms.GroupBox();
		this.checkBox1 = new System.Windows.Forms.CheckBox();
		this.radioButton3 = new System.Windows.Forms.RadioButton();
		this.radioButton2 = new System.Windows.Forms.RadioButton();
		this.radioButton1 = new System.Windows.Forms.RadioButton();
		this.groupBox1.SuspendLayout();
		this.SuspendLayout();
		// 
		// helpButton
		// 
		this.helpButton.ForeColor = System.Drawing.Color.White;
		this.helpButton.Location = new System.Drawing.Point(440, 280);
		this.helpButton.Name = "helpButton";
		this.helpButton.Size = new System.Drawing.Size(136, 32);
		this.helpButton.TabIndex = 5;
		this.helpButton.Text = "Show Help";
		this.helpButton.Click += new System.EventHandler(this.helpButton_Click);
		// 
		// startButton
		// 
		this.startButton.ForeColor = System.Drawing.Color.White;
		this.startButton.Location = new System.Drawing.Point(440, 364);
		this.startButton.Name = "startButton";
		this.startButton.RightToLeft = System.Windows.Forms.RightToLeft.No;
		this.startButton.Size = new System.Drawing.Size(136, 32);
		this.startButton.TabIndex = 7;
		this.startButton.Text = "Network Play";
		this.startButton.Click += new System.EventHandler(this.startButton_Click);
		// 
		// closeButton
		// 
		this.closeButton.DialogResult = System.Windows.Forms.DialogResult.Cancel;
		this.closeButton.ForeColor = System.Drawing.Color.White;
		this.closeButton.Location = new System.Drawing.Point(440, 406);
		this.closeButton.Name = "closeButton";
		this.closeButton.RightToLeft = System.Windows.Forms.RightToLeft.No;
		this.closeButton.Size = new System.Drawing.Size(136, 32);
		this.closeButton.TabIndex = 8;
		this.closeButton.Text = "Exit";
		this.closeButton.Click += new System.EventHandler(this.closeButton_Click);
		// 
		// soloButton
		// 
		this.soloButton.ForeColor = System.Drawing.Color.White;
		this.soloButton.Location = new System.Drawing.Point(440, 322);
		this.soloButton.Name = "soloButton";
		this.soloButton.Size = new System.Drawing.Size(136, 32);
		this.soloButton.TabIndex = 6;
		this.soloButton.Text = "Solo Play";
		this.soloButton.Click += new System.EventHandler(this.soloButton_Click);
		// 
		// groupBox1
		// 
		this.groupBox1.Controls.Add(this.checkBox1);
		this.groupBox1.Controls.Add(this.radioButton3);
		this.groupBox1.Controls.Add(this.radioButton2);
		this.groupBox1.Controls.Add(this.radioButton1);
		this.groupBox1.ForeColor = System.Drawing.Color.White;
		this.groupBox1.Location = new System.Drawing.Point(16, 320);
		this.groupBox1.Name = "groupBox1";
		this.groupBox1.Size = new System.Drawing.Size(128, 128);
		this.groupBox1.TabIndex = 4;
		this.groupBox1.TabStop = false;
		this.groupBox1.Text = "Screen Resolution";
		// 
		// checkBox1
		// 
		this.checkBox1.Checked = true;
		this.checkBox1.CheckState = System.Windows.Forms.CheckState.Checked;
		this.checkBox1.Location = new System.Drawing.Point(16, 96);
		this.checkBox1.Name = "checkBox1";
		this.checkBox1.TabIndex = 3;
		this.checkBox1.Text = "Full Screen";
		this.checkBox1.CheckedChanged += new System.EventHandler(this.checkBox1_CheckedChanged);
		// 
		// radioButton3
		// 
		this.radioButton3.Checked = true;
		this.radioButton3.ForeColor = System.Drawing.Color.White;
		this.radioButton3.Location = new System.Drawing.Point(16, 72);
		this.radioButton3.Name = "radioButton3";
		this.radioButton3.Size = new System.Drawing.Size(80, 24);
		this.radioButton3.TabIndex = 2;
		this.radioButton3.TabStop = true;
		this.radioButton3.Text = "1024 x 768";
		this.radioButton3.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
		// 
		// radioButton2
		// 
		this.radioButton2.ForeColor = System.Drawing.Color.White;
		this.radioButton2.Location = new System.Drawing.Point(16, 48);
		this.radioButton2.Name = "radioButton2";
		this.radioButton2.Size = new System.Drawing.Size(72, 24);
		this.radioButton2.TabIndex = 1;
		this.radioButton2.Text = "800 x 600";
		this.radioButton2.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
		// 
		// radioButton1
		// 
		this.radioButton1.ForeColor = System.Drawing.Color.White;
		this.radioButton1.Location = new System.Drawing.Point(16, 24);
		this.radioButton1.Name = "radioButton1";
		this.radioButton1.Size = new System.Drawing.Size(72, 24);
		this.radioButton1.TabIndex = 0;
		this.radioButton1.Text = "640 x 480";
		this.radioButton1.CheckedChanged += new System.EventHandler(this.radioButton_CheckedChanged);
		// 
		// SplashScreen
		// 
		this.AcceptButton = this.soloButton;
		this.AutoScale = false;
		this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
		this.BackColor = System.Drawing.Color.Black;
		this.BackgroundImage = ((System.Drawing.Image)(resources.GetObject("$this.BackgroundImage")));
		this.CancelButton = this.closeButton;
		this.ClientSize = new System.Drawing.Size(594, 454);
		this.ControlBox = false;
		this.Controls.Add(this.groupBox1);
		this.Controls.Add(this.soloButton);
		this.Controls.Add(this.closeButton);
		this.Controls.Add(this.startButton);
		this.Controls.Add(this.helpButton);
		this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.Fixed3D;
		this.MaximizeBox = false;
		this.MaximumSize = new System.Drawing.Size(600, 460);
		this.MinimizeBox = false;
		this.MinimumSize = new System.Drawing.Size(600, 460);
		this.Name = "SplashScreen";
		this.ShowInTaskbar = false;
		this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Hide;
		this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
		this.groupBox1.ResumeLayout(false);
		this.ResumeLayout(false);

	}
	#endregion

	private void helpButton_Click(object sender, System.EventArgs e) {
		HelpScreen helpScreen = new HelpScreen();
		helpScreen.ShowDialog();
	}

	private void startButton_Click(object sender, System.EventArgs e) {
		mainClass.EnableNetwork = true;
		this.Close();
	}

	private void closeButton_Click(object sender, System.EventArgs e) {
		Environment.Exit(0);
	}

	private void soloButton_Click(object sender, System.EventArgs e) {
		mainClass.EnableNetwork  = false;
		this.Close();
	}

	private void radioButton_CheckedChanged(object sender, System.EventArgs e) {
		RadioButton btn = (RadioButton)sender;
		Console.WriteLine(btn.ToString());
		if (btn.Checked) {
			if (btn == radioButton1)
				mainClass.GameFormSize = new Size(640,480);
			if (btn == radioButton2)
				mainClass.GameFormSize = new Size(800, 600);
			if (btn == radioButton3)
				mainClass.GameFormSize = new Size(1024, 768);
		}
	}

	private void checkBox1_CheckedChanged(object sender, System.EventArgs e) {
		mainClass.FullScreen = checkBox1.Checked;
	}
}
