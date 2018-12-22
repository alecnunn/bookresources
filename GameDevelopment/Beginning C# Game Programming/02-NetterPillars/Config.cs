using System;
using System.Drawing;
using System.Windows.Forms;
namespace Netterpillars {
	public class Config : System.Windows.Forms.Form {
		public Config() : base() {        

			//This call is required by the Windows Form Designer.
			InitializeComponent();

			//Add any initialization after the InitializeComponent() call
		
		}

		//Form overrides dispose to clean up the component list.
		protected override void Dispose(bool disposing) {
			if (disposing) {
				if (components != null) {
					components.Dispose();
				}
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code
		internal System.Windows.Forms.Button cmdOK;
		internal System.Windows.Forms.Button cmdCancel;
		internal System.Windows.Forms.Label lblNetterpillars;
		internal System.Windows.Forms.Label lblSpiders;
		internal System.Windows.Forms.Label lblMushrooms;
		internal System.Windows.Forms.Label lblGameField;
		internal System.Windows.Forms.NumericUpDown updSpiders;
		internal System.Windows.Forms.NumericUpDown updNetterpillars;
		internal System.Windows.Forms.DomainUpDown updGameField;
		internal System.Windows.Forms.DomainUpDown updMushrooms;

		//Required by the Windows Form Designer
		private System.ComponentModel.Container components = new System.ComponentModel.Container();

		//NOTE: The following procedure is required by the Windows Form Designer
		//It can be modified using the Windows Form Designer.  
		//Do not modify it using the code editor.
		[System.Diagnostics.DebuggerStepThrough]
		private void InitializeComponent() {
			this.lblMushrooms = new System.Windows.Forms.Label();
			this.updSpiders = new System.Windows.Forms.NumericUpDown();
			this.updNetterpillars = new System.Windows.Forms.NumericUpDown();
			this.updGameField = new System.Windows.Forms.DomainUpDown();
			this.lblNetterpillars = new System.Windows.Forms.Label();
			this.lblSpiders = new System.Windows.Forms.Label();
			this.cmdCancel = new System.Windows.Forms.Button();
			this.lblGameField = new System.Windows.Forms.Label();
			this.cmdOK = new System.Windows.Forms.Button();
			this.updMushrooms = new System.Windows.Forms.DomainUpDown();
			((System.ComponentModel.ISupportInitialize)(this.updSpiders)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.updNetterpillars)).BeginInit();
			this.SuspendLayout();
			// 
			// lblMushrooms
			// 
			this.lblMushrooms.Location = new System.Drawing.Point(8, 48);
			this.lblMushrooms.Name = "lblMushrooms";
			this.lblMushrooms.Size = new System.Drawing.Size(128, 32);
			this.lblMushrooms.TabIndex = 2;
			this.lblMushrooms.Text = "Mushrooms";
			// 
			// updSpiders
			// 
			this.updSpiders.Location = new System.Drawing.Point(144, 128);
			this.updSpiders.Maximum = new System.Decimal(new int[] {
																	   5,
																	   0,
																	   0,
																	   0});
			this.updSpiders.Minimum = new System.Decimal(new int[] {
																	   1,
																	   0,
																	   0,
																	   0});
			this.updSpiders.Name = "updSpiders";
			this.updSpiders.Size = new System.Drawing.Size(48, 34);
			this.updSpiders.TabIndex = 2;
			this.updSpiders.Value = new System.Decimal(new int[] {
																	 1,
																	 0,
																	 0,
																	 0});
			// 
			// updNetterpillars
			// 
			this.updNetterpillars.Location = new System.Drawing.Point(144, 8);
			this.updNetterpillars.Maximum = new System.Decimal(new int[] {
																			 4,
																			 0,
																			 0,
																			 0});
			this.updNetterpillars.Minimum = new System.Decimal(new int[] {
																			 1,
																			 0,
																			 0,
																			 0});
			this.updNetterpillars.Name = "updNetterpillars";
			this.updNetterpillars.Size = new System.Drawing.Size(48, 34);
			this.updNetterpillars.TabIndex = 1;
			this.updNetterpillars.Value = new System.Decimal(new int[] {
																		   1,
																		   0,
																		   0,
																		   0});
			// 
			// updGameField
			// 
			this.updGameField.Items.Add("Big");
			this.updGameField.Items.Add("Medium");
			this.updGameField.Items.Add("Small");
			this.updGameField.Location = new System.Drawing.Point(144, 88);
			this.updGameField.Name = "updGameField";
			this.updGameField.TabIndex = 4;
			this.updGameField.Text = "Medium";
			// 
			// lblNetterpillars
			// 
			this.lblNetterpillars.Location = new System.Drawing.Point(8, 8);
			this.lblNetterpillars.Name = "lblNetterpillars";
			this.lblNetterpillars.Size = new System.Drawing.Size(144, 32);
			this.lblNetterpillars.TabIndex = 2;
			this.lblNetterpillars.Text = "Netterpillars";
			// 
			// lblSpiders
			// 
			this.lblSpiders.Location = new System.Drawing.Point(8, 128);
			this.lblSpiders.Name = "lblSpiders";
			this.lblSpiders.Size = new System.Drawing.Size(120, 29);
			this.lblSpiders.TabIndex = 3;
			this.lblSpiders.Text = "Spiders";
			// 
			// cmdCancel
			// 
			this.cmdCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
			this.cmdCancel.Location = new System.Drawing.Point(272, 48);
			this.cmdCancel.Name = "cmdCancel";
			this.cmdCancel.Size = new System.Drawing.Size(80, 32);
			this.cmdCancel.TabIndex = 7;
			this.cmdCancel.Text = "Cancel";
			// 
			// lblGameField
			// 
			this.lblGameField.Location = new System.Drawing.Point(8, 88);
			this.lblGameField.Name = "lblGameField";
			this.lblGameField.Size = new System.Drawing.Size(128, 29);
			this.lblGameField.TabIndex = 3;
			this.lblGameField.Text = "Game Field";
			// 
			// cmdOK
			// 
			this.cmdOK.DialogResult = System.Windows.Forms.DialogResult.OK;
			this.cmdOK.Location = new System.Drawing.Point(272, 8);
			this.cmdOK.Name = "cmdOK";
			this.cmdOK.Size = new System.Drawing.Size(80, 32);
			this.cmdOK.TabIndex = 6;
			this.cmdOK.Text = "OK";
			this.cmdOK.Click += new System.EventHandler(this.cmdOK_Click);
			// 
			// updMushrooms
			// 
			this.updMushrooms.Items.Add("Many");
			this.updMushrooms.Items.Add("Just Right");
			this.updMushrooms.Items.Add("Few");
			this.updMushrooms.Location = new System.Drawing.Point(144, 48);
			this.updMushrooms.Name = "updMushrooms";
			this.updMushrooms.TabIndex = 8;
			this.updMushrooms.Text = "Just Right";
			// 
			// Config
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(10, 27);
			this.ClientSize = new System.Drawing.Size(368, 128);
			this.Controls.AddRange(new System.Windows.Forms.Control[] {
																		  this.updMushrooms,
																		  this.updGameField,
																		  this.updSpiders,
																		  this.updNetterpillars,
																		  this.lblMushrooms,
																		  this.lblGameField,
																		  this.lblSpiders,
																		  this.lblNetterpillars,
																		  this.cmdCancel,
																		  this.cmdOK});
			this.Font = new System.Drawing.Font("Comic Sans MS", 14.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Name = "Config";
			this.Text = "Game Configuration";
			this.Load += new System.EventHandler(this.Config_Load);
			((System.ComponentModel.ISupportInitialize)(this.updSpiders)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.updNetterpillars)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void cmdOK_Click(System.Object sender, System.EventArgs e) {
			MainGame.netterpillarGameEngine.Size = (GameEngine.GameFieldSizes)updGameField.SelectedIndex;
			MainGame.netterpillarGameEngine.NetterpillarNumber = (int)System.Math.Round(updNetterpillars.Value);
			MainGame.netterpillarGameEngine.Mushrooms = (GameEngine.MushroomQuantity)updMushrooms.SelectedIndex;
			//MainGame.netterpillarGameEngine.Spiders = updSpiders.Value
		}

		private void Config_Load(object sender, System.EventArgs e) {
			updGameField.SelectedIndex = (int)MainGame.netterpillarGameEngine.Size;
			updNetterpillars.Value = MainGame.netterpillarGameEngine.NetterpillarNumber;
			updMushrooms.SelectedIndex = (int)MainGame.netterpillarGameEngine.Mushrooms;
			//updSpiders.Value = MainGame.netterpillarGameEngine.Spiders
		}
	}
}