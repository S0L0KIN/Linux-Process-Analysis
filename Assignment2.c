#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //for readlink (filename)
#include <dirent.h> //for directories
#include <pwd.h> //for owner name

struct threshNode {
    int pid;
    int fd;
    struct threshNode *next;
} threshNode;

struct fdNode { //want an object for each fd
    int fd;
    long int inode;
    char name[256]; //max length of file name is 255 (use readlink for this)
    struct fdNode *next; //need dynamic storage of all the nodes, choose linked list cause simple to implement
} fdNode;

struct pidObj {
    int pid;
    struct pidObj *next; //for list of all pids
    struct fdNode *fdhead;  //stores all this pids fds
} pidObj;

struct threshNode *create_TN(int pid, int fd, struct threshNode* head) {
    struct threshNode* new = NULL;
    new = (struct threshNode*)malloc(sizeof(struct threshNode)); //allocate
    if (new == NULL) {
        perror("No space");
        return new;
    }
    new->pid = pid; //initialize
    new->fd = fd;
    new->next = head; //with insert into linked list
    return new;
}

struct fdNode *create_fdNode(int fd, int inode, char name[256], struct fdNode* head) {
    struct fdNode* new = NULL;
    new = (struct fdNode*)malloc(sizeof(struct fdNode)); //allocate obj
    if (new == NULL) {
        perror("No space");
        return new;
    }

    new->fd = fd; //initialize
    new->inode = inode;
    strncpy(new->name, name, 256);
    new->next = head; //we can use this to skip needing an insert and just insert into list upon creation
    return new;
}

struct fdNode *linkfdNodes(int pid) {
    DIR *fd_dir = NULL; //open dir and check if valid
    char path[256] = {'\0'};
    snprintf(path, 256, "/proc/%d/fd", pid); //write path
    fd_dir = opendir(path); //open directory
    if (fd_dir == NULL) { //handle errors
        perror("Could not find directory");
        exit(1);
    }

    char temp[256];
    struct fdNode* head = NULL;
    struct dirent *entry = NULL;

    while((entry = readdir(fd_dir)) != NULL) { //loop through every entry
        if(strcmp(entry->d_name, "..") && strcmp(entry->d_name, ".")) {
            snprintf(path, 256, "/proc/%d/fd/%d", pid, atoi(entry->d_name));
            readlink(path, temp, 256);
            head = create_fdNode(atoi(entry->d_name), entry->d_ino, temp, head); //add to linked list
        }
        
    }
    closedir(fd_dir); //close and return
    return head;
}

struct pidObj *create_pidObj(int pid, struct pidObj *head) {
    struct pidObj* new = NULL;
    new = (struct pidObj*)malloc(sizeof(struct pidObj)); //allocate obj
    if (new == NULL) {
        perror("No space");
        return new;
    }

    new->pid = pid; //initialize variables
    new->next = head;
    new->fdhead = linkfdNodes(pid); //immediately create the list of fds
    return new;
}

struct threshNode* printPP(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold) {
    struct fdNode* temp = NULL; //to iterate through fds
    int i=0; //for counter
    printf("\tPID\tFD\n"); //header
    printf("\t=============================\n");

    while (head != NULL){ //for each pid
        temp = head->fdhead;
        while(temp != NULL) { //for each fd in each pid
            printf("%d\t%d\t%d\n", i, head->pid, temp->fd); //print

            if ((threshcheck == 0) && (temp->fd > threshold)) { //we have not built threshold LL and add this one
                thead = create_TN(head->pid, temp->fd, thead); //add to LL
            }

            i++;
            temp = temp->next; //iterate

        }
        head = head->next;
    }
    printf("\n\n");
    return thead;
}

struct threshNode* printSW(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold) {
    struct fdNode* temp = NULL; //to iterate through fds
    int i=0; //for counter
    printf("\tPID\tFD\tFilename\n"); //header
    printf("\t=============================\n");

    while (head != NULL){ //for each pid
        temp = head->fdhead;
        while(temp != NULL) { //for each fd in each pid
            printf("%d\t%d\t%d\t%s\t \n", i, head->pid, temp->fd,temp->name); //print

            if ((threshcheck == 0) && (temp->fd > threshold)) { //we have not built threshold LL and add this one
                thead = create_TN(head->pid, temp->fd, thead); //add to LL
            }

            i++;
            temp = temp->next; //iterate

        }
        head = head->next;
    }
    printf("\t=============================\n\n");
    return thead;
}

struct threshNode* printIN(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold) {
    struct fdNode* temp = NULL; //to iterate through fds
    printf("\tFD\tInode\n"); //header
    printf("\t=============================\n");

    while (head != NULL){ //for each pid
        temp = head->fdhead;
        while(temp != NULL) { //for each fd in each pid
            printf("\t%d\t%ld\n", temp->fd,temp->inode); //print

            if ((threshcheck == 0) && (temp->fd > threshold)) { //we have not built threshold LL and add this one
                thead = create_TN(head->pid, temp->fd, thead); //add to LL
            }

            temp = temp->next; //iterate

        }
        head = head->next;
    }
    printf("\t=============================\n\n");
    return thead;
}

struct threshNode* printComposite(struct pidObj *head, struct threshNode* thead, int threshcheck, int threshold) {
    struct fdNode* temp = NULL; //to iterate through fds
    int i=0;
    printf("\tPID\tFD\tFilename \t\t\tInode\n"); //header
    printf("\t===============================================================\n");

    while (head != NULL){ //for each pid
        temp = head->fdhead;
        while(temp != NULL) { //for each fd in each pid
            printf("%d\t%d\t%d\t%s\t%ld\n", i, head->pid, temp->fd, temp->name, temp->inode); //print

            if ((threshcheck == 0) && (temp->fd > threshold)) { //we have not built threshold LL and add this one
                thead = create_TN(head->pid, temp->fd, thead); //add to LL
            }

            temp = temp->next; //iterate
            i++;
        }
        head = head->next;
    }
    printf("\t===============================================================\n\n");
    return thead;
}

void printThresh(struct threshNode* head) {
    printf("### Offending processes:\n");

    while (head != NULL) {
        printf("%d (%d), ", head->pid, head->fd);
        head = head->next;
    }
}

void saveTXT (struct pidObj* head) {
    FILE *file = fopen("compositeTable.txt", "w"); //open file and error check
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    
    struct fdNode* temp = NULL; //to iterate through fds
    int i=0;
    fprintf(file, "\tPID\tFD\tFilename \t\t\tInode\n"); //header
    fprintf(file, "\t===============================================================\n");

    while (head != NULL){ //for each pid
        temp = head->fdhead;
        while(temp != NULL) { //for each fd in each pid
            fprintf(file, "%d\t%d\t%d\t%s\t%ld\n", i, head->pid, temp->fd, temp->name, temp->inode); //print
            temp = temp->next; //iterate
            i++;
        }
        head = head->next;
    }
    fprintf(file, "\t===============================================================\n\n");

    fclose(file); //close file
    printf("\n## Composite table has been saved to compositeTable.txt ###\n\n");
}

void saveBIN (struct pidObj* head) {
    FILE *file = fopen("compositeTable.bin", "wb"); //open file and error check
    if (file == NULL) {
        perror("Error opening file");
        exit(1);
    }
    
    struct fdNode* temp = NULL; // To iterate through fds

    //this implementation loses some formatting but still writes the data correctly 
    while (head != NULL) { // For each pid
        temp = head->fdhead;
        while (temp != NULL) { // For each fd in each pid
            // Use fwrite to write binary data to the file
            fwrite(&head->pid, sizeof(int), 1, file);
            fwrite(&temp->fd, sizeof(int), 1, file);
            fwrite(temp->name, sizeof(char), sizeof(temp->name), file);
            fwrite(&temp->inode, sizeof(long), 1, file);

            temp = temp->next; // Iterate
        }
        head = head->next;
    }

    fclose(file);
    printf("\n## Composite table has been saved to compositeTable.bin ###\n\n");
}

int main(int argc, char** argv) {
    int perprocess = 0;
    int systemwide = 0;
    int Vnodes = 0;
    int composite = 0;
    int threshold = 0; //gonna require threshold to be an int >0
    int threshchecked = 1; //use to see if we need to check threshold at all
    int specpid = 0; //if a specific pid is requested with positional arguments, this will be changed
    int txt = 0;
    int binary = 0;

    for (int i=1; i<argc; i++) { //check each flags
        if (!strcmp(argv[i], "--per-process")) perprocess = 1;
        else if (!strcmp(argv[i], "--systemWide")) systemwide = 1;
        else if (!strcmp(argv[i], "--Vnodes")) Vnodes = 1;
        else if (!strcmp(argv[i], "--composite")) composite = 1;
        else if (!strcmp(argv[i], "--output_TXT")) txt = 1;
        else if (!strcmp(argv[i], "--output_binary")) binary = 1;
        else if (!strncmp(argv[i], "--threshold=", 12)) {
            threshold = atoi(&argv[i][12]);
            if (threshold <=0) { //error check threshold
                perror("Threshold must be >0");
                return 1; 
            }
        }
    }
    if (!perprocess && !systemwide && !Vnodes && !txt && !binary)  composite = 1; //define standard behaviour for no arguments

    if (argc > 1) { //only check positional if there is an argument
        specpid = atoi(argv[1]); //define positional argument, since there is no process with pid 0 we will only get a pid if a number is put here
        if (specpid < 0) specpid = 0; //cant have negative pid
    }   

    struct pidObj *head = NULL; //declare variables to hold LL
    struct threshNode *threshhead = NULL;
    if (threshold) threshchecked = 0; //need to only add to thresh linked list once

    char path[256];
    if (specpid) head = create_pidObj(specpid, head); //only care abt specific pid
    else {
        DIR *proc_dir; //open directory
        proc_dir = opendir("/proc/");
        struct dirent *entry = NULL;
        while((entry = readdir(proc_dir)) != NULL) { //while still entries
            if (entry->d_name[0] >= '0' && entry->d_name[0] <= '9') { //check if its a number using ascii values (cause then it will be a pid dir)
                snprintf(path, 256, "/proc/%d/fd", atoi(entry->d_name));
            	if (access(path, R_OK) == 0) {

                    head = create_pidObj(atoi(entry->d_name), head);
                }
            }
        } 
    }

    if (perprocess) { //pp flag
        threshhead = printPP(head, threshhead, threshchecked, threshold);
        threshchecked = 1;
    }
    if (systemwide) { //sw flag
        threshhead = printSW(head, threshhead, threshchecked, threshold);
        threshchecked = 1;
    }
    if (Vnodes) { //inode flag
        threshhead = printIN(head, threshhead, threshchecked, threshold);
        threshchecked = 1;
    }
    if (composite) { //composite or no flag
        threshhead = printComposite(head, threshhead, threshchecked, threshold);
        threshchecked = 1;
    }
    if (threshold) { //threshold flag
        printThresh(threshhead);
    }

    //bonus
    if (txt) {
        saveTXT(head);
    }
    if (binary) {
        saveBIN(head);
    }

    return 0;
}
