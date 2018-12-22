using System;
using System.Drawing;
using Microsoft.DirectX;

namespace SpaceWar {
	/// <summary>
	/// Extends the functionality of the PointF class to include vector operations and rotation.
	/// </summary>
	public struct MyPointF {
		PointF point;

		public MyPointF(float x, float y) {
			point = new PointF(x, y);
		}

		public static MyPointF operator+(MyPointF p1, MyPointF p2) {
			return(new MyPointF(p1.point.X + p2.point.X,
				p1.point.Y + p2.point.Y));
		}

		public static MyPointF operator-(MyPointF p1, MyPointF p2) {
			return(new MyPointF(p1.point.X - p2.point.X,
				p1.point.Y - p2.point.Y));
		}

		public static MyPointF operator+(MyPointF p1, Vector2 vec) {
			return(new MyPointF(p1.point.X + vec.X,
				p1.point.Y + vec.Y));
		}

		public MyPointF Rotate(float angleInDegrees) {
			// rotate the point around zero...
			double angleInRadians = angleInDegrees * (Math.PI / 180.0f);
			float newX = (float)(point.X * Math.Cos(angleInRadians) - point.Y * Math.Sin(angleInRadians));
			float newY = (float)(point.X * Math.Sin(angleInRadians) + point.Y * Math.Cos(angleInRadians));
			
			return (new MyPointF(newX, newY));
		}

		public static MyPointF operator/(MyPointF point, float divisor) {
			return(new MyPointF(point.point.X / divisor, point.point.Y / divisor));
		}

		public static MyPointF operator*(MyPointF point, float multiplier) {
			return(new MyPointF(point.point.X * multiplier, point.point.Y * multiplier));
		}

		public float X {
			get {
				return(point.X);
			}
			set {
				point.X = value;
			}
		}

		public float Y {
			get {
				return(point.Y);
			}
			set {
				point.Y = value;
			}
		}

		public static implicit operator PointF(MyPointF point) {
			return(point.point);
		}

		public static implicit operator MyPointF(PointF point) {
			return(new MyPointF(point.X, point.Y));
		}		
		
		public static implicit operator MyPointF(Point point) {
			return(new MyPointF(point.X, point.Y));
		}

		public float Length {
			get {
				return((float) Math.Sqrt(point.X * point.X + point.Y * point.Y));
			}
		}

		public static float Distance(MyPointF pt1, MyPointF pt2) {
			MyPointF delta = pt1 - pt2;
			return (float) Math.Sqrt(delta.X * delta.X + delta.Y * delta.Y);
		}

		public override string ToString() {
			return(String.Format("({0}, {1})",
				point.X, point.Y));
		}
	}
}
