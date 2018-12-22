#include<stdio.h>
#ifdef WINDOWS_32
#include<win32.c>		
#endif
#ifdef LINUX_x86
#include<linux.c>		
#endif
#include<iset.c>
#include<exenv.c>

void main(int argc, char *argv[])
{
	FILE *fptr;
	U2 i;
	U1 buffer[1024];

	fptr = fopen("rawbin.RUN","wb");
	if(fptr==NULL){ printf("cannot open file\n"); }

	/*
		Recall the header section elements
			U2 magic		
			U8 szSymTbl	
			U8 szStrTbl	
		   +U8 szByteCode
		   --------------
		   26 bytes total

		We will exclude symbol table info, so the
		szSymTbl and szStrTbl will be zeroed

		if add commands, will need to modify
		i) szByteCode field in header = last_index-25 
		ii) i loop range in for loop [0,last_index]
	
	*/

	/*start header-------------------------------------------------*/

	/* magic number */
	buffer[0]=0xDE; buffer[1]=0xED;

	/* szSymTbl */
	buffer[2]=0; buffer[3]=0; buffer[4]=0; buffer[5]=0;
	buffer[6]=0; buffer[7]=0; buffer[8]=0; buffer[9]=0;

	/* szStrTbl */
	buffer[10]=0; buffer[11]=0; buffer[12]=0; buffer[13]=0;
	buffer[14]=0; buffer[15]=0; buffer[16]=0; buffer[17]=0;

	/* szByteCode (328) */
	buffer[18]=0; buffer[19]=0; buffer[20]=0; buffer[21]=0;
	buffer[22]=0; buffer[23]=0; buffer[24]=1; buffer[25]=72;
	
	/*end header---------------------------------------------------*/

	/*bytecode ----------------------------------------------------*/

	/*LBI $R1, -25*/
	buffer[26]=LBI;buffer[27]=$R1;buffer[28]=-25;

	/*LWI $R2, -150*/
	buffer[29]=LWI;buffer[30]=$R2;buffer[31]=255;buffer[32]=106;

	/*LDI $R3, 10,234,768 */
	buffer[33]=LDI;buffer[34]=$R3;
	buffer[35]=0;buffer[36]=156;buffer[37]=43;buffer[38]=144;

	/*LQI $R5, -2,147,483,649  */
	buffer[39]=LQI;buffer[40]=$R5;
	buffer[41]=255;buffer[42]=255;buffer[43]=255;buffer[44]=255;
	buffer[45]=127;buffer[46]=255;buffer[47]=255;buffer[48]=255;
	
	/*LF1I $F1, 23.45e4 */
	buffer[49]=LF1I;buffer[50]=$F1;
	buffer[51]=72;buffer[52]=101;buffer[53]=1;buffer[54]=0;

	/*LF2I $D1, -100.2e-4*/
	buffer[55]=LF2I;buffer[56]=$D1;
	buffer[57]=191;buffer[58]=132;buffer[59]=133;buffer[60]=93;
	buffer[61]=162;buffer[62]=114;buffer[63]=134;buffer[64]=47;
	
	/*CAST_IF $R7, $F1*/
	buffer[65]=CAST_IF;buffer[66]=$R7;buffer[67]=$F1;

	/*CAST_ID $R8, $D1*/
	buffer[68]=CAST_ID;buffer[69]=$R8;buffer[70]=$D1;

	/*CAST_FI $F2, $R1*/
	buffer[71]=CAST_FI;buffer[72]=$F2;buffer[73]=$R1;

	/*CAST_FD $F3, $D1*/
	buffer[74]=CAST_FD;buffer[75]=$F3;buffer[76]=$D1;

	/*CAST_DI $D2, $R5*/
	buffer[77]=CAST_DI;buffer[78]=$D2;buffer[79]=$R5;

	/*CAST_DF $D3, $F1*/
	buffer[80]=CAST_DF;buffer[81]=$D3;buffer[82]=$F1;

	/*LAD $R9,400*/
	buffer[83]=LAD;buffer[84]=$R9;
	buffer[85]=0;buffer[86]=0;buffer[87]=0;buffer[88]=0;
	buffer[89]=0;buffer[90]=0;buffer[91]=1;buffer[92]=144;

	/*LAI $R10,$R9,20*/
	buffer[93]=LAI;buffer[94]=$R10;buffer[95]=$R9;
	buffer[96]=0;buffer[97]=0;buffer[98]=0;buffer[99]=0;
	buffer[100]=0;buffer[101]=0;buffer[102]=0;buffer[103]=20;
	
	/*SB $R3,$R9*/
	buffer[104]=SB;buffer[105]=$R3;buffer[106]=$R9;

	/*SW $R3,$R9*/
	buffer[107]=SW;buffer[108]=$R3;buffer[109]=$R9;

	/*SD $R3,$R9*/
	buffer[110]=SD;buffer[111]=$R3;buffer[112]=$R9;

	/*SQ $R3,$R9*/
	buffer[113]=SQ;buffer[114]=$R3;buffer[115]=$R9;

	/*SF1 $F1,$R9*/
	buffer[116]=SF1;buffer[117]=$F1;buffer[118]=$R9;

	/*SF2 $D1,$R9*/
	buffer[119]=SF2;buffer[120]=$D1;buffer[121]=$R9;

	/*LB $R1,$R9*/
	buffer[122]=LB;buffer[123]=$R1;buffer[124]=$R9;

	/*LW $R1,$R9*/
	buffer[125]=LW;buffer[126]=$R1;buffer[127]=$R9;

	/*LD $R1,$R9*/
	buffer[128]=LD;buffer[129]=$R1;buffer[130]=$R9;

	/*LQ $R1,$R9*/
	buffer[131]=LQ;buffer[132]=$R1;buffer[133]=$R9;

	/*LF1 $F2,$R9*/
	buffer[134]=LF1;buffer[135]=$F2;buffer[136]=$R9;

	/*LF2 $D2,$R9*/
	buffer[137]=LF2;buffer[138]=$D2;buffer[139]=$R9;

	/*PUSHB $R1*/
	buffer[140]=PUSHB;buffer[141]=$R1;

	/*PUSHW $R2*/
	buffer[142]=PUSHW;buffer[143]=$R2;

	/*PUSHD $R3*/
	buffer[144]=PUSHD;buffer[145]=$R3;

	/*PUSHQ $R4*/
	buffer[146]=PUSHQ;buffer[147]=$R4;

	/*PUSHF1 $F1*/
	buffer[148]=PUSHF1;buffer[149]=$F1;

	/*PUSHF2 $D1*/
	buffer[150]=PUSHF2;buffer[151]=$D1;

	/*POPB $R1*/
	buffer[152]=POPB;buffer[153]=$R1;

	/*POPW $R1*/
	buffer[154]=POPW;buffer[155]=$R1;

	/*POPD $R1*/
	buffer[156]=POPD;buffer[157]=$R1;

	/*POPQ $R1*/
	buffer[158]=POPQ;buffer[159]=$R1;

	/*POPF1 $F2*/
	buffer[160]=POPF1;buffer[161]=$F2;

	/*POPF2 $D2*/
	buffer[162]=POPF2;buffer[163]=$D2;

	/*MOV $R3,$R4*/
	buffer[164]=MOV;buffer[165]=$R3;buffer[166]=$R4;

	/*MOVF $F3,$F1*/
	buffer[167]=MOVF;buffer[168]=$F3;buffer[169]=$F1;

	/*MOVD $D3,$D1*/
	buffer[170]=MOVD;buffer[171]=$D3;buffer[172]=$D1;

	/*LWI $R1,*/
	buffer[173]=LWI;buffer[174]=$R1;
	buffer[175]=0;buffer[176]=155; //181-26=155

	/*JMP $R1 */
	buffer[177]=JMP;buffer[178]=$R1;

	/*NOP NOP NOP*/
	buffer[179]=NOP;buffer[180]=NOP;

	/*LBI $R1,10*/
	buffer[181]=LBI;buffer[182]=$R1;buffer[183]=10;

	/*LBI $R2,10*/
	buffer[184]=LBI;buffer[185]=$R2;buffer[186]=10;

	/*LWI $R3,170*/
	buffer[187]=LWI;buffer[188]=$R3;
	buffer[189]=0;buffer[190]=170; //196-26=170

	/*JE $R1, $R2, $R3 */
	buffer[191]=JE;buffer[192]=$R1;buffer[193]=$R2;buffer[194]=$R3;

	/*NOP NOP NOP*/
	buffer[195]=NOP;

	/*JNE $R1, $R2, $R3 */
	buffer[196]=JNE;buffer[197]=$R1;buffer[198]=$R2;buffer[199]=$R3;

	/*SLT $R1,$R2,$R3*/
	buffer[200]=SLT;buffer[201]=$R1;buffer[202]=$R2;buffer[203]=$R3;

	/*LBI $R1,2*/
	buffer[204]=LBI;buffer[205]=$R1;buffer[206]=2;

	/*LBI $R2,8*/
	buffer[207]=LBI;buffer[208]=$R2;buffer[209]=8;

	/*LBI $R3,4*/
	buffer[210]=LBI;buffer[211]=$R3;buffer[212]=4;

	/*AND $R3,$R1,$R2*/
	buffer[213]=AND;buffer[214]=$R3;buffer[215]=$R1;buffer[216]=$R2;

	/*OR  $R3,$R1,$R2*/
	buffer[217]=OR;buffer[218]=$R3;buffer[219]=$R1;buffer[220]=$R2;

	/*XOR $R3,$R1,$R2*/
	buffer[221]=XOR;buffer[222]=$R3;buffer[223]=$R1;buffer[224]=$R2;

	/*BT $R1,$R2,$R3*/
	buffer[225]=BT;buffer[226]=$R1;buffer[227]=$R2;buffer[228]=$R3;

	/*BS $R1,$R2*/
	buffer[229]=BS;buffer[230]=$R1;buffer[231]=$R2;

	/*SRA $R1,$R2,$R3*/
	buffer[232]=SRA;buffer[233]=$R1;buffer[234]=$R2;buffer[235]=$R3;

	/*SRL $R1,$R2,$R3*/
	buffer[236]=SRL;buffer[237]=$R1;buffer[238]=$R2;buffer[239]=$R3;

	/*SL $R1,$R2,$R3*/
	buffer[240]=SL;buffer[241]=$R1;buffer[242]=$R2;buffer[243]=$R3;

	/*LBI $R2,-8*/
	buffer[244]=LBI;buffer[245]=$R2;buffer[246]=-8;

	/*LBI $R3,4*/
	buffer[247]=LBI;buffer[248]=$R3;buffer[249]=4;

	/*ADD $R4,$R2,$R3*/
	buffer[250]=ADD;buffer[251]=$R4;buffer[252]=$R2;buffer[253]=$R3;

	/*SUB $R4,$R2,$R3*/
	buffer[254]=SUB;buffer[255]=$R4;buffer[256]=$R2;buffer[257]=$R3;

	/*MULT $R4,$R2,$R3*/
	buffer[258]=MULT;buffer[259]=$R4;buffer[260]=$R2;buffer[261]=$R3;

	/*DIV $R1,$R4,$R2,$R3*/
	buffer[262]=DIV;buffer[263]=$R1;buffer[264]=$R4;buffer[265]=$R2;
	buffer[266]=$R3;

	/*LF1I $F2,-8.0e2*/
	buffer[267]=LF1I;buffer[268]=$F2;
	buffer[269]=196;buffer[270]=72;buffer[271]=0;buffer[272]=0;

	/*LF1I $F3,1.2e2*/
	buffer[273]=LF1I;buffer[274]=$F3;
	buffer[275]=66;buffer[276]=240;buffer[277]=0;buffer[278]=0;

	/*FADD $F4,$F2,$F3*/
	buffer[279]=FADD;buffer[280]=$F4;buffer[281]=$F2;buffer[282]=$F3;

	/*FSUB $F4,$F2,$F3*/
	buffer[283]=FSUB;buffer[284]=$F4;buffer[285]=$F2;buffer[286]=$F3;

	/*FMULT $F4,$F2,$F3*/
	buffer[287]=FMULT;buffer[288]=$F4;buffer[289]=$F2;buffer[290]=$F3;

	/*FDIV $F4,$F2,$F3*/
	buffer[291]=FDIV;buffer[292]=$F4;buffer[293]=$F2;buffer[294]=$F3;

	/*FSLT $F4,$F2,$F3*/
	buffer[295]=FSLT;buffer[296]=$F4;buffer[297]=$F2;buffer[298]=$F3;

	/*LF2I $D2,-8.0e2*/
	buffer[299]=LF2I;buffer[300]=$D2;
	buffer[301]=192;buffer[302]=137;buffer[303]=0;buffer[304]=0;
	buffer[305]=0;buffer[306]=0;buffer[307]=0;buffer[308]=0;

	/*LF2I $D3,1.2e2*/
	buffer[309]=LF2I;buffer[310]=$D3;
	buffer[311]=64;buffer[312]=94;buffer[313]=0;buffer[314]=0;
	buffer[315]=0;buffer[316]=0;buffer[317]=0;buffer[318]=0;

	/*DADD $D4,$D2,$D3*/
	buffer[319]=DADD;buffer[320]=$D4;buffer[321]=$D2;buffer[322]=$D3;

	/*DSUB $D4,$D2,$D3*/
	buffer[323]=DSUB;buffer[324]=$D4;buffer[325]=$D2;buffer[326]=$D3;

	/*DMULT $D4,$D2,$D3*/
	buffer[327]=DMULT;buffer[328]=$D4;buffer[329]=$D2;buffer[330]=$D3;

	/*DDIV $D4,$D2,$D3*/
	buffer[331]=DDIV;buffer[332]=$D4;buffer[333]=$D2;buffer[334]=$D3;

	/*DSLT $D4,$D2,$D3*/
	buffer[335]=DSLT;buffer[336]=$D4;buffer[337]=$D2;buffer[338]=$D3;

	/*LF2I $D1, 4.56789e2*/
	buffer[339]=LF2I;buffer[340]=$D1;
	buffer[341]=64;buffer[342]=124;buffer[343]=140;buffer[344]=159;
	buffer[345]=190;buffer[346]=118;buffer[347]=200;buffer[348]=180;

	/*PUSHF2 $D1*/
	buffer[349]=PUSHF2;buffer[350]=$D1;

	/*POPF2 $D2*/
	buffer[351]=POPF2;buffer[352]=$D2;

	buffer[353]=HALT;

	for(i=0;i<354;i++){ fputc(buffer[i],fptr); }

	if(fclose(fptr)){ printf("cannot close test executable\n"); }
	return;

}/*end main*/