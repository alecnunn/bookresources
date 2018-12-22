using System;
using System.Runtime.InteropServices;

namespace SpaceDonuts {
	/// <summary>
	/// Summary description for HighResolutionTimer.
	/// </summary>
	public class HighResolutionTimer {
		[System.Security.SuppressUnmanagedCodeSecurity]
		[DllImport("kernel32")]
		private static extern bool QueryPerformanceFrequency(ref long PerformanceFrequency);

		[System.Security.SuppressUnmanagedCodeSecurity]
		[DllImport("kernel32")]
		private static extern bool QueryPerformanceCounter(ref long PerformanceCount);

		private bool timerInitialized = false;
		private bool usingQPF = false;
		private bool timerStopped = true;
		private long ticksPerSecond = 0;
		private long stopTime = 0;
		private long lastElapsedTime = 0;
		private long baseTime = 0; // Set when timer is started or reset
		private long timeNow = 0; 
		private double elapsedTime = 0.0;

		public HighResolutionTimer() { 
			timerInitialized = true;

			usingQPF = QueryPerformanceFrequency(ref ticksPerSecond);
			if (usingQPF)
				ticksPerSecond = ticksPerSecond;
			else
				throw new Exception("Timer init failure -- no QueryPerformanceCounter");
		}


		public void Reset() {
			QueryPerformanceCounter(ref timeNow);
			baseTime        = timeNow;
			lastElapsedTime = timeNow;
			stopTime        = 0;
			timerStopped     = false;
		}

		public void Start() {
			if (stopTime != 0)
				timeNow = stopTime;
			else
				QueryPerformanceCounter(ref timeNow);

			if (timerStopped)
				baseTime += timeNow - stopTime;
			stopTime = 0;
			lastElapsedTime = timeNow;
			timerStopped = false;
		}

		public void Stop() {
			QueryPerformanceCounter(ref timeNow);
			stopTime        = timeNow;
			lastElapsedTime = timeNow;
			timerStopped    = true;
		}

		public long Frequency{
			get { return ticksPerSecond; }
		}

		public void Advance() {
			stopTime += ticksPerSecond/10;
		}

		public float AbsoluteTime {
			get { 
				if (stopTime != 0)
					timeNow = stopTime;
				else
					QueryPerformanceCounter(ref timeNow);
				return (float) (timeNow / (double) ticksPerSecond);
			}
		}

		public float ApplicationTime {
			get {
				return TimeDelta(baseTime);
			}
		}

		public float ElapsedTime {
			get {
				return TimeDelta(lastElapsedTime);
			}
		}

		private float TimeDelta(float refTime) {
			QueryPerformanceCounter(ref timeNow);
			elapsedTime = (double) (timeNow - refTime) / (double) ticksPerSecond;
			lastElapsedTime = timeNow;
			return (float)elapsedTime;
		}

		public float PeekTime {
			get {
					long peek = 0;
					QueryPerformanceCounter(ref peek);
					return (float) (peek - lastElapsedTime) / (float) ticksPerSecond;
				}
		}
	}
}
