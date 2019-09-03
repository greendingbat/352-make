/* Author: Matt Burns
	Portion of assignment 10 dealing with processing the contents of the makefile
*/

#include "mymake2.h"

// Read makefile 1 line at a time, process based on contents
void readMakeFile(FILE* input) {
	char* line = NULL;
	size_t sz;
	node* targetNode = NULL;
	while ( getline(&line, &sz, input) > 0 ) {
		if (blankLine(line)) {
			continue;
		}
		if (line[0] == '\t') {	// command line
			if (targetNode == NULL) {
				fprintf(stderr, "Error in file, command with no target\n");
				free(line);
				errQuit();
			}
			processCommandLine(line, targetNode);
		} else { 				// target line
			targetNode = processTargetLine(line);
		}
	}
	free(line);
}

// Parse line that contains a target
// Split off target from dependencies
node* processTargetLine(char* line) {
	// Split off the target as everything before the ':'
	// If line does not contain a ':', fatal error
	// Target can't be obtained using sscanf, as there may be no whitespace
	char* ptr = line;
	while (*ptr != ':') {
		if (*ptr == '\0') { 	
		// End of string was reached, meaning the line is invalid
			fprintf(stderr, "Illegal line: %s", line);
			free(line);
			errQuit();			
		}
		ptr++;
	}
	*ptr = '\0';
	char* target = stripWhitespace(line);
	char* dependencies = ptr + 1;
	
	// ADD TARGET TO LIST
	node* targetNode = addNode(target, 1);
	
	// ADD DEPENDENCIES TO LIST
	
	char depName[65];
	int n;
	while ( sscanf(dependencies, "%64s%n", depName, &n) > 0) {
		//printf("found dependency %s\n", depName);
		// Create a node for the dependency
		node* depNode = addNode(depName, 0);
		addLink(targetNode, depNode);
		dependencies += n;
	}
	
	return targetNode;
}


// Create a node with the passed name
// returns pointer to node created, or the node with that name that already exists in the list
node* addNode(char* name, int isTarget) {
	node* newNode = malloc(sizeof(node));
	if (newNode == NULL) {
		outOfMemory();
	}
	newNode->name = strdup(name);
	newNode->linkHead = NULL;
	newNode->next = NULL;
	newNode->cmdHead = NULL;
	newNode->visited = 0;
	newNode->completed = 0;
	newNode->isTarget = isTarget;
	newNode->exists = 0;
	newNode->mtime = 0;
	newNode->mustBuild = 0;
	
	
	if (head == NULL) {	// List is empty, create new node, make it the head
		head = newNode;
		tail = head;
	} else { // List is not empty. Search for node with name
		node* ptr = head;
		while(ptr) {
			if (strcmp(ptr->name, name) == 0) {
				// Node was found at ptr
				if (ptr->isTarget == 1 && isTarget == 1) {
					errSeen = 1;
					fprintf(stderr, "Repeated target: %s\n", name);
					freeList(newNode);
					errQuit();
				} else if (ptr->isTarget != 1 && isTarget == 1) {
					// node was not a target before, make it a target
					ptr->isTarget = 1;
					freeList(newNode);
					return ptr;
				} else {
					freeList(newNode);
					return ptr;
				}
			}
			ptr = ptr->next;
		}
		tail->next = newNode;
		tail = tail->next;
	}
	
	return newNode;
	
}

// Create a link between two passed nodes
// Adds edge* to linkList of "from" node
void addLink(node* from, node* to) {
	edge* newLink = malloc(sizeof(edge));
	if (newLink == NULL) {
		outOfMemory();
	}
	newLink->to = to;
	newLink->next = NULL;
	if (from->linkHead == NULL) { // link list was empty
		from->linkHead = newLink;
	} else { // search for to node in from node link list
		edge* linkPtr = from->linkHead;
		while (linkPtr->next) {
			if (strcmp(linkPtr->to->name, to->name) == 0) {
				free(newLink);
				return;
			}
			linkPtr = linkPtr->next;
		}
 		if (strcmp(linkPtr->to->name, to->name) == 0) {
			free(newLink);
			return;
		} 
		linkPtr->next = newLink;
	}
}

// Process command line
// Put in command list of proper node
void processCommandLine(char* line, node* targetNode) {
	command* newCmd = malloc(sizeof(command));
	if (newCmd == NULL) {
		outOfMemory();
	}
	char* ptr = line;
	
	// remove newline
	while (*ptr != '\0') {
		if (*ptr == '\n') {
			*ptr = '\0';
		}
		ptr++;
	}
	
	newCmd->cmdName = strdup(line + 1);
	newCmd->next = NULL;
	if (targetNode->cmdHead == NULL) {
		targetNode->cmdHead = newCmd;
	} else {
		command* cmdPtr = targetNode->cmdHead;
		while (cmdPtr->next) {
			cmdPtr = cmdPtr->next;
		}
		cmdPtr->next = newCmd;
	}	
}


// Simple boolean function returns 1 if line contains no non-whitespace chars
int blankLine(char* line) {
	char* ptr = line;
	while (*ptr != '\0') {
		if (*ptr != '\t' && *ptr != ' ' && *ptr != '\n') { //isspace
			return 0;
		}
		ptr++;
	}
	
	return 1;	
}

// Strip whitespace from target name
// NOTE: THIS FUNCTION DOES NOT WORK IF THE PASSED STRING CONTAINS SPACES
// (more than one word)
char* stripWhitespace(char* str) {
	char* startPtr = str;
	char* endPtr = str;
	// Remove leading whitespace:
	while (*str != '\0') {
		while (isspace(*startPtr)) {
			startPtr++;
		}
		str++;
	}
	endPtr = startPtr;
	// Remove trailing whitespace:
	while (*endPtr != '\0' && !isspace(*endPtr)) {
		endPtr++;
	}
	*endPtr = '\0';
	return startPtr;
}