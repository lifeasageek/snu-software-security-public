#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <common.h>

#define memoMAXCnt 5
int memoCnt = 0;

typedef struct memo
{
	void (*func)(char *);
	char data[32];
}MEMO;

MEMO *pointer[5];

void jump_to_here() {
  read_flag();
}

void help()
{
	printf("1. Allocation Memo\n");
	printf("2. Write Memo\n");
	printf("3. Print Memo\n");
}

int input_idx()
{
	int idx;
	printf("Input index of memo (0~%d)\n", memoCnt-1);
	scanf("%d", &idx);

	if(idx >= memoCnt)
	{
		printf("Invalid index\n");
		return -1;
	}

	return idx;
}

void print(char *data)
{
	printf("%s\n", data);
}

void print_memo()
{
	int idx = input_idx();
	if(idx == -1)
	{
		return;
	}

	pointer[idx]->func(pointer[idx]->data);
}

void write_memo()
{
	int idx = input_idx();
	if(idx == -1)
	{
		return;
	}
	
	printf("Write memo\n");
	scanf("%s", pointer[idx]->data);
}

void alloc_memo()
{
	int idx;
	if(memoCnt == memoMAXCnt)
	{
		printf("Cannot allocate memo\n");
		return ;
	}

	idx = memoCnt++;
	pointer[idx] = (MEMO *)malloc(sizeof(MEMO));
	memset(pointer[idx], 0, sizeof(MEMO));

	pointer[idx]->func = print; 

}

void start() {
  int cmd;

  while(1)
  {
	  help();
	  scanf("%d", &cmd);

	  switch(cmd)
	  {
	  	case 1:
	  		alloc_memo();
	  		break;
	  	case 2:
		  	write_memo();
	  		break;
	  	case 3:
	  		print_memo();
		  	break;
	  	default:
	  		exit(1);
		  	break;
	  }
  }

}

int main(int argc, char** argv) {
  /* no warning */
  start();
  return 0;
}
