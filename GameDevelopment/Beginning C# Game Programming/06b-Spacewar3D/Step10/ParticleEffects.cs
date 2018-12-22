using System;
using System.Windows.Forms;
using Microsoft.DirectX;
using Microsoft.DirectX.Direct3D;
using System.Drawing;
public class ParticleEffects {
	public struct PointVertex {
		public Vector3 v;
		public int color;
		public static readonly VertexFormats Format =   VertexFormats.Position | VertexFormats.Diffuse;
	};

	/// <summary>
	/// Global data for the particles
	/// </summary>
	public struct Particle {
		public Vector3 positionVector;       // Current position
		public Vector3 velocityVector;       // Current velocity
		public Vector3 initialPosition;      // Initial position
		public Vector3 initialVelocity;      // Initial velocity
		public float creationTime;     // Time of creation
		public System.Drawing.Color diffuseColor; // Initial diffuse color
		public System.Drawing.Color fadeColor;    // Faded diffuse color
		public float fadeProgression;      // Fade progression
	};


	private float time = 0.0f;
	private const int bufferChunkSize = 8192; 
	private const int maxBufferSize = bufferChunkSize*4; //supports 4 bufferChunkSize buffers before discarding
	private int baseParticle = maxBufferSize;
	private int particles = 0;
	private const int particlesLimit = 2048;
	private Vector3 location;
	private Vector3 offset;
	private System.Collections.ArrayList particlesList = new System.Collections.ArrayList();
	private System.Collections.ArrayList freeParticles = new System.Collections.ArrayList();
	private System.Random rand = new System.Random();
	public Vector3 EmitterLocation {get { return location; } }
	public Vector3 EmitterOffset { get { return offset; } set { offset = value; } }
	// Geometry
	private VertexBuffer vertexBuffer = null;
	private Texture particleTexture;
	public Texture ParticleTexture { set { particleTexture = value; } }
	private Device device = null;

	/// <summary>
	/// VaporTrail constructor
	/// </summary>
	public ParticleEffects(Device device) {
		this.device = device;
		if (device != null) {
			device.DeviceLost += new System.EventHandler(this.InvalidateDeviceObjects);
			device.Disposing += new System.EventHandler(this.InvalidateDeviceObjects);
			device.DeviceReset += new System.EventHandler(this.RestoreDeviceObjects);
		}
		RestoreDeviceObjects(device, null);
	}
        
	public void InvalidateDeviceObjects(object sender, EventArgs e) {
		if (vertexBuffer != null)
			vertexBuffer.Dispose();
		vertexBuffer = null;
	}
        
	/// <summary>
	/// Restores the device objects
	/// </summary>
	public void RestoreDeviceObjects(object sender, EventArgs e) {
		Device device = (Device)sender;
		// Create a vertex buffer for the particle system.  The size of this buffer
		// does not relate to the number of particles that exist.  Rather, the
		// buffer is used as a communication channel with the device.. we fill in 
		// a bit, and tell the device to draw.  While the device is drawing, we
		// fill in the next bit using NOOVERWRITE.  We continue doing this until 
		// we run out of vertex buffer space, and are forced to discard the buffer
		// and start over at the beginning.
		vertexBuffer = new VertexBuffer(typeof(PointVertex), maxBufferSize, device,  Usage.Dynamic | Usage.WriteOnly | Usage.Points, PointVertex.Format, Pool.Default);
	}
          
	/// <summary>
	/// Updates the thrust effects (vapor trail)
	/// </summary>
	public void UpdateThrustEffect(float ElapsedTime, int NumParticlesToEmit,
		System.Drawing.Color EmitColor,System.Drawing.Color FadeColor, Vector3 Position) {
		time += ElapsedTime;
		location = Position;
		for (int i = particlesList.Count-1; i >= 0; i--) {
			Particle p = (Particle)particlesList[i];
			// Calculate new position
			float fT = time - p.creationTime;
			p.fadeProgression -= ElapsedTime * 0.60f;
			p.positionVector   = p.initialVelocity * fT + p.initialPosition;
			p.velocityVector.Z = 0;
			if (p.fadeProgression < 0.0f)
				p.fadeProgression = 0.0f;
			// Kill old particles
			if (p.fadeProgression <= 0.0f) {
				// Kill particle
				freeParticles.Add(p);
				particlesList.RemoveAt(i);
				particles--;
			}
			else
				particlesList[i] = p;
		}
		// Emit new particles
		int particlesEmit = particles + NumParticlesToEmit;
		while(particles < particlesLimit && particles < particlesEmit) {
			Particle particle;
			if (freeParticles.Count > 0) {
				particle = (Particle)freeParticles[0];
				freeParticles.RemoveAt(0);
			}
			else {
				particle = new Particle();
			}
			// Emit new particle
			particle.initialPosition = Position + offset;
			particle.positionVector = particle.initialPosition;
			particle.velocityVector = particle.initialVelocity;
			particle.diffuseColor = EmitColor;
			particle.fadeColor    = FadeColor;
			particle.fadeProgression      = 1.0f;
			particle.creationTime     = time;
			particlesList.Add(particle);
			particles++;
		}
	}

	/// <summary>
	/// Renders the scene
	/// </summary>
	public void Render() {
		// Set the render states for using point sprites
		device.RenderState.ZBufferWriteEnable = false;
		device.RenderState.AlphaBlendEnable = true;
		device.RenderState.SourceBlend = Blend.One;
		device.RenderState.DestinationBlend = Blend.One;
		bool lightEnabled = device.RenderState.Lighting;
		device.RenderState.Lighting = false;
		device.SetTexture(0, particleTexture);
		device.Transform.World = Matrix.Identity;
		device.RenderState.PointSpriteEnable = true;
		device.RenderState.PointScaleEnable = true;
		device.RenderState.PointSize = 1.0f;
		device.RenderState.PointScaleA = 0f;
		device.RenderState.PointScaleB = 1.0f;
		device.RenderState.PointScaleC = 1.0f;
		// Set up the vertex buffer to be rendered
		device.SetStreamSource(0, vertexBuffer, 0);
		device.VertexFormat = PointVertex.Format;
		PointVertex[] vertices = null;
		int numParticlesToRender = 0;
		// Lock the vertex buffer.  We fill the vertex buffer in small
		// chunks, using LockFlags.NoOverWrite.  When we are done filling
		// each chunk, we call DrawPrim, and lock the next chunk.  When
		// we run out of space in the vertex buffer, we start over at
		// the beginning, using LockFlags.Discard.
		baseParticle += bufferChunkSize;
		if (baseParticle >= maxBufferSize)
			baseParticle = 0;
		int count = 0;
		vertices = (PointVertex[])vertexBuffer.Lock(baseParticle * DXHelp.GetTypeSize(typeof(PointVertex)), typeof(PointVertex), (baseParticle != 0) ? LockFlags.NoOverwrite : LockFlags.Discard, bufferChunkSize);
		foreach(Particle p in particlesList) {
			Vector3 vPos = p.positionVector;
			Vector3 velocityVector = p.velocityVector;
			float LengthSq = velocityVector.LengthSq();
			uint steps;
			if (LengthSq < 1.0f)        steps = 2;
			else if (LengthSq <  4.00f) steps = 3;
			else if (LengthSq <  9.00f) steps = 4;
			else if (LengthSq < 12.25f) steps = 5;
			else if (LengthSq < 16.00f) steps = 6;
			else if (LengthSq < 20.25f) steps = 7;
			else                          steps = 8;
			velocityVector *= -0.01f / (float)steps;
			System.Drawing.Color diffuse = ColorOperator.Lerp(p.fadeColor, p.diffuseColor, p.fadeProgression);
			// Render each particle a bunch of times to get a blurring effect
			for (int i = 0; i < steps; i++) {
				vertices[count].v     = vPos;
				vertices[count].color = diffuse.ToArgb();
				count++;
				if (++numParticlesToRender == bufferChunkSize) {
					// Done filling this chunk of the vertex buffer.  Lets unlock and
					// draw this portion so we can begin filling the next chunk.
					vertexBuffer.Unlock();
					device.DrawPrimitives(PrimitiveType.PointList, baseParticle, numParticlesToRender);
					// Lock the next chunk of the vertex buffer.  If we are at the 
					// end of the vertex buffer, LockFlags.Discard the vertex buffer and start
					// at the beginning.  Otherwise, specify LockFlags.NoOverWrite, so we can
					// continue filling the VB while the previous chunk is drawing.
					baseParticle += bufferChunkSize;
					if (baseParticle >= maxBufferSize)
						baseParticle = 0;
					vertices = (PointVertex[])vertexBuffer.Lock(baseParticle * DXHelp.GetTypeSize(typeof(PointVertex)), typeof(PointVertex), (baseParticle != 0) ? LockFlags.NoOverwrite : LockFlags.Discard, bufferChunkSize);
					count = 0;
					numParticlesToRender = 0;
				}
				vPos += velocityVector;
			}
		}

		// Unlock the vertex buffer
		vertexBuffer.Unlock();
		// Render any remaining particles
		if (numParticlesToRender > 0)
			device.DrawPrimitives(PrimitiveType.PointList, baseParticle, numParticlesToRender);
		// Reset render states
		device.RenderState.PointSpriteEnable = false;
		device.RenderState.PointScaleEnable = false;
		device.RenderState.Lighting = lightEnabled;
		device.RenderState.ZBufferWriteEnable = true;
		device.RenderState.AlphaBlendEnable = false;
	}

	public void Dispose() {
		if (vertexBuffer != null)
			vertexBuffer.Dispose();
	}
}