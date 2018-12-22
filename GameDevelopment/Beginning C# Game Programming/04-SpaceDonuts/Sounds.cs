using System;
namespace SpaceDonuts {
	[Flags]
	public enum Sounds {
		ShipAppear			= 0x00000001, //shipappear.wav
		ShipShield			= 0x00000002, //shield.wav
		ShipFire			= 0x00000004, //gunfire.wav
		ShipExplode			= 0x00000008, //bangbang.wav
		ShipThrust			= 0x00000010, //rev.wav
		ShipBrake			= 0x00000020, //skid.wav
		ShipBounce			= 0x00000040, //bounce.wav
		ShipHum				= 0x00000080, //hum.wav
		LevelStart			= 0x00000100, //level.wav
		DonutExplode		= 0x00000200, //d_bang.wav
		PyramidExplode		= 0x00000400, //p_bang.wav
		CubeExplode			= 0x00000800, //c_bang.wav
		SphereExplode		= 0x00001000, //s_bang.wav
	}
}
