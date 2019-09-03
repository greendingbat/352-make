#include "mymake2.h"

int errSeen = 0;
node* head;
node* tail;
FILE* input;
int cmdRun;

int main(int argc, char* argv[]) {

	head = NULL;
	tail = NULL;
	char* makeFileName = "myMakefile";
	char* target = NULL;
	
	// Argument count (argc - 1):
	// 		If 0:	Use default makefile, first target
	// 		If 1:	if arg is "-f", print usage tip (exit (1))
	//				if anything else, use as target
	// 		If 2:	if first arg is "-f", use second argument as makefile
	//				else, print usage tip (exit (1))
	// 		If 3:	if first arg is "-f", use second as makefile
	//				if second arg is "-f", use third as makefile
	// 		If >3:	
	
	if ( (argc - 1) == 0 ) {					// 0 Args
		// default behavior
	} else if ( (argc - 1) == 1 )  {			// 1 Arg
		if ( strcmp("-f", argv[1]) == 0 ) {
			printUsageTip(argv[0]);
		} else {
			target = argv[1];
		}
	} else if ( (argc - 1) == 2 ) {				// 2 Args
		if ( strcmp("-f", argv[1]) == 0 && strcmp("-f", argv[2]) != 0) {
			makeFileName = argv[2];
		} else {
			printUsageTip(argv[0]);
		}
	} else if ( (argc - 1) == 3 ) {				// 3 Args
		if ( strcmp("-f", argv[1]) == 0 && strcmp("-f", argv[2]) != 0 && strcmp("-f", argv[3]) != 0) {
			makeFileName = argv[2];
			target = argv[3];
		} else if ( strcmp("-f", argv[2]) == 0 && strcmp("-f", argv[1]) != 0 && strcmp("-f", argv[3]) != 0) {
			makeFileName = argv[3];
			target = argv[1];
		} else {
			printUsageTip(argv[0]);
		}
	} else {									// >3 Args
		printUsageTip(argv[0]);
	}
	
	// Attempt to open file makeFileName
	input = fopen(makeFileName, "r");
	if (input == NULL) {
		perror(makeFileName);
		exit(1);
	}
	
	readMakeFile(input);
	
	// if no target specified, use first in file
	if (target == NULL && head != NULL) {
		target = head->name;
	}
	
	cmdRun = 0;
	int found = 0;
	node* targetNode = head;
	while (targetNode != NULL) {
		if (strcmp(targetNode->name, target) == 0) {
			found = 1;
			break;
		}
		targetNode = targetNode->next;
	}
	if (found == 1) {
		resetFlags();
		pot(targetNode);
		if (cmdRun == 0) {
			printf("%s is up to date.\n", target);
		}
	} else {
		errSeen = 1;
		fprintf(stderr, "Target %s not found\n", target);
	}
	
	
	
	
	
/*  	printf("Printing list:\n");
	node* ptr = head;
	while (ptr) {
		printf("%s (%d)| ", ptr->name, ptr->isTarget);
		edge* linkPtr = ptr->linkHead;
		while (linkPtr) {
			printf("%s - ", linkPtr->to->name);
			linkPtr = linkPtr->next;
		}
		printf("\n");
		if (ptr->cmdHead != NULL) {
			printf("Commands:\n");
		}
		command* cmdPtr = ptr->cmdHead;
		while (cmdPtr) {
			printf("%s\n", cmdPtr->cmdName);
			cmdPtr = cmdPtr->next;
		}
		ptr = ptr->next;
	} */
	
	freeList(head);
	fclose(input);
	return errSeen;
}

// Post-order traversal, starting at node curr
void pot(node* curr) {
	struct stat nodeStat;
	if (curr->visited) {
		return;
	}
	curr->visited = 1;
	
	// set n.fileDate and n.doesExist
	if (checkExists(curr->name)) {
		stat(curr->name, &nodeStat);
		curr->exists = 1;
		curr->mtime = nodeStat.st_mtime;
	} else {
		curr->exists = 0;
	}
	
	if (curr->exists != 1) {
		if (curr->isTarget != 1) {
			fprintf(stderr, "No rule to make target %s\n", curr->name);
			errQuit();
		} else {
			curr->mustBuild = 1;
		}
	}
	
	
	edge* linkPtr = curr->linkHead;
	while (linkPtr) {
		node* d = linkPtr->to;
		pot(d);
		if (d->completed != 1) {
			fprintf(stderr, "Circular dependency dropped\n");
		} else if (d->mustBuild == 0) {
			if (d->exists != 1 || (d->mtime > curr->mtime)) {
				curr->mustBuild = 1;
			}
		}
		linkPtr = linkPtr->next;
	}
	
	if (curr->mustBuild) {
		// run all commands
		command* cmdPtr = curr->cmdHead;
		while (cmdPtr) {
			printf("%s\n", cmdPtr->cmdName);
			int cmdRetVal = system(cmdPtr->cmdName);
			cmdRun = 1;
			if (cmdRetVal != 0) {
				//perror(cmdPtr->cmdName);
				fprintf(stderr, "Command failed\n");
				exit(1);
			}
			cmdPtr = cmdPtr->next;
		}
		
		if (checkExists(curr->name)) {
			stat(curr->name, &nodeStat);
			curr->exists = 1;
			curr->mtime = nodeStat.st_mtime;
		} else {
			curr->exists = 0;
		}
	}
	
	
	
/* 	printf("%s\n", curr->name);
	command* cmdPtr = curr->cmdHead;
	while (cmdPtr) {
		printf("  %s\n", cmdPtr->cmdName);
		cmdPtr = cmdPtr->next;
	} */

	curr->completed = 1;
}


// Check to see if file with given name exists by attempting to open it
int checkExists(char* fileName) {
	FILE *file;
    if ((file = fopen(fileName, "r")))
    {
        fclose(file);
        return 1;
    }
    return 0;
	
}

// reset all flags in all nodes in list for POT
void resetFlags() {
	node* ptr = head;
	while (ptr) {
		ptr->visited = 0;
		ptr->completed = 0;
		ptr->exists = 0;
		ptr->mustBuild = 0;
		ptr->mtime = 0;
		ptr = ptr->next;
	}
}

// Free list memory, starting at passed node
// This means this function can be used to free individual nodes
// or portions of a list
void freeList(node* head) {
	edge* linkPtr;
	edge* nextPtr;
	node* nextNode;
	command* cmdPtr;
	command* nextCmd;
	while (head != NULL) {
		free(head->name);
		linkPtr = head->linkHead;
		while (linkPtr != NULL) {
			//free(linkPtr->to);
			nextPtr = linkPtr->next;
			free(linkPtr);
			linkPtr = nextPtr;
		}
 		cmdPtr = head->cmdHead;
		while (cmdPtr != NULL) {
			nextCmd = cmdPtr->next;
			free(cmdPtr->cmdName);
			free(cmdPtr);
			cmdPtr = nextCmd;
		} 
		nextNode = head->next;
		free(head);
		head = nextNode;
	}	
	
}

// Helper function prints usage help
void printUsageTip(char* progName) {
	fprintf(stderr, "Usage: %s [-f makefile] [target]\n", progName);
	exit(1);
}

// Called whenever malloc fails
void outOfMemory() {
	fprintf(stderr, "Out of memory\n");
	exit(1);
}

// Do somecleanup and then exit with return code 1
void errQuit() {
	freeList(head);
	fclose(input);
	exit(1);
}

