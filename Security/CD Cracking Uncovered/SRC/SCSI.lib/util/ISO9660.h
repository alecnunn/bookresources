#include <windows.h>

#define PVD_sector 0x10
#pragma pack(1)

struct Directory_Record
{
	/*  000  */ BYTE	LEN_DR;
	/*  001  */ BYTE	Extended_Attribute_Record_Length;
	/*  002  */ DWORD	Location_Extent_A;
	/*  006  */ DWORD	Location_Extent_B;
	/*  010  */ DWORD	Data_Length_A;
	/*  014  */ DWORD	Data_Length_B;
	/*  018  */ BYTE	Recording_Date_Time[7];
	/*  025  */ BYTE	File_Flags;
	/*  026  */ BYTE	File_Unit_Size;
	/*  027  */ BYTE	Interleave_Gap_Size;
	/*  028  */ DWORD	Volume_Sequence_Number;
	/*  032  */ BYTE	LEN_FI;
	/*  033  */ BYTE	File_Identifier[34];
};

struct Primary_Volume_Descriptor
{
	/*  001  */ BYTE	Volume_Descriptor_Type;
	/*  002  */ BYTE	Standard_Identifier[5];
	/*  007  */ BYTE	Volume_Descriptor_Version;
	/*  008  */ BYTE	Unused_Field_1;
	/*  009  */ BYTE	System_Identifier[32];
	/*  041  */ BYTE	Volume_Identifier[32];
	/*  073  */ BYTE	Unused_Field_2[8];
	/*  081  */ DWORD	Volume_Space_Size_A;
	/*  085  */ DWORD	Volume_Space_Size_B;
	/*  089  */ BYTE	Unused_Field_3[32];
	/*  121  */ DWORD	Volume_Set_Size;
	/*  125  */ DWORD	Volume_Sequence_Number;
	/*  129  */ DWORD	Logical_Block_Size;
	/*  133  */ DWORD	Path_Table_Size_A;
	/*  137  */ DWORD	Path_Table_Size_B;
	/*  141  */ DWORD	Location_Occurrence_Type_L_Path_Table;
	/*  145  */ DWORD	Location_Optional_Occurrence_Type_L_Path_Table;
	/*  149  */ DWORD	Location_Occurrence_Type_M_Path_Table;
	/*  153  */ DWORD	Location_Optional_Occurrence_Type_M_Path_Table;
	/*  157  */ BYTE	Directory_Record_Root_Directory[34];
	/*  191  */ BYTE	Volume_Set_Identifier[128];
	/*  319  */ BYTE	Publisher_Identifier[128];
	/*  447  */ BYTE	Data_Preparer_Identifier[128];
	/*  575  */ BYTE	Application_Identifier[128];
	/*  703  */ BYTE	Copyright_File_Identifier[37];
	/*  740  */ BYTE	Abstract_File_Identifier[37];
	/*  777  */ BYTE	Bibliographic_File_Identifier[37];
	/*  814  */ BYTE	Volume_Creation_Date_Time[17];
	/*  831  */ BYTE	Volume_Modification_Date_Time[17];
	/*  848  */ BYTE	Volume_Expiration_Date_Time[17];
	/*  865  */ BYTE	Volume_Effective_Date_Time[17];
	/*  882  */ BYTE	File_Structure_Version;
	/*  883  */ BYTE	Reserved_1;
	/*  884  */ BYTE	Application_Use[512];
	/*  1396 */ BYTE	Reserved_2[653];
};

