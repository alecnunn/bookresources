using System;
using System.Collections;
using System.Drawing;
using Microsoft.DirectX;
using Microsoft.DirectX.DirectDraw;

namespace SpaceWar {
	/// <summary>
	/// Summary description for RotatableShape.
	/// </summary>
	[Serializable]
	public class RotatableShape {
		static ArrayList shapes = new ArrayList();


		
		Point center;
		int currentStep;

		// add a line at all the rotations
		public static void AddLine(MyPointF end1, MyPointF end2, float scale) {
			float angleIncrement = 360f / Constants.RotateSteps;
			float angle = 0.0f;
			for (int step = 0; step < Constants.RotateSteps; step++) {
				Shape shape = (Shape) shapes[step];
				shape.AddLine(end1.Rotate(angle), end2.Rotate(angle), scale);
				angle += angleIncrement;
			}
		}

		public RotatableShape() {
		}

		public static void CreateShapes() {
			for (int step = 0; step < Constants.RotateSteps; step++) {
				shapes.Add(new Shape());
			}
		}
		public int CurrentStep {
			get {
				return currentStep;
			} 
			set {
				currentStep = value;
			}
		}

		public float Angle {
			get {
				return (360f / Constants.RotateSteps) * currentStep;
			}
		}

		public Point Center {
			get {
				return center;
			}
			set {
				center = value;
			}
		}

		public void ChangeCurrentStep(int delta) {
			currentStep = (currentStep + delta) % Constants.RotateSteps;
			if (currentStep < 0)
				currentStep += Constants.RotateSteps;
		}

		public void IncrementCurrentStep() {
			currentStep++;
			if (currentStep == Constants.RotateSteps)
				currentStep = 0;
		}

		public void DecrementCurrentStep() {
			currentStep--;
			if (currentStep < 0)
				currentStep = Constants.RotateSteps - 1;
		}


		public Line GetCurrentLineByIndex(int index) {
			Shape shape = (Shape) shapes[currentStep];
			return shape[index];			
		}

		public void Draw(Surface surface, int startIndex, int endIndex) {
			Shape shape = (Shape) shapes[currentStep];
			shape.Center = center;
			shape.Draw(surface, startIndex, endIndex);
		}
	}
}
