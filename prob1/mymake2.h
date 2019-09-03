#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/stat.h>


// Struct defs
typedef struct node {
	char* name;
	struct edge* linkHead;
	struct node* next;
	struct command* cmdHead;
	int isTarget;
	int visited;
	int completed;
	int exists;
	int mustBuild;
	time_t mtime;
} node;

typedef struct edge {
	struct node* to;
	struct edge* next;
} edge;

typedef struct command {
	char* cmdName;
	struct command* next;
} command;


// Global vars

extern int errSeen;
extern node* head;
extern node* tail;
extern FILE* input;
extern int cmdRun;


// Function prototypes

// in mymake2.c
void printUsageTip(char*);
void freeList(node* head);
void outOfMemory();
void errQuit();
void pot(node*);
void resetFlags();
int checkExists(char*);

// in handleinput2.c
void readMakeFile(FILE*);
node* processTargetLine(char*);
node* addNode(char*, int);
void addLink(node*, node*);
void processCommandLine(char*, node* targetNode);
int blankLine(char*);
char* stripWhitespace(char*);



