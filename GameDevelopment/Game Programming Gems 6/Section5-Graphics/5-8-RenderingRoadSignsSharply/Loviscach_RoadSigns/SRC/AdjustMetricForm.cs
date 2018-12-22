using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace BilevelTextureMagnification
{
	/// <summary>
	/// Non-modal dialog to adjust the settings of the metric used for the optimization
	/// </summary>
	internal class AdjustMetricForm : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.TrackBar allIntersectionsTrackBar;
		private System.Windows.Forms.TrackBar anglePositionTrackBar;

		private System.ComponentModel.Container components = null;

		internal AdjustMetricForm()
		{
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(AdjustMetricForm));
			this.anglePositionTrackBar = new System.Windows.Forms.TrackBar();
			this.allIntersectionsTrackBar = new System.Windows.Forms.TrackBar();
			this.label1 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			((System.ComponentModel.ISupportInitialize)(this.anglePositionTrackBar)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.allIntersectionsTrackBar)).BeginInit();
			this.SuspendLayout();
			// 
			// anglePositionTrackBar
			// 
			this.anglePositionTrackBar.LargeChange = 10;
			this.anglePositionTrackBar.Location = new System.Drawing.Point(64, 24);
			this.anglePositionTrackBar.Maximum = 100;
			this.anglePositionTrackBar.Name = "anglePositionTrackBar";
			this.anglePositionTrackBar.Orientation = System.Windows.Forms.Orientation.Vertical;
			this.anglePositionTrackBar.Size = new System.Drawing.Size(45, 136);
			this.anglePositionTrackBar.TabIndex = 0;
			this.anglePositionTrackBar.TickFrequency = 10;
			// 
			// allIntersectionsTrackBar
			// 
			this.allIntersectionsTrackBar.Location = new System.Drawing.Point(8, 192);
			this.allIntersectionsTrackBar.Maximum = 100;
			this.allIntersectionsTrackBar.Name = "allIntersectionsTrackBar";
			this.allIntersectionsTrackBar.Size = new System.Drawing.Size(160, 45);
			this.allIntersectionsTrackBar.TabIndex = 1;
			this.allIntersectionsTrackBar.TickFrequency = 10;
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(40, 8);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(96, 23);
			this.label1.TabIndex = 2;
			this.label1.Text = "Favor Angle";
			this.label1.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(96, 232);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(88, 32);
			this.label2.TabIndex = 3;
			this.label2.Text = "At Vert/Horiz Intersections";
			this.label2.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// label3
			// 
			this.label3.Location = new System.Drawing.Point(-8, 232);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(72, 32);
			this.label3.TabIndex = 4;
			this.label3.Text = "Always";
			this.label3.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// label4
			// 
			this.label4.Location = new System.Drawing.Point(32, 168);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(112, 16);
			this.label4.TabIndex = 5;
			this.label4.Text = "Favor Position";
			this.label4.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// AdjustMetricForm
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(184, 270);
			this.ControlBox = false;
			this.Controls.Add(this.label4);
			this.Controls.Add(this.label3);
			this.Controls.Add(this.label2);
			this.Controls.Add(this.label1);
			this.Controls.Add(this.allIntersectionsTrackBar);
			this.Controls.Add(this.anglePositionTrackBar);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
			this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "AdjustMetricForm";
			this.Text = "Adjust Metric";
			this.TopMost = true;
			((System.ComponentModel.ISupportInitialize)(this.anglePositionTrackBar)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.allIntersectionsTrackBar)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		internal void LoadValues(double anglePositionValue, double allIntersectionsValue)
		{
			anglePositionTrackBar.Value = (int)(100.0*anglePositionValue + 0.5);
			allIntersectionsTrackBar.Value = (int)(100.0*allIntersectionsValue + 0.5);
		}

		internal void GiveValues(out double anglePositionValue, out double allIntersectionsValue)
		{
			anglePositionValue = 0.01*anglePositionTrackBar.Value;
			allIntersectionsValue = 0.01*allIntersectionsTrackBar.Value;
		}
	}
}
