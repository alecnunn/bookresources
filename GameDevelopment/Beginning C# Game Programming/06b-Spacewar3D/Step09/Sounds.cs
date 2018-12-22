using System;


[Flags]
public enum Sounds {
	ShipAppear			= 0x00000001,
	ShipHyper			= 0x00000002,
	ShipFire				= 0x00000004,
	ShipExplode			= 0x00000008,
	ShipThrust			= 0x00000010,
	OtherShipThrust  = 0x00000020,
	OtherShipExplode = 0x00000040,
	OtherShipFire		= 0x00000080,
	OtherShipAppear  = 0x00000100,
	Taunt					= 0x00000200,
	Dude1				= 0x00000400,
	Dude2				= 0x00000800,
	Dude3				= 0x00001000,
	Dude4				= 0x00002000,
	Dude5				= 0x00004000,
}
