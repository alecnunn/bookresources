using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace BilevelTextureMagnification
{
	/// <summary>
	/// Form to set the two or four .dds files to combine as channels into a single .dds file;
	/// the actual processing is delegated to the RenderWindow class so that we're not concerned with DX stuff here
	/// </summary>
	public class MulticolorForm : System.Windows.Forms.Form
	{
		private RenderWindow myRenderWindow;

		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Button browse1Button;
		private System.Windows.Forms.TextBox input1TextBox;
		private System.Windows.Forms.TextBox input2TextBox;
		private System.Windows.Forms.TextBox input3TextBox;
		private System.Windows.Forms.TextBox input4TextBox;
		private System.Windows.Forms.TextBox outputTextBox;
		private System.Windows.Forms.Button browse2Button;
		private System.Windows.Forms.Button browse3Button;
		private System.Windows.Forms.Button browse4Button;
		private System.Windows.Forms.Button browseOutputButton;
		private System.Windows.Forms.Button combineButton;
		private System.Windows.Forms.RadioButton twoChannelsRadioButton;
		private System.Windows.Forms.RadioButton fourChannelsRadioButton;

		private System.ComponentModel.Container components = null;

		internal MulticolorForm(RenderWindow rw)
		{
			myRenderWindow = rw;

			InitializeComponent();
		}

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
		private void InitializeComponent()
		{
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(MulticolorForm));
			this.browse1Button = new System.Windows.Forms.Button();
			this.twoChannelsRadioButton = new System.Windows.Forms.RadioButton();
			this.fourChannelsRadioButton = new System.Windows.Forms.RadioButton();
			this.label1 = new System.Windows.Forms.Label();
			this.input1TextBox = new System.Windows.Forms.TextBox();
			this.input2TextBox = new System.Windows.Forms.TextBox();
			this.input3TextBox = new System.Windows.Forms.TextBox();
			this.input4TextBox = new System.Windows.Forms.TextBox();
			this.outputTextBox = new System.Windows.Forms.TextBox();
			this.browse2Button = new System.Windows.Forms.Button();
			this.browse3Button = new System.Windows.Forms.Button();
			this.browse4Button = new System.Windows.Forms.Button();
			this.browseOutputButton = new System.Windows.Forms.Button();
			this.combineButton = new System.Windows.Forms.Button();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// browse1Button
			// 
			this.browse1Button.Location = new System.Drawing.Point(336, 136);
			this.browse1Button.Name = "browse1Button";
			this.browse1Button.TabIndex = 0;
			this.browse1Button.Text = "Browse ...";
			this.browse1Button.Click += new System.EventHandler(this.browseForInput);
			// 
			// twoChannelsRadioButton
			// 
			this.twoChannelsRadioButton.Checked = true;
			this.twoChannelsRadioButton.Location = new System.Drawing.Point(24, 40);
			this.twoChannelsRadioButton.Name = "twoChannelsRadioButton";
			this.twoChannelsRadioButton.Size = new System.Drawing.Size(176, 24);
			this.twoChannelsRadioButton.TabIndex = 1;
			this.twoChannelsRadioButton.TabStop = true;
			this.twoChannelsRadioButton.Text = "Two channels (A, L)";
			this.twoChannelsRadioButton.Click += new System.EventHandler(this.changeNumberOfChannels);
			// 
			// fourChannelsRadioButton
			// 
			this.fourChannelsRadioButton.Location = new System.Drawing.Point(24, 64);
			this.fourChannelsRadioButton.Name = "fourChannelsRadioButton";
			this.fourChannelsRadioButton.Size = new System.Drawing.Size(176, 24);
			this.fourChannelsRadioButton.TabIndex = 2;
			this.fourChannelsRadioButton.Text = "Four channels (A, R, G, B)";
			this.fourChannelsRadioButton.Click += new System.EventHandler(this.changeNumberOfChannels);
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(16, 16);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(208, 23);
			this.label1.TabIndex = 3;
			this.label1.Text = "Combine single-channel .dds files:";
			// 
			// input1TextBox
			// 
			this.input1TextBox.Location = new System.Drawing.Point(24, 136);
			this.input1TextBox.Name = "input1TextBox";
			this.input1TextBox.Size = new System.Drawing.Size(296, 20);
			this.input1TextBox.TabIndex = 4;
			this.input1TextBox.Text = "Input file 1";
			// 
			// input2TextBox
			// 
			this.input2TextBox.Location = new System.Drawing.Point(24, 168);
			this.input2TextBox.Name = "input2TextBox";
			this.input2TextBox.Size = new System.Drawing.Size(296, 20);
			this.input2TextBox.TabIndex = 5;
			this.input2TextBox.Text = "Input file 2";
			// 
			// input3TextBox
			// 
			this.input3TextBox.Enabled = false;
			this.input3TextBox.Location = new System.Drawing.Point(24, 200);
			this.input3TextBox.Name = "input3TextBox";
			this.input3TextBox.Size = new System.Drawing.Size(296, 20);
			this.input3TextBox.TabIndex = 6;
			this.input3TextBox.Text = "Input file 3";
			// 
			// input4TextBox
			// 
			this.input4TextBox.Enabled = false;
			this.input4TextBox.Location = new System.Drawing.Point(24, 232);
			this.input4TextBox.Name = "input4TextBox";
			this.input4TextBox.Size = new System.Drawing.Size(296, 20);
			this.input4TextBox.TabIndex = 7;
			this.input4TextBox.Text = "Input file 4";
			// 
			// outputTextBox
			// 
			this.outputTextBox.Location = new System.Drawing.Point(24, 304);
			this.outputTextBox.Name = "outputTextBox";
			this.outputTextBox.Size = new System.Drawing.Size(296, 20);
			this.outputTextBox.TabIndex = 8;
			this.outputTextBox.Text = "Output file";
			// 
			// browse2Button
			// 
			this.browse2Button.Location = new System.Drawing.Point(336, 168);
			this.browse2Button.Name = "browse2Button";
			this.browse2Button.TabIndex = 9;
			this.browse2Button.Text = "Browse ...";
			this.browse2Button.Click += new System.EventHandler(this.browseForInput);
			// 
			// browse3Button
			// 
			this.browse3Button.Enabled = false;
			this.browse3Button.Location = new System.Drawing.Point(336, 200);
			this.browse3Button.Name = "browse3Button";
			this.browse3Button.TabIndex = 10;
			this.browse3Button.Text = "Browse ...";
			this.browse3Button.Click += new System.EventHandler(this.browseForInput);
			// 
			// browse4Button
			// 
			this.browse4Button.Enabled = false;
			this.browse4Button.Location = new System.Drawing.Point(336, 232);
			this.browse4Button.Name = "browse4Button";
			this.browse4Button.TabIndex = 11;
			this.browse4Button.Text = "Browse ...";
			this.browse4Button.Click += new System.EventHandler(this.browseForInput);
			// 
			// browseOutputButton
			// 
			this.browseOutputButton.Location = new System.Drawing.Point(336, 304);
			this.browseOutputButton.Name = "browseOutputButton";
			this.browseOutputButton.TabIndex = 12;
			this.browseOutputButton.Text = "Browse ...";
			this.browseOutputButton.Click += new System.EventHandler(this.browseForOutput);
			// 
			// combineButton
			// 
			this.combineButton.Location = new System.Drawing.Point(336, 368);
			this.combineButton.Name = "combineButton";
			this.combineButton.TabIndex = 13;
			this.combineButton.Text = "Combine";
			this.combineButton.Click += new System.EventHandler(this.combine);
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(16, 104);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(208, 23);
			this.label2.TabIndex = 14;
			this.label2.Text = "Input .dds files:";
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(16, 272);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(208, 23);
			this.label3.TabIndex = 15;
			this.label3.Text = "Output .dds file:";
			// 
			// MulticolorForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(432, 406);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.combineButton);
			this.Controls.Add(this.browseOutputButton);
			this.Controls.Add(this.browse4Button);
			this.Controls.Add(this.browse3Button);
			this.Controls.Add(this.browse2Button);
			this.Controls.Add(this.outputTextBox);
			this.Controls.Add(this.input4TextBox);
			this.Controls.Add(this.input3TextBox);
			this.Controls.Add(this.input2TextBox);
			this.Controls.Add(this.input1TextBox);
			this.Controls.Add(this.twoChannelsRadioButton);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.fourChannelsRadioButton);
			this.Controls.Add(this.browse1Button);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "MulticolorForm";
			this.Text = "Multicolor Combiner";
			this.ResumeLayout(false);

		}
		#endregion

		private void changeNumberOfChannels(object sender, System.EventArgs e)
		{
			if(twoChannelsRadioButton.Checked)
			{
				input3TextBox.Enabled = false;
				browse3Button.Enabled = false;
				input4TextBox.Enabled = false;
				browse4Button.Enabled = false;
			}
			else
			{
				input3TextBox.Enabled = true;
				browse3Button.Enabled = true;
				input4TextBox.Enabled = true;
				browse4Button.Enabled = true;
			}
		}

		private void browseForInput(object sender, System.EventArgs e)
		{
			using(OpenFileDialog fd = new OpenFileDialog())
			{
				fd.Filter = "Texture Files|*.dds";
				DialogResult dr = fd.ShowDialog();
				if(dr == DialogResult.OK)
				{
					TextBox t;
					if(sender == browse1Button)
					{
						t = input1TextBox;
					}
					else if(sender == browse2Button)
					{
						t = input2TextBox;
					}
					else if(sender == browse3Button)
					{
						t = input3TextBox;
					}
					else
					{
						t = input4TextBox;
					}

					t.Text = fd.FileName;
				}
			}		
		}

		private void browseForOutput(object sender, System.EventArgs e)
		{
			using(SaveFileDialog fd = new SaveFileDialog())
			{
				fd.Filter = "Texture Files|*.dds";
				DialogResult dr = fd.ShowDialog();
				if(dr == DialogResult.OK)
				{
					outputTextBox.Text = fd.FileName;
				}
			}
		}

		private void combine(object sender, System.EventArgs e)
		{
			string errorMessage;
			bool resultOK = myRenderWindow.MulticolorCombine(out errorMessage, twoChannelsRadioButton.Checked,
				new string[]{input1TextBox.Text, input2TextBox.Text, input3TextBox.Text, input4TextBox.Text},
				outputTextBox.Text);
			if(resultOK)
			{
				Close();
			}
			else
			{
				MessageBox.Show(this, errorMessage, "Error", MessageBoxButtons.OK, MessageBoxIcon.Error);
			}
		}
	}
}
