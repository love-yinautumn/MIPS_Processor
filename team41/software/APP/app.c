#define	true 1 
#define BUFFER		(*((volatile unsigned int*)0x1FFF000C))
#define	In_INDEX	(*((volatile unsigned int*)0x1FFF0004))
#define	OUT_INDEX	(*((volatile unsigned int*)0x1FFF0008))
#define	STATE		(*((volatile unsigned long*)0x1FFF0020))
#define	TIME_READ	(*((volatile unsigned int*)0x80000010))
#define	TIME_RESET	(*((volatile unsigned int*)0x80000018))
#define	TEMP		(*((volatile unsigned int*)0x1FFF0030))
#define	BENCHMARK	100000000

void _PUSH_TO_BUFFER(char character);
void	_PUSH_CYCLES(char Sel, unsigned int cycles);
void	_MOD();					//Returns X%Y
void	_DIV();

unsigned int MOD( unsigned int  x,  unsigned int  y);					//Returns X%Y
unsigned int DIV( unsigned int  x,  unsigned int  y);

unsigned int Vplusone( unsigned int A);
unsigned int Rplusone( unsigned int A);
void case_r();
void case_v();
void case_R();
void case_V();
 int main( void){


	asm( "lui	$t1,	0x1FFF" 	);
	asm( "sw	$0, 	0x4($t1)"	);		//zero out In_INDEX
	asm( "sw	$0, 	0x8($t1)"	);		//zero out Out_INDEX
	asm( "sw	$0, 	0x20($t1)"	);		//zero out STATE
	asm( "addiu	$t0, 	$0, 1");
	asm( "sw	$t0, 	0x24($t1)"	);		//Enable the Timer_state
	asm( "sw	$0, 	0x28($t1)"	);		//zero out mm
	asm( "sw	$0, 	0x2c($t1)"	);		//zero out ss
	asm( "sw	$0, 	0x0($t1)"	);		//zero out RTC
	asm( "li	$t0,	50000000 " 	);		//<----THIS SHOULD BE 50M !!!!!				 <---------------------------------
	asm( "mtc0	$t0,	$11"		);		//Compare = 50_000_0000
	asm( "mtc0  $0,		$9" 		);		/////zero out count register
	_PUSH_TO_BUFFER('\n');
	asm( "lui	$t0,		0xFFFF" );
	asm( "ori	$t0,	$t0,	0xFFFF" );
	asm( "mtc0	$0,	$13"		);		//Cause	= 0x0000_0000
	asm( "mtc0	$t0,	$12" 		);		//Status= 0xFFFF_FFFF
		
	
	asm( "START_MAIN:"	);

	while ( true )
	{

		switch( STATE )
		{
		
		
		
			case 'R':
				
				TIME_RESET = 0;
				asm( "begin_case_R: " );
				for (register unsigned int k = 0; k < BENCHMARK; k = Rplusone(k) ) {

				};		
				_PUSH_CYCLES( 'R', TIME_READ);
				asm ("li	$t3, 0x1FFF0020");
				asm ("sw	$0,	0($t3)"); 
				break;
				
			case 'V':
				
				asm( "begin_case_V: " );
				TIME_RESET = 0;
				for (TEMP = 0; TEMP < BENCHMARK; TEMP = Vplusone(TEMP) ) {

				};	

				//case_V();
				_PUSH_CYCLES( 'V', TIME_READ);
				asm ("li	$t3, 0x1FFF0020");
				asm ("sw	$0,	0($t3)"); 
				break;
				
			case 'r':
				
				//Restore the timer;
				asm( "begin_case_r: " );
				TIME_RESET = 0;
				
				case_r();

				_PUSH_CYCLES( 'r', TIME_READ);
				asm ("li	$t3, 0x1FFF0020");
				asm ("sw	$0,	0($t3)"); 	
				break;
				
			case 'v':
				
				asm( "begin_case_v: " );
				TIME_RESET = 0;
				case_v();
				_PUSH_CYCLES( 'v', TIME_READ);
				asm ("li	$t3, 0x1FFF0020");
				asm ("sw	$0,	0($t3)"); 
				break;

			

				
			case 'd':
			case 'e':
			default:	break;

			
		};
\
	};
	asm("addiu	$sp,	$sp, 24");
	return 0;
}

void case_r()
{
	asm("sw	$v0,	0($sp)");
	asm("sw $v1,	4($sp)");
	asm("li	$v1,	100000000");
	asm("li	$v0,	0");
	asm("_DECR_r:");
	asm("bne	$v0,	$v1,	_DECR_r");
	asm("addiu	$v0,	$v0,	1");
	asm("lw	$v0,	0($sp)");
	asm("lw $v1,	4($sp)");
	asm("nop");
}




void case_v()
{
	asm("sw	$v0,	0($sp)");
	asm("sw $v1,	4($sp)");
	asm("li	$v1,	100000000");
	asm("li	$v0,	0");
	asm("li	$t4,	0x1FFF0030");
	asm("li		$v0,	0");
	asm("sw		$v0,	0($t4)");
	asm("_DECR_v:");
	asm("lw $v0,	0($t4)");
	asm("beq	$v0,	$v1,	_END_v");
	asm("addiu	$v0,	$v0,	1");
	asm("j	_DECR_v");
	asm("sw		$v0,	0($t4)");
	asm("_END_v:");
	asm("lw	$v0,	0($sp)");
	asm("lw $v1,	4($sp)");
	asm("nop");
}




unsigned int Rplusone( unsigned int A)
{
	return A+1;
}

unsigned int Vplusone( unsigned int A)
{
	return TEMP+1;
}


void	_PUSH_CYCLES(char Sel, unsigned int cycles)
{

	_PUSH_TO_BUFFER(0xd);
	_PUSH_TO_BUFFER('\n');
	_PUSH_TO_BUFFER(Sel);
	_PUSH_TO_BUFFER(':');
	_PUSH_TO_BUFFER(' ');
				
	unsigned int temp_cycles = 1000000000;
	
	for( ; temp_cycles > 0; )
	{
		if( temp_cycles == 100000000 || temp_cycles == 100000 || temp_cycles == 100 )
		{
			_PUSH_TO_BUFFER( ',' );
		}
		
		char	temp_char = '0' + (char)DIV( cycles, temp_cycles );
		_PUSH_TO_BUFFER( temp_char );
		cycles = MOD( cycles , temp_cycles  );
		temp_cycles = DIV( temp_cycles , 10);
	}
	
	_PUSH_TO_BUFFER(' ');
	_PUSH_TO_BUFFER('C');
	_PUSH_TO_BUFFER('Y');
	_PUSH_TO_BUFFER('C');
	_PUSH_TO_BUFFER('L');
	_PUSH_TO_BUFFER('E');
	_PUSH_TO_BUFFER('S');
	_PUSH_TO_BUFFER(13);
	_PUSH_TO_BUFFER('\n');
	return ;
	

}

void	_PUSH_TO_BUFFER(char character)					//#BUFFER[inIdx]=a0
{
	
	asm( "sw			$v0,	24($sp)"	);	
	asm( "sw			$t2,	20($sp)"	);
	asm( "sw			$t0,	12($sp)"	);
	asm( "sw			$t1,	8($sp)"		);	
	asm( "sw			$a0,	4($sp)"		);
	asm( "sw			$v0,	24($sp)"	);		
	asm( "lui			$t1,	0x8000"		);	//check if UART ready
	asm( "lw			$t1,	0($t1)"		);	//check if UART ready
	asm(	"nop" );
	asm( "bgtz			$t1,	_TO_UART"	);	//check if UART ready
	asm(	"nop" );
	asm( "lui			$t0,	0x1FFF"		);
	asm( "lw			$a0,	4($t0)			#a0=*inIdx"			);
	asm(	"nop" );
	asm( "addiu		$a0,	$a0,	0x1"	);	//inIdx++
	asm( "addiu		$a1,	$0,		20	"	);
	
	_MOD();	//v0=(inIdx++)%20
	asm( "lw			$t1,	8($t0)			#t1=OutIdx"	);
	asm(	"nop" );
	asm( "addu		$t2,	$v0,	$0"		);
	asm( "beq			$t1,	$v0, 	_END_PUSH_TO_BUFFER"		); //outIndx == (inIdx++)%20, jump if buffer full
	asm( "lw			$a0,	4($t0)"		); //a0 = original inIdx
	asm(	"nop" );
	asm( "addiu		$t0,	$t0,	0xc"		); //t0 points to buffer
	asm( "addu		$a1,	$a0,	$t0		#a1=BUFFER+*inIdx"		);
	asm( "lw			$t1,	4($sp)"		); //t1 has character we want to send
	asm(	"nop" );
	asm( "sb			$t1,	0($a1)"		);
	asm( "addu		$a0,	$t2,	$0"		);
	asm( "lui			$t0,	0x1FFF			#update inIdx"		);
	asm( "sw			$a0,	4($t0)"		);
	asm( "j			_END_PUSH_TO_BUFFER"	);

	asm( "_TO_UART:"	);
	asm( "lui			$t1,	0x8000"		);
	asm( "sw			$a0,	0x8($t1)"	);

	asm( "_END_PUSH_TO_BUFFER:"	);
	asm( "lw			$v0,	24($sp)"	);
	asm( "lw			$t2,	20($sp)"	);
	asm( "lw			$t0,	12($sp)"	);
	asm( "lw			$t1,	8($sp)"		);
	asm( "lw			$a0,	4($sp)"		);
	asm(	"nop" );

	
}


void _DIV() //returns x/y
{
	asm(	"sw		$a0,		0($sp)");
	asm(	"sw		$a1,		8($sp)");
	asm(	"sw		$t0,		4($sp)");
	asm(	"addiu	$v0,	$0,	0");
	asm(	"_DIV_TEST:"						);
	asm(	"sltu		$t0,	$a0,	$a1"	);
	asm(	"bgtz			$t0,	_END_DIV"	);
	asm(	"nop" );
	asm(	"subu		$a0,	$a0,	$a1"	);
	asm(	"addiu		$v0,	$v0,	1"		);
	asm(	"j			_DIV_TEST"				);
	asm(	"nop"								);
	asm(	"_END_DIV:"							);
	asm( 	"lw			$a0,	0($sp)"		);
	asm(	"lw			$t0,	4($sp)"		);
	asm(	"lw			$a1,	8($sp)"		);
	asm(	"nop" );

}


void	_MOD()		//Returns X%Y
{
	asm(	"sw		$a0,		0($sp)");
	asm(	"sw		$a1,		8($sp)");
	asm(	"sw		$t0,		4($sp)");
	asm(	"addiu	$v0,		$0,		0");
	asm(	"li		$t0,		1"		);
	asm(	"beq	$a1,		$t0,	_END_MOD"	);
	asm(	"nop"								);

	asm(	"addiu	$v0,		$a0,		0");
	asm(	"_MOD_TEST:"						);
	asm(	"sltu		$t0,	$v0,	$a1"	);
	asm(	"bgtz		$t0,	_END_MOD"	);
	asm(	"nop"								);
	asm(	"subu		$v0,	$v0,	$a1"	);
	asm(	"j			_MOD_TEST"				);
	asm(	"nop"								);
	asm(	"_END_MOD:"							);
	asm( 	"lw			$a0,	0($sp)"		);
	asm(	"lw			$a1,	8($sp)"		);
	asm(	"lw			$t0,		4($sp)");
	asm(	"nop" );
}


unsigned int MOD( unsigned int  x,  unsigned int  y)					//Returns X%Y
{
	unsigned int z = x;
		for( ; z >= y;)
		{
			z=z-y;
		}
			
	return z;
}

unsigned int DIV( unsigned int x,  unsigned int  y)
{
	unsigned int z=0;
		for( ; x >= y;)
		{
			z=z+1;
			x = x-y;
		}
		
	return z;
}



