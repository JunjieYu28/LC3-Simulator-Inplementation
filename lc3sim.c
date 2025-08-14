#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/***************************************************************/
/*                                                             */
/* Files: isaprogram   LC-3 machine language program file     */
/*                                                             */
/***************************************************************/

/***************************************************************/
/* These are the functions you'll have to write.               */
/***************************************************************/

void process_instruction();

/***************************************************************/
/* A couple of useful definitions.                             */
/***************************************************************/
#define FALSE 0
#define TRUE  1

/***************************************************************/
/* Use this to avoid overflowing 16 bits on the bus.           */
/***************************************************************/
#define Low16bits(x) ((x) & 0xFFFF)

/***************************************************************/
/* Main memory.                                                */
/***************************************************************/
/* 
  MEMORY[A] stores the word address A
*/

#define WORDS_IN_MEM    0x08000 
int MEMORY[WORDS_IN_MEM];

/***************************************************************/

/***************************************************************/

/***************************************************************/
/* LC-3 State info.                                           */
/***************************************************************/
#define LC_3_REGS 8

int RUN_BIT;	/* run bit */


typedef struct System_Latches_Struct{

  int PC,		/* program counter */
    N,		/* n condition bit */
    Z,		/* z condition bit */
    P;		/* p condition bit */
  int REGS[LC_3_REGS]; /* register file. */
} System_Latches;

/* Data Structure for Latch */

System_Latches CURRENT_LATCHES, NEXT_LATCHES;

/***************************************************************/
/* A cycle counter.                                            */
/***************************************************************/
int INSTRUCTION_COUNT;

/***************************************************************/
/*                                                             */
/* Procedure : help                                            */
/*                                                             */
/* Purpose   : Print out a list of commands                    */
/*                                                             */
/***************************************************************/
void help() {                                                    
  printf("----------------LC-3 ISIM Help-----------------------\n");
  printf("go               -  run program to completion         \n");
  printf("run n            -  execute program for n instructions\n");
  printf("mdump low high   -  dump memory from low to high      \n");
  printf("rdump            -  dump the register & bus values    \n");
  printf("?                -  display this help menu            \n");
  printf("quit             -  exit the program                  \n\n");
}

/***************************************************************/
/*                                                             */
/* Procedure : cycle                                           */
/*                                                             */
/* Purpose   : Execute a cycle                                 */
/*                                                             */
/***************************************************************/
void cycle() {                                                

  process_instruction();
  CURRENT_LATCHES = NEXT_LATCHES;
  INSTRUCTION_COUNT++;
}

/***************************************************************/
/*                                                             */
/* Procedure : run n                                           */
/*                                                             */
/* Purpose   : Simulate the LC-3 for n cycles                 */
/*                                                             */
/***************************************************************/
void run(int num_cycles) {                                      
  int i;

  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating for %d cycles...\n\n", num_cycles);
  for (i = 0; i < num_cycles; i++) {
    if (CURRENT_LATCHES.PC == 0x0000) {
	    RUN_BIT = FALSE;
	    printf("Simulator halted\n\n");
	    break;
    }
    cycle();
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : go                                              */
/*                                                             */
/* Purpose   : Simulate the LC-3 until HALTed                 */
/*                                                             */
/***************************************************************/
void go() {                                                     
  if (RUN_BIT == FALSE) {
    printf("Can't simulate, Simulator is halted\n\n");
    return;
  }

  printf("Simulating...\n\n");
  while (CURRENT_LATCHES.PC != 0x0000)
    cycle();
  RUN_BIT = FALSE;
  printf("Simulator halted\n\n");
}

/***************************************************************/ 
/*                                                             */
/* Procedure : mdump                                           */
/*                                                             */
/* Purpose   : Dump a word-aligned region of memory to the     */
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void mdump(FILE * dumpsim_file, int start, int stop) {          
  int address; /* this is a address */

  printf("\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  printf("-------------------------------------\n");
  for (address = start ; address <= stop ; address++)
    printf("  0x%.4x (%d) : 0x%.2x\n", address , address , MEMORY[address]);
  printf("\n");

  /* dump the memory contents into the dumpsim file */
  fprintf(dumpsim_file, "\nMemory content [0x%.4x..0x%.4x] :\n", start, stop);
  fprintf(dumpsim_file, "-------------------------------------\n");
  for (address = start ; address <= stop ; address++)
    fprintf(dumpsim_file, " 0x%.4x (%d) : 0x%.2x\n", address , address , MEMORY[address]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : rdump                                           */
/*                                                             */
/* Purpose   : Dump current register and bus values to the     */   
/*             output file.                                    */
/*                                                             */
/***************************************************************/
void rdump(FILE * dumpsim_file) {                               
  int k; 

  printf("\nCurrent register/bus values :\n");
  printf("-------------------------------------\n");
  printf("Instruction Count : %d\n", INSTRUCTION_COUNT);
  printf("PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  printf("CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  printf("Registers:\n");
  for (k = 0; k < LC_3_REGS; k++)
    printf("%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  printf("\n");

  /* dump the state information into the dumpsim file */
  fprintf(dumpsim_file, "\nCurrent register/bus values :\n");
  fprintf(dumpsim_file, "-------------------------------------\n");
  fprintf(dumpsim_file, "Instruction Count : %d\n", INSTRUCTION_COUNT);
  fprintf(dumpsim_file, "PC                : 0x%.4x\n", CURRENT_LATCHES.PC);
  fprintf(dumpsim_file, "CCs: N = %d  Z = %d  P = %d\n", CURRENT_LATCHES.N, CURRENT_LATCHES.Z, CURRENT_LATCHES.P);
  fprintf(dumpsim_file, "Registers:\n");
  for (k = 0; k < LC_3_REGS; k++)
    fprintf(dumpsim_file, "%d: 0x%.4x\n", k, CURRENT_LATCHES.REGS[k]);
  fprintf(dumpsim_file, "\n");
  fflush(dumpsim_file);
}

/***************************************************************/
/*                                                             */
/* Procedure : get_command                                     */
/*                                                             */
/* Purpose   : Read a command from standard input.             */  
/*                                                             */
/***************************************************************/
void get_command(FILE * dumpsim_file) {                         
  char buffer[20];
  int start, stop, cycles;

  printf("LC-3-SIM> ");

  scanf("%s", buffer);
  printf("\n");

  switch(buffer[0]) {
  case 'G':
  case 'g':
    go();
    break;

  case 'M':
  case 'm':
    scanf("%i %i", &start, &stop);
    mdump(dumpsim_file, start, stop);
    break;

  case '?':
    help();
    break;
  case 'Q':
  case 'q':
    printf("Bye.\n");
    exit(0);

  case 'R':
  case 'r':
    if (buffer[1] == 'd' || buffer[1] == 'D')
	    rdump(dumpsim_file);
    else {
	    scanf("%d", &cycles);
	    run(cycles);
    }
    break;

  default:
    printf("Invalid Command\n");
    break;
  }
}

/***************************************************************/
/*                                                             */
/* Procedure : init_memory                                     */
/*                                                             */
/* Purpose   : Zero out the memory array                       */
/*                                                             */
/***************************************************************/
void init_memory() {                                           
  int i;

  for (i=0; i < WORDS_IN_MEM; i++) {
    MEMORY[i] = 0;
  }
}

/**************************************************************/
/*                                                            */
/* Procedure : load_program                                   */
/*                                                            */
/* Purpose   : Load program and service routines into mem.    */
/*                                                            */
/**************************************************************/
void load_program(char *program_filename) {                   
  FILE * prog;
  int ii, word, program_base;

  /* Open program file. */
  prog = fopen(program_filename, "r");
  if (prog == NULL) {
    printf("Error: Can't open program file %s\n", program_filename);
    exit(-1);
  }

  /* Read in the program. */
  if (fscanf(prog, "%x\n", &word) != EOF)
    program_base = word ;
  else {
    printf("Error: Program file is empty\n");
    exit(-1);
  }

  ii = 0;
  while (fscanf(prog, "%x\n", &word) != EOF) {
    /* Make sure it fits. */
    if (program_base + ii >= WORDS_IN_MEM) {
	    printf("Error: Program file %s is too long to fit in memory. %x\n",
             program_filename, ii);
	    exit(-1);
    }

    /* Write the word to memory array. */
    MEMORY[program_base + ii] = word;
    ii++;
  }

  if (CURRENT_LATCHES.PC == 0) CURRENT_LATCHES.PC = program_base;

  printf("Read %d words from program into memory.\n\n", ii);
}

/************************************************************/
/*                                                          */
/* Procedure : initialize                                   */
/*                                                          */
/* Purpose   : Load machine language program                */ 
/*             and set up initial state of the machine.     */
/*                                                          */
/************************************************************/
void initialize(char *program_filename, int num_prog_files) { 
  int i;

  init_memory();
  for ( i = 0; i < num_prog_files; i++ ) {
    load_program(program_filename);
    while(*program_filename++ != '\0');
  }
  CURRENT_LATCHES.Z = 1;  
  NEXT_LATCHES = CURRENT_LATCHES;
    
  RUN_BIT = TRUE;
}

/***************************************************************/
/*                                                             */
/* Procedure : main                                            */
/*                                                             */
/***************************************************************/
int main(int argc, char *argv[]) {                              
  FILE * dumpsim_file;

  /* Error Checking */
  if (argc < 2) {
    printf("Error: usage: %s <program_file_1> <program_file_2> ...\n",
           argv[0]);
    exit(1);
  }

  printf("LC-3 Simulator\n\n");

  initialize(argv[1], argc - 1);

  if ( (dumpsim_file = fopen( "dumpsim", "w" )) == NULL ) {
    printf("Error: Can't open dumpsim file\n");
    exit(-1);
  }

  while (1)
    get_command(dumpsim_file);
    
}

/***************************************************************/
/* Do not modify the above code.
   You are allowed to use the following global variables in your
   code. These are defined above.

   MEMORY

   CURRENT_LATCHES
   NEXT_LATCHES

   You may define your own local/global variables and functions.
   You may use the functions to get at the control bits defined
   above.

   Begin your code here 	  			       */

/***************************************************************/
int opcode, command[16];

int bin_to_dec(int num[], int begin, int end, int flag)
{
  int j = 1, res = 0;
  for(int i = begin; i <= end; ++i){
    res += j * num[i];
    j *= 2;
  }

  //imm && minus 挺妙的
  if(flag == 1 && num[end] == 1){
    int minus_res = 1;
    for(int i = begin; i <= end; ++i){
      minus_res *= 2;
    }
    return Low16bits(res-minus_res);
  }
  return Low16bits(res);
}

void dec_to_bin(int res[], int n)
{
  int i = 0;
  while(n > 0){
    int tmp = n % 2;
    n /= 2;
    res[i] = tmp;
    ++i;
  }
  while(i < 16){
    res[i] = 0;
    ++i;
  }
  return ;
}

void setNZP(int num)
{
  int tmp[16];
  dec_to_bin(tmp, num);
  CURRENT_LATCHES.N = 0;
  CURRENT_LATCHES.P = 0;
  CURRENT_LATCHES.Z = 0;
  if(num == 0)
    CURRENT_LATCHES.Z = 1;
  else if(tmp[15] == 0)
    CURRENT_LATCHES.P = 1;
  else
    CURRENT_LATCHES.N = 1;
  return ;
}

void ADD()
{
  int SR1 = bin_to_dec(command,6,8,0);
  int DR = bin_to_dec(command,9,11,0);
  if(command[5] == 0){
    int SR2 = bin_to_dec(command,0,2,0);
    CURRENT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + CURRENT_LATCHES.REGS[SR2]);
  }
  else{
    int imm = bin_to_dec(command,0,4,1);
    CURRENT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.REGS[SR1] + imm);
  }
  setNZP(CURRENT_LATCHES.REGS[DR]);
  return ;
}

void AND()
{
  int sr1[16], sr2[16], res[16];
  int SR1 = bin_to_dec(command,6,8,0);
  int DR = bin_to_dec(command,9,11,0);

  dec_to_bin(sr1, CURRENT_LATCHES.REGS[SR1]);
  if (command[5] == 0){
    int SR2 = bin_to_dec(command,0,2,0);
    dec_to_bin(sr2, CURRENT_LATCHES.REGS[SR2]);
  }
  else{
    int imm = bin_to_dec(command,0,4,1);
    dec_to_bin(sr2,imm);
  }
  for (int i = 0; i < 16; ++i) 
    res[i] = sr1[i] * sr2[i];
  CURRENT_LATCHES.REGS[DR] = bin_to_dec(res, 0, 15, 1);
  setNZP(CURRENT_LATCHES.REGS[DR]);
  return;
}

void BR()
{
  int flag = 0;
  if(command[11] == 1 && CURRENT_LATCHES.N == 1)
    flag = 1;
  if(command[10] == 1 && CURRENT_LATCHES.Z == 1)
    flag = 1;
  if(command[9] == 1 && CURRENT_LATCHES.P == 1)
    flag = 1;
  
  if(flag == 0) 
    return ;
  int offset = bin_to_dec(command,0,8,1);
  CURRENT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + offset);
  return ;
}

void JMP()
{
  int baseR = bin_to_dec(command,6,8,0);
  int tmp = CURRENT_LATCHES.PC;
  CURRENT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
  CURRENT_LATCHES.REGS[7] = tmp;
  return ;
}

void JSR()
{
  CURRENT_LATCHES.REGS[7] = CURRENT_LATCHES.PC;
  if (command[11] == 1){
    int offset = bin_to_dec(command,0,10,1);
    CURRENT_LATCHES.PC = Low16bits(CURRENT_LATCHES.PC + offset);
  }
  else{
    int baseR = bin_to_dec(command,6,8,0);
    CURRENT_LATCHES.PC = CURRENT_LATCHES.REGS[baseR];
  }
  return ;  
}

void LD()
{
  int DR = bin_to_dec(command,9,11,0);
  int offset = bin_to_dec(command,0,8,1);
  int loc = Low16bits(CURRENT_LATCHES.PC + offset);
  CURRENT_LATCHES.REGS[DR] = MEMORY[loc];
  setNZP(CURRENT_LATCHES.REGS[DR]);
  return ;
}

void LDI()
{
  int DR = bin_to_dec(command,9,11,0);
  int offset = bin_to_dec(command,0,8,1);
  int loc = MEMORY[Low16bits(CURRENT_LATCHES.PC + offset)];
  CURRENT_LATCHES.REGS[DR] = MEMORY[loc];
  setNZP(CURRENT_LATCHES.REGS[DR]);
  return ;
}

void LDR()
{
  int baseR = bin_to_dec(command,6,8,0);  
  int DR = bin_to_dec(command,9,11,0);
  int offset = bin_to_dec(command,0,5,1);
  int loc = Low16bits(CURRENT_LATCHES.REGS[baseR] + offset);
  CURRENT_LATCHES.REGS[DR] = MEMORY[loc];
  setNZP(CURRENT_LATCHES.REGS[DR]);
  return ;
}

void LEA()
{
  int DR = bin_to_dec(command,9,11,0);
  int offset = bin_to_dec(command,0,8,1);
  CURRENT_LATCHES.REGS[DR] = Low16bits(CURRENT_LATCHES.PC + offset);
  return ;
}

void NOT()
{
  int sr[16], res[16];  
  int SR = bin_to_dec(command,6,8,0);  
  int DR = bin_to_dec(command,9,11,0);
  dec_to_bin(sr,CURRENT_LATCHES.REGS[SR]);
  for(int i = 0;i < 16;++i){
    if(sr[i] == 0)
      res[i] = 1;
    else
      res[i] = 0;
  }
  CURRENT_LATCHES.REGS[DR] = bin_to_dec(res,0,15,1);
  setNZP(CURRENT_LATCHES.REGS[DR]);
  return ;
}

// void RET(){}

// void RTI(){}

void ST()
{
  int SR = bin_to_dec(command,9,11,0);
  int offset = bin_to_dec(command,0,8,1);
  int loc = Low16bits(CURRENT_LATCHES.PC + offset);
  MEMORY[loc] = CURRENT_LATCHES.REGS[SR];
  return ;
}

void STI()
{
  int SR = bin_to_dec(command,9,11,0);
  int offset = bin_to_dec(command,0,8,1);
  int loc = MEMORY[Low16bits(CURRENT_LATCHES.PC + offset)];
  MEMORY[loc] = CURRENT_LATCHES.REGS[SR];
  return ; 
}

void STR()
{
  int SR = bin_to_dec(command,9,11,0);
  int offset = bin_to_dec(command,0,5,1);
  int baseR = bin_to_dec(command,6,8,0);
  int loc = Low16bits(CURRENT_LATCHES.REGS[baseR] + offset);
  MEMORY[loc] = CURRENT_LATCHES.REGS[SR];
  return ;
}

void TRAP()
{
  int trapvect = bin_to_dec(command,0,7,0);
  if(trapvect == 37) // not 25 here, the code is 0xF025 --> 2*16+5 = 37
    CURRENT_LATCHES.PC = 0;
  return ;
}

void Fetch()
{
  int op = MEMORY[CURRENT_LATCHES.PC];
  ++CURRENT_LATCHES.PC;
  dec_to_bin(command, op);
}

void Decode()
{
  opcode = bin_to_dec(command,12,15,0);
}

void Excute()
{
  switch (opcode)
  {
    case 1: ADD(); break;
    case 5: AND(); break;
    case 0: BR(); break;
    case 12: JMP(); break;
    case 4: JSR(); break;
    case 2: LD(); break;
    case 10: LDI(); break;
    case 6: LDR(); break;
    case 14: LEA(); break;
    case 9: NOT(); break;
    // case 12: RET(); break;
    // case 8: RTI(); break;
    case 3: ST(); break;
    case 11: STI(); break;
    case 7: STR(); break;
    case 15: TRAP(); break;
    default: break;
  }
}


void process_instruction(){
  /*  function: process_instruction
   *  
   *    Process one instruction at a time  
   *       -Fetch one instruction
   *       -Decode 
   *       -Execute
   *       -Update NEXT_LATCHES
   */     
  Fetch();
  Decode();
  Excute();
  NEXT_LATCHES = CURRENT_LATCHES;
}
