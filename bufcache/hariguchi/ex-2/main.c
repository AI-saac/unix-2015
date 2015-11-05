// Yuki Hariguchi
// 61117552

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define NHASH 4
#define STAT_LOCKED 0x00000001
#define STAT_VALID 0x00000002
#define STAT_DWR 0x00000004
#define STAT_KRDWR 0x00000008
#define STAT_WAITED 0x00000010
#define STAT_OLD 0x00000020

struct buf_header{ int blkno; struct buf_header *hash_fp; struct buf_header *hash_bp; unsigned int stat; struct buf_header *free_fp; struct buf_header *free_bp; char *cache_data; }; typedef struct buf_header buf;
struct command_table{ char *cmd; void (*func)(int, char *[]); };
buf h_head[NHASH]; buf f_head; int bit = 1; int i = 0;
//functino delaration
buf *GetBuf(int index);
void Pst(buf *p);
void pbo(int index);
void Pball();
void Phashl(int hkey);
void Phash();
void Pf();
int SN(int blkno);
void Set(buf *h, int stat);
void ReSet(buf *h, int stat);
void Pstd(buf *p, int index);
buf *Getb(int blkno);
buf *getblk(int blknum);
void brelse(buf *buffer);
buf *getb(int blkno);
void remfree(buf *buf);
void addhash(buf *elem);
int isfree(buf *buffer);
int check(buf *buffer, int state);
void insert_hash_head(buf *h, buf *p);
void insert_free_head(buf *h, buf *p);
void insert_hash_tail(buf *h, buf *p);
void insert_free_tail(buf *h, buf *p);
buf *remove_free_head(); buf *remove_hash_head();

//functions
void insert_hash_head(buf *h, buf *p){ p -> hash_bp = h; p -> hash_fp = h -> hash_fp; h -> hash_fp -> hash_bp = p; h -> hash_fp = p; }
void insert_free_head(buf *h, buf *p){ p -> free_bp = h; p -> free_fp = h -> free_fp; h -> free_fp -> free_bp = p; h -> free_fp = p; }
void insert_hash_tail(buf *h, buf *p){ p -> hash_fp = h; p -> hash_bp = h -> hash_bp; h -> hash_bp -> hash_fp = p; h -> hash_bp = p; }
void insert_free_tail(buf *h, buf *p){ p -> free_fp = h; p -> free_bp = h -> free_bp; h -> free_bp -> free_fp = p; h -> free_bp = p; }
void remove_hash(buf *p){ buf *prev = p -> hash_bp; buf *next = p -> hash_fp; prev -> hash_fp = p -> hash_fp; next -> hash_bp = prev; }
void remove_free(buf *p){ f_head.free_fp = f_head.free_fp -> free_fp; f_head.free_fp -> free_bp = &f_head; p -> free_fp = NULL; p -> free_bp = NULL; }

buf *remove_free_head(){
  if(f_head.free_fp == &f_head && f_head.free_bp == &f_head)
    return NULL;
  buf *ret = f_head.free_fp;
  while(check(ret, STAT_DWR)){
    if(ret == &f_head) return NULL;
    ret = ret -> free_fp;
  }
  remove_hash(ret); f_head.free_fp = f_head.free_fp -> free_fp; f_head.free_fp -> free_bp = &f_head;
  return ret;
}

buf *remove_hash_head(int i){
  buf *ret = h_head[i].hash_fp; remove_hash(ret);
  return ret;
}

buf *getb(int num){
  int hkey = num % 4; buf *p;
  for(p = h_head[hkey].hash_fp; p != &h_head[hkey]; p = p -> hash_fp){
    if(p -> blkno == num)
      return p;
  }
  return NULL;
}

void remfree(buf *buffer){ buf *prev = buffer -> free_bp; buf *next = buffer -> free_fp; prev -> free_fp = next; next -> free_bp = prev; buffer -> free_fp = NULL; buffer -> free_bp = NULL; }
void addhash(buf *b){ int key = b -> blkno; int hkey = key % 4; b -> stat = b -> stat | STAT_LOCKED; insert_hash_tail(&h_head[hkey], b); }

int isfree(buf *buffer){ buf *p;
  for(p = &f_head; p != &f_head; p = p -> free_fp){
    if(p == buffer)
      return p -> blkno;
  }
  return 0;
}


int check(buf *buffer, int state){ return !(buffer -> stat & state ^ state); }

buf *getblk(int blknum){
  while(&h_head[blknum % 4] != NULL){
    buf *buffer = getb(blknum);
    if(buffer != NULL){
      if(check(buffer, STAT_LOCKED)){
	//sleep();
	printf("scenario 5\n"); printf("Process goes to sleep\n");
	buffer -> stat = buffer -> stat | STAT_WAITED;
	return NULL; continue;
      }
      //scenario 1
      printf("scenario 1\n"); buffer -> stat = STAT_LOCKED | STAT_VALID;
      remfree(buffer); return buffer;
    }
    else{
      if( f_head.free_fp == &f_head && f_head.free_bp == &f_head ){ //scenario 4
	//sleep();
	printf("scenario 4\n"); printf("Process goes to sleep\n"); return buffer;
	//continue;
      }
      buf *ref = f_head.free_fp;
      if(check(ref, STAT_DWR)){
	//scenario 3
	printf("scenario 3\n");
	buf *prev = ref -> free_bp; buf *next = ref -> free_fp;
	prev -> free_fp = next; next -> free_bp = prev;
	ref -> stat =  STAT_LOCKED | STAT_VALID | STAT_KRDWR | STAT_LOCKED;
	continue;
      }
      //scenario 2
      printf("scenario 2\n");
      buf *b = remove_free_head();
      b -> stat = b -> stat ^ STAT_VALID;
      b -> blkno = blknum; addhash(b); b -> stat = b -> stat | STAT_VALID; return b;
    }
  }
  return NULL;
}

void brelse(buf *buffer){
  //wakeup();
  if(check(buffer, STAT_LOCKED)){
    printf("Wakeup processes wating for any buffer\n");
    if(check(buffer, STAT_LOCKED | STAT_VALID | STAT_WAITED)){
      //wakeup();
      printf("Wakeup processes waiting for buffer of blkno %d\n", buffer -> blkno);
    }
    //raise_cpu_level();
    if(check(buffer, STAT_VALID) && !check(buffer, STAT_OLD)){
      printf("Enqueued tail\n");
      insert_free_tail(&f_head, buffer);
      buffer -> stat = STAT_VALID;
    }
    else{
      printf("Enqueued head\n");
      insert_free_head(&f_head, buffer);
      buffer -> stat = STAT_VALID;
    }
    //lower_cpu_level();
  }
  else{
    printf("Error! buffer should be locked\n");
  }
}

int parline(char *cmd, char **av){
  char *b = calloc(1, 100); char *p; int argc = 0;
  strcpy(b, cmd);
  b[strlen(b) - 1] = ' ';
  while(*b && ((*b == ' ')))
    b++;
  argc = 0; p = strchr(b, ' ');
  while(p){
    *p = '\0'; av[argc++] = b; b = p + 1;
    while(*b && (*b == ' '))
      b++;
    p = strchr(b, ' ');
  }
  av[argc] = NULL;
  if(argc == 0)
    return 0;
  return argc;
}

int parst(char *av){
  char val = av[0];
  switch(val){
  case('L'): return 1;
  case('V'): return 2;
  case('D'): return 4;
  case('K'): return 8;
  case('W'): return 16;
  case('O'): return 32;
  default:   return 0;
  }
}

void help_proc(int num, char *name[]){
  //init
  printf("init  \n"); printf("\tinitialize hash list and free list and make the \n");
  //buf
  printf("buf[n ...] \n"); printf("\tdisplay the status of the buffer at the block number.\n\n");
  //hash
  printf("hash[n ...] \n"); printf("\tdisplay the hash list at the hash key given from user.\n\n");
  //free
  printf("free\n"); printf("\tDisplay free list\n\n");
  //getblk
  printf("getblk n\n"); printf("\texecute getblk(n)\n\n");
  //brelse
  printf("brelse n\n"); printf("\texecute brelse()\n");
  //set
  printf("set n stat [stat]\n"); printf("\tset the status\n\n");
  //reset
  printf("reset n stat [stat]\n"); printf("\treset the status\n\n");
  //quit
  printf("quit\n"); printf("\tquit\n\n");
}

void init_proc(int ac, char *av[]){
  //init process
  for(i= 0; i < NHASH; i++){
    h_head[i].hash_fp = &h_head[i]; h_head[i].hash_bp = &h_head[i]; h_head[i].stat = 0; h_head[i].cache_data = NULL; h_head[i].free_fp = &f_head; h_head[i].free_bp = &f_head; buf *a = malloc(sizeof(buf)); buf *b = malloc(sizeof(buf)); buf *c = malloc(sizeof(buf)); insert_hash_head(&h_head[i], a); insert_hash_head(h_head[i].hash_fp, b); insert_hash_head(((h_head[i].hash_fp) -> hash_fp), c);
  } f_head.free_fp = &f_head; f_head.free_bp = &f_head;
  //adding to hash list
  buf *trac = h_head[0].hash_fp; trac -> blkno = 28; Set(trac, STAT_VALID); trac = trac -> hash_fp; trac -> blkno = 4; Set(trac, STAT_VALID); trac = trac -> hash_fp; trac -> blkno = 64; Set(trac, STAT_VALID | STAT_LOCKED);
  //mod = 1
  trac = h_head[1].hash_fp; trac -> blkno = 17; Set(trac, STAT_VALID | STAT_LOCKED); trac = trac -> hash_fp; trac -> blkno = 5; Set(trac, STAT_VALID); trac = trac -> hash_fp; trac -> blkno = 97; Set(trac, STAT_VALID);
  //mod = 2
  trac = h_head[2].hash_fp; trac -> blkno = 98; Set(trac, STAT_VALID | STAT_LOCKED); trac = trac -> hash_fp; trac -> blkno = 50; Set(trac, STAT_VALID | STAT_LOCKED); trac = trac -> hash_fp; trac -> blkno = 10; Set(trac, STAT_VALID);
  //mod = 3
  trac = h_head[3].hash_fp; trac -> blkno = 3; Set(trac, STAT_VALID); trac = trac -> hash_fp; trac -> blkno = 35; Set(trac, STAT_VALID | STAT_LOCKED); trac = trac -> hash_fp; trac -> blkno = 99; Set(trac, STAT_VALID | STAT_LOCKED);
  //adding freelist
  trac = &f_head; insert_free_tail(trac, h_head[3].hash_fp); insert_free_tail(trac, h_head[1].hash_fp -> hash_fp); insert_free_tail(trac, h_head[0].hash_fp -> hash_fp); insert_free_tail(trac, h_head[0].hash_fp); insert_free_tail(trac, h_head[1].hash_fp -> hash_fp -> hash_fp); insert_free_tail(trac, h_head[2].hash_fp -> hash_fp -> hash_fp);
}

void buf_proc(int ac, char *av[]){
  if(ac <= 1)
    Pball();
  else{
    for(i= 1; i < ac; i++){
      char cpy[100]; strncpy(cpy, av[i], 100); pbo(atoi(cpy));
    }
  }
}
void hash_proc(int ac, char *av[]){
  if(ac <= 1)
    Phash();
  else{
    for(i= 1; i < ac; i++)
      Phashl(atoi(av[i]));
  }
}
void free_proc(int ac, char *av[]){ Pf(); }
void getblk_proc(int ac, char *av[]){ buf *blockedbuf = getblk(atoi(av[1])); }
void brelse_proc(int ac, char *av[]){ if(getb(atoi(av[1]))){ buf *p = Getb(atoi(av[1])); brelse(p); } }

void set_proc(int ac, char *av[]){
  int state = 0; int checker = 0;
  for(i= 2; i < ac; i++)
     state += parst(av[i]);
  buf *buffer = Getb(atoi((av[1]))); buffer -> stat = buffer -> stat | state;
}

void reset_proc(int ac, char *av[]){
  int state = 0; int checker = 0;
  for(i= 2; i < ac; i++)
     state += parst(av[i]);
  buf *buffer = Getb(atoi((av[1]))); buffer -> stat = buffer -> stat ^ state;
}

void quit_proc(int ac, char *av[]){ bit = 0; }

void Pst(buf *p){
  int state = p -> stat;
  if(state & 0x20)  printf("O"); else  printf("-"); state = state << 1;
  if(state & 0x20)  printf("W"); else  printf("-"); state = state << 1;
  if(state & 0x20)  printf("K"); else  printf("-"); state = state << 1;
  if(state & 0x20)  printf("D"); else  printf("-"); state = state << 1;
  if(state & 0x20)  printf("V"); else  printf("-"); state = state << 1;
  if(state & 0x20)  printf("L"); else  printf("-");
}
void pbo(int index){
  int hkey = index / 3; buf *p = &h_head[hkey];
  for(i= index % 3; i >= 0; i--)
    p = p -> hash_fp;
  Pstd(p, index); Pst(p); printf("]\n");
}

void Pball(){
  int index = 0;
  for(i= 0; i < NHASH; i++){ buf *p;
    for(p = h_head[i].hash_fp; p != &h_head[i]; p = p -> hash_fp){
      Pstd(p, index); Pst(p); printf("]\n"); index++;
    }
  }
}

void Phashl(int hkey){
  int index = hkey * 3; buf *p;
  for(p = h_head[hkey].hash_fp; p != &h_head[hkey]; p = p -> hash_fp){
    Pstd(p, index); Pst(p); printf("]  "); index++;
  }
  printf("\n");
}

void Phash(){
  int index = 0;
  for(i= 0; i < NHASH; i++){
    printf("%d :", i); buf *p;
    for(p = h_head[i].hash_fp; p != &h_head[i]; p = p -> hash_fp){
      Pstd(p, index); Pst(p); printf("]"); index++;
    }
    printf("\n");
  }
}

void Pf(){
  int index = 0; buf *p;
  for(p = f_head.free_fp; p != &f_head; p = p -> free_fp){
    index = SN(p -> blkno); Pstd(p, index); Pst(p); printf("]");
  }
  printf("\n");
}

int SN(int blkno){
  buf *buffer = getb(blkno); int index = 0;
  for(i= 0; i < NHASH; i++){ buf *p;
    for(p = h_head[i].hash_fp; p != &h_head[i]; p = p -> hash_fp){
      if(p == buffer) return index; index++;
    }
  }
  return index;
}

void Set(buf *h, int state){ h -> stat = state; }
void ReSet(buf *h, int stat){ h -> stat = h -> stat ^ stat; }

buf *GetBuf(int index){
  int quotient = index / 4; int remainder = index % 4; buf *p = h_head[quotient].hash_fp;
  for(i= 0; i < remainder; i++)
    p = p -> hash_fp;
  return p;
}


void Pstd(buf *p, int index){
  if(index >= 10){
    if(p -> blkno >= 100)
      printf("[%d : %d ", index, p -> blkno);
    else{ if(p -> blkno >= 10) printf("[%d :  %d ", index, p -> blkno); else printf("[%d :   %d ", index, p -> blkno); }
  }
  else{
    if(p -> blkno >= 10){
      if(p -> blkno >= 100) printf("[ %d : %d ", index, p -> blkno); else printf("[ %d :  %d ", index, p -> blkno);
    }
    else
      printf("[ %d :   %d ", index, p -> blkno);
  }
}

buf *Getb(int blkno){
  int hkey = blkno % 4; buf *p;
  for(p = h_head[hkey].hash_fp; p != &h_head[hkey];p = p -> hash_fp){
    if(p -> blkno == blkno)
      return p;
  }
  return NULL;
}

struct command_table cmd_tbl[] = { {"help", help_proc}, {"init", init_proc}, {"buf", buf_proc}, {"hash", hash_proc}, {"free", free_proc}, {"getblk", getblk_proc}, {"brelse", brelse_proc}, {"set", set_proc}, {"reset", reset_proc}, {"quit", quit_proc}, {NULL, NULL} };

int main(int argc, char *argv[]){
  char cmdline[100];
  while(bit){
    printf("$ ");
    if(fgets(cmdline, 100, stdin) == NULL)
      exit(1);
    struct command_table *p;
    int ac = 0;
    char *av[16];
    ac = parline(cmdline, av);
    if(!ac)
      continue;
    for(p = cmd_tbl; p -> cmd; p++){
      if(strcmp(av[0], p -> cmd) == 0){ (*p -> func)(ac, av); break; }
    }
    if(p -> cmd == NULL) fprintf(stderr, "unknown cmmand: %s\n", av[0]);
  }
}
