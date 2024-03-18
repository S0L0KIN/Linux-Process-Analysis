struct threshNode
//is an ADT for nodes that are above threshold, stores all the information needed for printing those node in a linked list

struct fdNode
// is an ADT for every file descriptor in a given pid, stores all a specific fd's information needed for printing in a linked list

struct pidObj
// is an ADT for storing every process id (pid) the user has running in a linked list 

struct threshNode *create_TN(int pid, int fd, struct threshNode* head)
// allocates space for a threshNode on the heap, initializes it with pid and fd, and then immediately inserts into the front of the linked list at head

struct fdNode *create_fdNode(int fd, int inode, char name[256],struct fdNode* head)
// allocates space for a fdNode on the heap, initializes it with fd, inode, and name, and then immediately inserts into the front of the linked list at head

struct fdNode *linkfdNodes(int pid)
// PRECONDITION: pid must be a valid pid for user (does not segfault but will exit program)
// returns a linked list for every fd for the given path /proc/pid/fd

struct pidObj *create_pidObj(int pid, struct pidObj *head)
// allocates space for a pidObj on the heap, initializes it with pid, initializes a linked list of its fd nodes (using linkfdNodes function) and immediately inserts into the front of the linked list at head

struct threshNode* printPP(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold)
// prints the per-process table of all the pids in linked list head,
// if not threshcheck, it will create and return the linked list of the fds greater than threshold, into the thead.

struct threshNode* printSW(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold)
// prints the system wide table of all the pids in linked list head,
// if not threshcheck, it will create and return the linked list of the fds greater than threshold, into the thead.

struct threshNode* printIN(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold)
// prints the inode table of all the pids in linked list head,
// if not threshcheck, it will create and return the linked list of the fds greater than threshold, into the thead.

struct threshNode* printComposite(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold)
// prints the composite table of all the pids in linked list head,
// if not threshcheck, it will create and return the linked list of the fds greater than threshold, into the thead.

void printThresh(struct threshNode* head)
// prints the information in all the threshNodes contained in linked list at head

void saveTXT (struct pidObj* head)
// writes and saves the composite table of linked list at head into compositeTable.txt

void saveBIN (struct pidObj* head)
// writes and saves the composite table of linked list at head into compositeTable.bin (in binary)

int main(int argc, char** argv)
// parses all input flags, calls pid linked list building functions, and decides what needs to be printed

HOW TO RUN PROGRAM
	ACCEPTABLE FLAGS
	--per-process 	//prints the per-process table to the console
	--systemWide 	//prints the system wide table to the console
	--Vnodes	//prints the inode table to the console
	--composite	//prints the composite table to the console
	--threshold=X	//prints all the processes with FDs greater than X (where X > 0) in addition to whichever table would be printed based on flags
	--output_TXT	//saves the composite table in ASCII to a file named compositeTable.txt
	--output_binary //save the composite table in binary to a file named compositeTable.bin 
Additionally, the FIRST flag acts as a positional flag if you want to see the requested tables for a given pid (and only the first flag)
	i.e. "./a.out 1321 --composite" will print the composite table for only FDs with pid = 1321
In the case where no table flags are requested, solely the composite table will be printed by default.

