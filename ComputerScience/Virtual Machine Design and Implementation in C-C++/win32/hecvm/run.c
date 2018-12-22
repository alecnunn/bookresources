/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+                                                                   +
+ run.c - execution engine                                          +
+                                                                   +
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ public prototypes                                                 + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void run(U1 dbg);

/*+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
+ definitions                                                       + 
+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void run(U1 dbg)
{	
	U2 tick;
	U8 start_debug_instr;

	tick=0;
	debug = dbg;  /*set global from dbgcmd.c*/

	interruptOn=TRUE;

	DBG_RUN0("initiating bytecode execution\n");

	while(RAM[ R[$IP] ] != HALT)
	{
		if(debug==TRUE){ readDbgCmd(); }
		start_debug_instr = (U8)R[$IP];

		switch(RAM[ R[$IP] ])
		{
			case LBI:{ HANDLE_LBI(); }break;
			case LWI:{ HANDLE_LWI(); }break;
			case LDI:{ HANDLE_LDI(); }break;
			case LQI:{ HANDLE_LQI(); }break;
			case LF1I:{ HANDLE_LF1I(); }break;
			case LF2I:{ HANDLE_LF2I(); }break;

			case LAD:{ HANDLE_LAD(); }break;
			case LAI:{ HANDLE_LAI(); }break;

			case LB:{ HANDLE_LB(); }break;
			case LW:{ HANDLE_LW(); }break;
			case LD:{ HANDLE_LD(); }break;
			case LQ:{ HANDLE_LQ(); }break;
			case LF1:{ HANDLE_LF1(); }break;
			case LF2:{ HANDLE_LF2(); }break;
		
			case SB:{ HANDLE_SB(); }break;
			case SW:{ HANDLE_SW(); }break;
			case SD:{ HANDLE_SD(); }break;
			case SQ:{ HANDLE_SQ(); }break;
			case SF1:{ HANDLE_SF1(); }break;
			case SF2:{ HANDLE_SF2(); }break;

			case PUSHB:{ HANDLE_PUSHB(); }break;
			case PUSHW:{ HANDLE_PUSHW(); }break;
			case PUSHD:{ HANDLE_PUSHD(); }break;
			case PUSHQ:{ HANDLE_PUSHQ(); }break;
			case PUSHF1:{ HANDLE_PUSHF1(); }break;
			case PUSHF2:{ HANDLE_PUSHF2(); }break;

			case POPB:{ HANDLE_POPB(); }break;
			case POPW:{ HANDLE_POPW(); }break;
			case POPD:{ HANDLE_POPD(); }break;
			case POPQ:{ HANDLE_POPQ(); }break;
			case POPF1:{ HANDLE_POPF1(); }break;
			case POPF2:{ HANDLE_POPF2(); }break;

			case MOV:{ HANDLE_MOV(); }break;
			case MOVF:{ HANDLE_MOVF(); }break;
			case MOVD:{ HANDLE_MOVD(); }break;
			
			case JMP:{ HANDLE_JMP();  }break;
			case JE:{ HANDLE_JE();  }break;
			case JNE:{ HANDLE_JNE(); }break;
			case SLT:{ HANDLE_SLT(); }break;
			case INT:{ HANDLE_INT(); }break;
			case DI:{ HANDLE_DI(); }break;
			case EI:{ HANDLE_EI(); }break;
			case NOP:{ HANDLE_NOP(); }break;

			case AND:{ HANDLE_AND(); }break;
			case OR:{ HANDLE_OR(); }break;
			case XOR:{ HANDLE_XOR(); }break;
			case NOT:{ HANDLE_NOT(); }break;
			case BT:{ handleBT(); }break;
			case BS:{ handleBS(); }break;

			case SRA:{ HANDLE_SRA(); }break;
			case SRL:{ HANDLE_SRL(); }break;
			case SL:{ HANDLE_SL(); }break;

			case ADD:{ HANDLE_ADD(); }break;
			case SUB:{ HANDLE_SUB(); }break;
			case MULT:{ HANDLE_MULT(); }break;
			case DIV:{ HANDLE_DIV(); }break;

			case CAST_IF:{ HANDLE_CAST_IF(); }break;
			case CAST_ID:{ HANDLE_CAST_ID(); }break;
			case CAST_FI:{ HANDLE_CAST_FI(); }break;
			case CAST_FD:{ HANDLE_CAST_FD(); }break;
			case CAST_DI:{ HANDLE_CAST_DI(); }break;
			case CAST_DF:{ HANDLE_CAST_DF(); }break;

			case FADD:{ HANDLE_FADD(); }break;
			case FSUB:{ HANDLE_FSUB(); }break;
			case FMULT:{ HANDLE_FMULT(); }break;
			case FDIV:{ HANDLE_FDIV(); }break;
			case FSLT:{ HANDLE_FSLT(); }break;

			case DADD:{ HANDLE_DADD(); }break;
			case DSUB:{ HANDLE_DSUB(); }break;
			case DMULT:{ HANDLE_DMULT(); }break;
			case DDIV:{ HANDLE_DDIV(); }break;
			case DSLT:{ HANDLE_DSLT(); }break;

			default:
			{ 
				xmlBegin();
				fprintf(errPtr,"run(): ");
				fprintf(errPtr,"bad instruction (%d) ",RAM[R[$IP]]);
				fprintf(errPtr,"at address = ");
				fpU8(errPtr,(U8)R[$IP]);
				fprintf(errPtr,"\n");
				xmlEnd();

				ERROR_LVL1("run(): fatal error");

			}break;
		
		}/*end switch*/
	
		if(debug==TRUE){ printDbgInstr(start_debug_instr); }

		tick++;
		if(tick==65535){ tick = 0; }

	}/*end while*/

	if(debug==TRUE){ readDbgCmd(); }
	DBG_RUN0("HALT instruction executed\n");
	return;

}/*end run*/