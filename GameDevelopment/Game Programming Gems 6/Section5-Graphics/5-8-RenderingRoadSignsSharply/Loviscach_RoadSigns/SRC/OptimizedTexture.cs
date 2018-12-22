using System;
using System.Drawing;
using System.Drawing.Imaging;
using System.Threading;
using System.Windows.Forms;
using System.Runtime.Serialization;
using System.IO;

namespace BilevelTextureMagnification
{
	/// <summary>
	/// Data model and computation engine for the optimized texture
	/// </summary>
	[Serializable]
	internal class OptimizedTexture : IDisposable, ISerializable
	{
		private Bitmap hires;

		private int[,] optimizedArray;
		// every access to optimizedArray has to be accompanied by lock(this)

		private int[,] optimizedArrayWork;
		// optimizedArrayWork may only be accessed from the optimizerThread
		// once it has been created

		private bool[,] boundaryArray;
		private bool[,] blackWhiteArray;
		private int[,] intersectVert, intersectHoriz;
		private double[,] dirVert, dirHoriz;
		private int downsampling;
		private double anglePositionValue = 0.5;
		private double allIntersectionsValue = 0.5;

		private bool selectedIsVertical = false;
		private int selectedJ = 1;
		private int selectedI = 1;

		private Thread optimizerThread;
		private bool stopRequested = false;

		private ContourEditor myEditor;
		private AdjustMetricForm myAdjustMetricForm;
		private RenderWindow myRenderWindow;
		private Random myRandom = new Random();

		internal OptimizedTexture(string fileName, int downsampling)
		{
			try
			{
				hires = new Bitmap(fileName);
				if(hires.Width > 2048 || hires.Height > 2048)
				{
					throw new Exception("Image size must be 2048x2048 or less");
				}
				finishCTor(downsampling);
			}
			catch(Exception e)
			{
				hires.Dispose();
				hires = null;
				throw e;
			}
		}

		internal OptimizedTexture(Stream st, int downsampling)
		{
			try
			{
				Bitmap b = (Bitmap)Image.FromStream(st, false, true);
				hires = new Bitmap(b); // otherwise we won't be able to serialize hires
				b.Dispose();
				if(hires.Width > 2048 || hires.Height > 2048)
				{
					throw new Exception("Image size must be 2048x2048 or less");
				}
				finishCTor(downsampling);
			}
			catch(Exception e)
			{
				hires.Dispose();
				hires = null;
				throw e;
			}
		}

		private void finishCTor(int downsampling)
		{
			hires.SetResolution(96, 96);
			this.downsampling = downsampling;
			createArrays();

			optimizerThread = new Thread(new ThreadStart(optimize));
			optimizerThread.Priority = ThreadPriority.Lowest;
			optimizerThread.Start();
		}

		private void createArrays()
		{
			int[,] hiresArray = toArray(hires);
			blackWhiteArray = blackAndWhiteArray(hiresArray);
			boundaryArray = toBoundaryArray(hiresArray);
			optimizedArray = toArrayFromBW(blackWhiteArray, boundaryArray);
			optimizedArrayWork = (int[,])optimizedArray.Clone();

			int wTexture = hires.Width/downsampling;
			int hTexture = hires.Height/downsampling;
			intersectVert = new int[hTexture, wTexture];
			intersectHoriz = new int[hTexture, wTexture];
			computeIntersections(hiresArray);
			
			dirVert = new double[hTexture, wTexture];
			dirHoriz = new double[hTexture, wTexture];
			computeDirs(hiresArray);
		}

		// this cannot be part of the deserializing constructor
		internal void SetEditor(ContourEditor editor, AdjustMetricForm form, RenderWindow window)
		{
			myEditor = editor;
			myAdjustMetricForm = form;
			myAdjustMetricForm.LoadValues(anglePositionValue, allIntersectionsValue);
			myRenderWindow = window;
			myRenderWindow.SetBaseTexture(optimizedArrayWork, HiresImage);
		}

		protected OptimizedTexture(SerializationInfo info, StreamingContext context)
		{
			hires = (Bitmap)info.GetValue("hires", typeof(Bitmap));
			hires.SetResolution(96, 96);

			lock(this)
			{
				optimizedArray = (int[,])info.GetValue("optimizedArray", typeof(int[,]));
				optimizedArrayWork = (int[,])optimizedArray.Clone();
			}
	
			int[,] hiresArray = toArray(hires);
			intersectVert = (int[,])info.GetValue("intersectVert", typeof(int[,]));
			intersectHoriz = (int[,])info.GetValue("intersectHoriz", typeof(int[,]));
			dirVert = (double[,])info.GetValue("dirVert", typeof(double[,]));
			dirHoriz = (double[,])info.GetValue("dirHoriz", typeof(double[,]));
			downsampling = info.GetInt32("downsampling");

			blackWhiteArray = blackAndWhiteArray(hiresArray);
			boundaryArray = toBoundaryArray(hiresArray);

			anglePositionValue = info.GetDouble("anglePositionValue");
			allIntersectionsValue = info.GetDouble("allIntersectionsValue");

			optimizerThread = new Thread(new ThreadStart(optimize));
			optimizerThread.Priority = ThreadPriority.Lowest;
			optimizerThread.Start();
		}

		void ISerializable.GetObjectData(SerializationInfo info, StreamingContext context) 
		{
			info.AddValue("hires", hires);

			lock(this)
			{
				int[,] optimizedArrayCopy = (int[,])optimizedArray.Clone();
				info.AddValue("optimizedArray", optimizedArrayCopy);
			}
			
			info.AddValue("intersectVert", intersectVert);
			info.AddValue("intersectHoriz", intersectHoriz);
			info.AddValue("dirVert", dirVert);
			info.AddValue("dirHoriz", dirHoriz);
			info.AddValue("downsampling", downsampling);

			double anglePositionValue;
			double allIntersectionsValue;
			myAdjustMetricForm.GiveValues(out anglePositionValue, out allIntersectionsValue);

			info.AddValue("anglePositionValue", anglePositionValue);
			info.AddValue("allIntersectionsValue", allIntersectionsValue);
		}

		internal void ChangeDownsampling(int downsampling)
		{
			optimizerThread.Abort();
			optimizerThread.Join();
			optimizerThread = null;

			this.downsampling = downsampling;
			finishCTor(downsampling);
			myRenderWindow.SetBaseTexture(optimizedArrayWork, HiresImage);
		}

		public void Dispose()
		{
			// first stop the thread because it may try to access other members of the class
			stopRequested = true;
			optimizerThread.Join();

			hires.Dispose();
			hires = null;
		}

		private void computeIntersections(int[,] hiresArray)
		{
			for(int j = 0; j < hires.Height/downsampling; j++)
			{
				for(int i = 0; i < hires.Width/downsampling; i++)
				{
					if(j >= 1 && blackWhiteArray[j-1, i] != blackWhiteArray[j, i])
					{
						int[] sum = new int[downsampling];
						for(int jj = 0; jj < downsampling; jj++)
						{
							for(int jjj = -downsampling/2; jjj < downsampling/2; jjj++)
							{
								for(int iii = -1; iii <= 1; iii++)
								{
									sum[jj] += hiresArray[j*downsampling+jj-downsampling/2+jjj, i*downsampling + downsampling/2+iii]-128;
								}								
							}
						}
						
						int jMin = 0;
						int aMin = Math.Abs(sum[jMin]);
						for(int jj = 1; jj < downsampling; jj++)
						{
							int aSum = Math.Abs(sum[jj]);
							if(aSum < aMin)
							{
								jMin = jj;
								aMin = aSum;
							}
						}
						intersectVert[j, i] = jMin + j*downsampling - downsampling/2;						
					}

					if(i >= 1 && blackWhiteArray[j, i-1] != blackWhiteArray[j, i])
					{
						int[] sum = new int[downsampling];
						for(int ii = 0; ii < downsampling; ii++)
						{
							for(int jjj = -1; jjj <=1; jjj++)
							{
								for(int iii = -downsampling/2; iii < downsampling/2; iii++)
								{
									sum[ii] += hiresArray[j*downsampling + downsampling/2+jjj, i*downsampling+ii - downsampling/2+iii]-128;
								}								
							}
						}
						
						int iMin = 0;
						int aMin = Math.Abs(sum[iMin]);
						for(int ii = 1; ii < downsampling; ii++)
						{
							int aSum = Math.Abs(sum[ii]);
							if(aSum < aMin)
							{
								iMin = ii;
								aMin = aSum;
							}
						}
						intersectHoriz[j, i] = iMin + i*downsampling-downsampling/2;						
					}	
				}
			}
		}

		private void computeDirs(int[,] hiresArray)
		{
			int minRadius2 = (downsampling*downsampling)/10;
			int maxRadius2 = (downsampling*downsampling)/6;

			for(int j = 0; j < hires.Height/downsampling - 1; j++)
			{
				for(int i = 0; i < hires.Width/downsampling - 1; i++)
				{
					if(j >= 1 && blackWhiteArray[j-1, i] != blackWhiteArray[j, i])
					{
						double cosSum = 0.0;
						double sinSum = 0.0;
						for(int jjj = -downsampling; jjj < downsampling; jjj++)
						{
							for(int iii = -downsampling; iii < downsampling; iii++)
							{
								// restrict to annulus
								int isq = iii*iii + jjj * jjj;							
								if(isq < minRadius2 || isq >= maxRadius2)
								{
									continue;
								}
								
								double c = iii;
								double s = jjj;
								int a = hiresArray[intersectVert[j,i] + jjj, i*downsampling + downsampling/2 + iii];
								cosSum += c*a;
								sinSum += s*a;
							}
						}
						dirVert[j, i] = -cosSum/sinSum;
					}

					if(i >= 1 && blackWhiteArray[j, i] != blackWhiteArray[j, i-1])
					{
						double cosSum = 0.0;
						double sinSum = 0.0;
						for(int jjj = -downsampling; jjj < downsampling; jjj++)
						{
							for(int iii = -downsampling; iii < downsampling; iii++)
							{
								// restrict to annulus
								int isq = iii*iii + jjj * jjj;							
								if(isq < minRadius2 || isq >= maxRadius2)
								{
									continue;
								}
								
								double c = iii;
								double s = jjj;
								int a = hiresArray[j*downsampling + downsampling/2 + jjj, intersectHoriz[j,i] + iii];
								cosSum += c*a;
								sinSum += s*a;
							}
						}
						dirHoriz[j, i] = -sinSum/cosSum;
					}
				}
			}			
		}

		private bool[,] blackAndWhiteArray(int[,] img)
		{
			int w = img.GetLength(1)/downsampling;
			int h = img.GetLength(0)/downsampling;
			bool[,] a = new bool[h, w];

			for(int j = 0; j < h; j++)
			{
				for(int i = 0; i < w; i++)
				{
					a[j, i] = (img[j*downsampling + downsampling/2, i*downsampling + downsampling/2] >= 128);
				}
			}

			return a;
		}

		private int[,] toArrayFromBW(bool[,] bwArray, bool[,] boundaryArray)
		{
			int w = bwArray.GetLength(1);
			int h = bwArray.GetLength(0);
			int[,] a = new int[h, w];

			for(int j = 0; j < h; j++)
			{
				for(int i = 0; i < w; i++)
				{
					if(boundaryArray[j, i])
					{
						a[j, i] = bwArray[j, i] ? 255 : 0;
					}
					else
					{
						a[j, i] = bwArray[j, i] ? 255 : 0;
					}
				}
			}

			return a;
		}

		private bool[,] toBoundaryArray(int[,] img)
		{
			int w = img.GetLength(1)/downsampling;
			int h = img.GetLength(0)/downsampling;
			bool[,] a = new bool[h, w];

			for(int j = 0; j < h; j++)
			{
				for(int i = 0; i < w; i++)
				{
					bool boundary = false;
					bool center = (img[j*downsampling + downsampling/2, i*downsampling + downsampling/2] >= 128);

					int jMin = ((j>0)   ? j-1: 0  );
					int iMin = ((i>0)   ? i-1: 0  );
					int jMax = ((j<h-1) ? j+1: h-1);
					int iMax = ((i<w-1) ? i+1: w-1);

					for(int j1 = jMin; j1 <= jMax; j1++)
					{
						for(int i1 = iMin; i1 <= iMax; i1++)
						{
							if( (img[j1*downsampling + downsampling/2, i1*downsampling + downsampling/2] >= 128) != center)
							{
								boundary = true;
							}
						}
					}
					
					a[j, i] = boundary;					
				}
			}

			return a;
		}

		private int[,] toArray(Bitmap bm)
		{
			BitmapData bmData = bm.LockBits(new Rectangle(0, 0, bm.Width, bm.Height), ImageLockMode.ReadOnly,
				PixelFormat.Format24bppRgb);
			int w = bm.Width;
			int h = bm.Height;
			int[,] a = new int[h, w];

			unsafe
			{
				byte* bmBasePointer = (byte*)bmData.Scan0;
				int bmStride = bmData.Stride;
				for(int j = 0; j < h; j++)
				{
					byte* bmPointer = bmBasePointer + j*bmStride;
					for(int i = 0; i < w; i++)
					{
						a[j, i] = *bmPointer;
						bmPointer += 3;
					}
				}
			}
			bm.UnlockBits(bmData);
			return a;
		}

		private double square(double a)
		{
			return a*a;
		}

		private double distVert(int i, int j, int w, int h)
		{
			if(i < 0 || i >= w || j <= 0 || j >= h) // j may equal 0, but i may not
			{
				return 0.0;
			}

			if(blackWhiteArray[j-1, i] == blackWhiteArray[j, i])
			{
				return 0.0;
			}

			double s1 = 0.0;
			if(i >= 1)
			{
				s1 = Math.Atan(((optimizedArrayWork[j-1, i-1]-128) * (optimizedArrayWork[j, i]-128)
					-(optimizedArrayWork[j, i-1]-128) * (optimizedArrayWork[j-1, i]-128)
					)/square(optimizedArrayWork[j-1, i] - optimizedArrayWork[j, i]));
			}
			double s2 = 0.0;
			if(i <= w-2)
			{
				s2 = Math.Atan(	-((optimizedArrayWork[j-1, i+1]-128) * (optimizedArrayWork[j, i]-128)
					-(optimizedArrayWork[j, i+1]-128) * (optimizedArrayWork[j-1, i]-128)
					)/square(optimizedArrayWork[j-1, i] - optimizedArrayWork[j, i]));
			}
			double dir = Math.Atan(dirVert[j, i]);
			double angleDist = square(s1-dir) + square(s2-dir);
			double locationDist = square(
				(optimizedArrayWork[j-1, i] - 128)/(double)(optimizedArrayWork[j-1, i] - optimizedArrayWork[j, i])
				+ j - 0.5 - intersectVert[j, i]/(double)downsampling
				);
	
			return metric(angleDist, locationDist, dir);
		}

		private double distHoriz(int i, int j, int w, int h)
		{
			if(i <= 0 || i >= w || j < 0 || j >= h) // i may equal 0, but j may not
			{
				return 0.0;
			}

			if(blackWhiteArray[j, i-1] == blackWhiteArray[j, i])
			{
				return 0.0;
			}

			double s1 = 0.0;
			if(j >= 1)
			{
				s1 = Math.Atan(((optimizedArrayWork[j-1, i-1]-128) * (optimizedArrayWork[j, i]-128)
					-(optimizedArrayWork[j-1, i]-128) * (optimizedArrayWork[j, i-1]-128)
					)/square(optimizedArrayWork[j, i-1] - optimizedArrayWork[j, i]));
			}
			double s2 = 0.0;
			if(j <= h-2)
			{
				s2 = Math.Atan(-((optimizedArrayWork[j+1, i-1]-128) * (optimizedArrayWork[j, i]-128)
					-(optimizedArrayWork[j+1, i]-128) * (optimizedArrayWork[j, i-1]-128)
					)/square(optimizedArrayWork[j, i-1] - optimizedArrayWork[j, i]));
			}
			double dir = Math.Atan(dirHoriz[j, i]);
			double angleDist = square(s1-dir) + square(s2-dir);
			double locationDist = square(
				(optimizedArrayWork[j, i-1] - 128)/(double)(optimizedArrayWork[j, i-1] - optimizedArrayWork[j, i])
				+ i - 0.5 - intersectHoriz[j, i]/(double)downsampling
				);
			return metric(angleDist, locationDist, dir);
		}

		private double metric(double angleDist, double locationDist, double dir)
		{
			double q = Math.Min(1.0, Math.Abs(dir)*allIntersectionsValue);
			double p = (1.0-q)*anglePositionValue + q;
			return p*angleDist + (1.0-p)*locationDist;
		}

		private double computeDistance(int i, int j, double anglePositionValue, double allIntersectionsValue)
		{
			int w = optimizedArrayWork.GetLength(1);
			int h = optimizedArrayWork.GetLength(0);

			return
				  distVert (i-1, j  , w, h)
				+ distVert (i-1, j+1, w, h)
				+ distVert (i  , j  , w, h)
				+ distVert (i  , j+1, w, h)
				+ distVert (i+1, j  , w, h)
				+ distVert (i+1, j+1, w, h)
				+ distHoriz(i  , j-1, w, h)
				+ distHoriz(i+1, j-1, w, h)
				+ distHoriz(i  , j  , w, h)
				+ distHoriz(i+1, j  , w, h)
				+ distHoriz(i  , j+1, w, h)
				+ distHoriz(i+1, j+1, w, h);
		}

		private double clamp(double x)
		{
			if(x < 0.0)
			{
				return 0.0;
			}
			else if(x > 255.0)
			{
				return 255.0;
			}
			return x;
		}

		private void nextStep()
		{
			if(myAdjustMetricForm != null)
			{
				myAdjustMetricForm.GiveValues(out anglePositionValue, out allIntersectionsValue);
			}

			int h = optimizedArray.GetLength(0);
			int w = optimizedArray.GetLength(1);
			for(int j_ = 0; j_ < h; j_++)
			{
				for(int i_ = 0; i_ < w; i_++)
				{
					int i = myRandom.Next(w);
					if(i >= w) // bug in framework?!
					{
						i = w-1;
					}
					int j = myRandom.Next(h);
					if(j >= h) // bug in framework?!
					{
						j = h-1;
					}

					if(!boundaryArray[j, i])
					{
						continue;
					}

					int oldColor = optimizedArrayWork[j, i];

					const int randomAmount = 5;
					int r = myRandom.Next(randomAmount) + 1;
					if(myRandom.Next(2) == 1)
					{
						r = -r;
					}
					int newColor = (int) clamp(oldColor + r - 0.5);
					if(blackWhiteArray[j, i])
					{
						if(newColor < 128+16)
						{
							newColor = 128+16;
						}
					}
					else
					{
						if(newColor > 127-16)
						{
							newColor = 127-16;
						}
					}
					double oldDistance = computeDistance(i, j, anglePositionValue, allIntersectionsValue);
					optimizedArrayWork[j, i] = newColor;
					double newDistance = computeDistance(i, j, anglePositionValue, allIntersectionsValue);
					if(myRandom.NextDouble() > Math.Exp(-1e3*(newDistance-oldDistance)/(newDistance+oldDistance)))
					{
						optimizedArrayWork[j, i] = oldColor;
					}
				}
			}
		}

		private double interpolate(int[,] a, double x, double y, int w, int h)
		{
			double x1 = x - 0.5;
			double y1 = y - 0.5;
			double xf = Math.Floor(x1);
			double yf = Math.Floor(y1);
			int i = (int)xf;
			int j = (int)yf;
			int i1 = i+1;
			int j1 = j+1;
			double t = x1 - xf;
			double s = y1 - yf;

			// don't need to check if i >= w because then i1 would be, too. And so on.
			if(i < 0)
			{
				i = i1 = 0;
			}
			else if(i1 >= w)
			{
				i = i1 = w-1;
			}

			if(j < 0)
			{
				j = j1 = 0;
			}
			else if(j1 >= h)
			{
				j = j1 = h-1;
			}

			int a00 = a[j, i];
			int a01 = a[j1, i];
			int a10 = a[j, i1];
			int a11 = a[j1, i1];
			double ad =  a00 + (a01-a00)*s;
			return ad + (a10 + (a11-a10)*s - ad)*t;
		}

		internal void FillDisplayBitmap(Bitmap destination, int zoom, int baseX, int baseY, bool thresholded)
		{
			lock(this)
			{
				int srcW = optimizedArray.GetLength(1);
				int srcH = optimizedArray.GetLength(0);
				double scaling = downsampling*zoom;

				BitmapData destinationData = destination.LockBits(new Rectangle(0, 0, destination.Width, destination.Height), ImageLockMode.WriteOnly,
					PixelFormat.Format24bppRgb);

				unsafe
				{
					byte* destinationPointer = (byte*)destinationData.Scan0;
					int destinationStride = destinationData.Stride;
					int h = destination.Height;
					int w = destination.Width;

					for(int j = 0; j < h; j++)
					{
						double jRel = (j-baseY)/scaling;
						byte* outPointer = destinationPointer + j*destinationStride;
						for(int i = 0; i < w; i++)
						{
							double iRel = (i-baseX)/scaling;
							byte c = (byte)255;
							// Are we inside the image region?
							if(iRel < srcW && jRel < srcH)
							{
								double t = interpolate(optimizedArray, iRel, jRel, srcW, srcH);
								if(thresholded)
								{
									if( t >= 127.5)
									{
										c = (byte)255;
									}
									else
									{
										c = (byte)0;
									}
								}
								else
								{
									c = (byte)(t+0.5);
								}
							}
							*(outPointer++) = *(outPointer++) = *(outPointer++) = c;
						}
					}
				}
				destination.UnlockBits(destinationData);
			}
		}

		internal Size GetSize(int zoom)
		{
			return new Size(hires.Width*zoom, hires.Height*zoom);
		}

		internal void DrawSkeleton(Graphics g)
		{
			Pen pr = new Pen(Color.Red);
			Pen pb = new Pen(Color.Blue);
			Pen pg = new Pen(Color.Green);
			Pen pm = new Pen(Color.Magenta);
			Brush br = Brushes.Red;
			
			int h, w;
			lock(this)
			{
				h = optimizedArray.GetLength(0);
				w = optimizedArray.GetLength(1);
			}

			for(int j = 0; j < h; j++)
			{
				for(int i = 0; i < w; i++)
				{
					if(blackWhiteArray[j, i])
					{
						g.DrawEllipse(pr, i*downsampling + downsampling/2 - 1, j*downsampling + downsampling/2 - 1, 2, 2);		
					}
					else
					{
						g.FillEllipse(br, i*downsampling + downsampling/2 - 1, j*downsampling + downsampling/ 2 - 1, 3, 3);		
					}
					
					bool drawLarge = (selectedJ == j && selectedI == i);

					if(drawLarge)
					{
						if(selectedIsVertical)
						{
							g.DrawLine(pr, i*downsampling+downsampling/2, j*downsampling-downsampling/2, i*downsampling+downsampling/2, (j+1)*downsampling-downsampling/2);
						}
						else
						{
							g.DrawLine(pr, i*downsampling-downsampling/2, j*downsampling+downsampling/2, (i+1)*downsampling-downsampling/2, j*downsampling+downsampling/2);
						}
					}
					
					if(j >= 1 && blackWhiteArray[j-1, i] != blackWhiteArray[j, i])					
					{							
						double dir = dirVert[j, i];
						float x = 1.0f;
						float y = (float)dir;
						float scale = (drawLarge&&selectedIsVertical ? 8.0f : 4.0f)/(float)Math.Sqrt(x*x + y*y);
						x *= scale;
						y *= scale;

						int intsct = intersectVert[j, i];
						g.DrawLine(pb, i*downsampling + downsampling/2 - x, intsct - y, i*downsampling+downsampling/2 + x, intsct + y);
						g.DrawEllipse(pm, i*downsampling + downsampling/2 - 2, intsct - 2, 4, 4);
					}

					if(i >= 1 && blackWhiteArray[j, i] != blackWhiteArray[j, i-1])					
					{
						double dir = dirHoriz[j, i];
						float y = 1.0f;
						float x = (float)dir;
						float scale = (drawLarge&&!selectedIsVertical ? 8.0f : 4.0f)/(float)Math.Sqrt(x*x + y*y);
						x *= scale;
						y *= scale;
						
						int intsct = intersectHoriz[j, i];
						g.DrawLine(pg, intsct-x, j*downsampling + downsampling/2 - y, intsct + x, j*downsampling + downsampling/2 + y);
						g.DrawEllipse(pm, intsct - 2, j*downsampling-2 + downsampling/2, 4, 4);
					}
				}
			}
		}

		internal Bitmap HiresImage
		{
			get
			{
				return hires;
			}
		}

		internal void Select(int x, int y, int baseX, int baseY, int zoom)
		{
			int xLogical = (int)((x - baseX)/(double)zoom + 0.5);
			int yLogical = (int)((y - baseY)/(double)zoom + 0.5);
 
			int h, w;
			lock(this)
			{
				h = optimizedArray.GetLength(0);
				w = optimizedArray.GetLength(1);
			}

			for(int j = 0; j < h; j++)
			{
				for(int i = 0; i < w; i++)
				{
					Rectangle r = new Rectangle(i*downsampling+downsampling/2 - 2, j*downsampling-downsampling/2, 3, downsampling);
					if(j >= 1 && r.Contains(xLogical, yLogical))
					{
						selectedJ = j;
						selectedI = i;
						selectedIsVertical = true;
					}
					else
					{
						r = new Rectangle(i*downsampling-downsampling/2, j*downsampling+downsampling/2 - 2, downsampling, 3);
						if(i >= 1 && r.Contains(xLogical, yLogical))
						{
							selectedJ = j;
							selectedI = i;
							selectedIsVertical = false;
						}
					}
				}
			}
		}

		internal void ResetDirection()
		{
			if(selectedIsVertical)
			{
				dirVert[selectedJ, selectedI] = 0.0;
			}
			else
			{
				dirHoriz[selectedJ, selectedI] = 0.0;
			}
		}

		internal void MoveDown()
		{
			if(selectedIsVertical)
			{
				intersectVert[selectedJ, selectedI]++;
			}
		}

		internal void MoveLeft()
		{
			if(!selectedIsVertical)
			{
				intersectHoriz[selectedJ, selectedI]--;
			}
		}

		internal void ResetLocation()
		{
			if(selectedIsVertical)
			{
				intersectVert[selectedJ, selectedI] = downsampling*selectedJ;
			}
			else
			{
				intersectHoriz[selectedJ, selectedI] = downsampling*selectedI;
			}
		}

		internal void MoveRight()
		{
			if(!selectedIsVertical)
			{
				intersectHoriz[selectedJ, selectedI]++;
			}
		}

		internal void RotateCCW()
		{
			if(selectedIsVertical)
			{
				dirVert[selectedJ, selectedI] -= 0.1;
			}
			else
			{
				dirHoriz[selectedJ, selectedI] += 0.1;
			}
		}

		internal void MoveUp()
		{
			if(selectedIsVertical)
			{
				intersectVert[selectedJ, selectedI]--;
			}
		}

		internal void RotateCW()
		{
			if(selectedIsVertical)
			{
				dirVert[selectedJ, selectedI] += 0.1;
			}
			else
			{
				dirHoriz[selectedJ, selectedI] -= 0.1;
			}
		}

		private const int numberOfIterationsPerBatch = 50;

		private void optimize()
		{
			while(!stopRequested)
			{
				for(int i = 0; i< numberOfIterationsPerBatch; i++)
				{
					nextStep();
				}
				lock(this)
				{
					optimizedArray = (int[,])optimizedArrayWork.Clone();
				}
				if(myEditor != null)
				{
					myEditor.Dirty();
				}
				if(myRenderWindow != null)
				{
					myRenderWindow.SetTexture(optimizedArrayWork, HiresImage);
				}
			}
		}
	}
}