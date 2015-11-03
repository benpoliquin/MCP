/****************************
 * Ben Poliquin			
 * Master Control Program	
 ****************************/
#include <stdio.h>
#include <stdlib.h>
#include <wordexp.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <proc/readproc.h>

#define MAX_LEN 256
#define MAX_BUFF 1000
 
int alarm_on;

void signal_handler(int sig){};

void alarm_handler(int sig){
	alarm_on = 1;
}

/***********************************************************
 * Function allows a call 
 * to create a clean print from the MCP Scheduler. 
 ***********************************************************/
void print_proc(pid_t pid[]){
	PROCTAB* proc = openproc(PROC_FILLMEM | PROC_FILLSTAT 
		| PROC_FILLSTATUS | PROC_PID, pid);
	proc_t proc_info;
	memset(&proc_info, 0, sizeof(proc_info));
	printf("\t\t CMD\t PPID \tUTIME \tSIZE\n");
	printf("\t\t ---------------------------\n");
	while(readproc(proc, &proc_info) != NULL){
		 printf("%20s: \t %d \t %5lld \t %ld \n",
         proc_info.cmd, proc_info.ppid,
         proc_info.utime, proc_info.size);
	}
	closeproc(proc);
}

int main(int argc, char const *argv[]){
	
	FILE *f = fopen(argv[1], "r");
	char buffer[MAX_BUFF][MAX_BUFF], line[MAX_LEN];
	int i, r, len, sig, numProgs = 0;	
	pid_t pid[numProgs], tmpPID;
	
	//Sets up the alarm
	signal(SIGALRM, alarm_handler);

	wordexp_t wordStruct; //wordexp is a struct with 3 types
	wordexp_t wordArray[MAX_LEN];
	
	/***********************************************************
	 * Help for sigset was found at stackoverflow regarding a 
	 * question on the ambiguous description of sigwait.
	 ***********************************************************/
	sigset_t sigset;
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR1);
	sigprocmask(SIG_BLOCK, &sigset, NULL);
	signal(SIGUSR1, signal_handler);
	
	/***********************************************************
	 * f is check for being null, and if it isnt, will continue 
	 * with the program. The words in the file are all assigned
	 * in an array/struct that keeps them easy to keep track of.
	 ***********************************************************/	
	if(f != NULL){
		while(fgets(line, sizeof(line), f) != NULL){
			len = strlen(line);
			if(line[len-1] == '\n'){
				line[len-1] = '\0';
			}
			strcpy(buffer[numProgs], line);
			wordexp(buffer[numProgs], &wordStruct, 0);
			wordArray[numProgs] = wordStruct;
			++numProgs;
		}
		
		/***********************************************************
		 * The following loops include the child processes and 
		 * forks the program. 
		 ***********************************************************/
		for(i = 0; i < numProgs; i++){
			tmpPID = fork();
			pid[i] = tmpPID;
			
			if(tmpPID == 0){ //Child Process
				sigwait(&sigset, &sig); //Waiting on SIGUSR1
				execvp(wordArray[i].we_wordv[0], wordArray[i].we_wordv);
			}
			if(tmpPID < 0) {
				perror("Failed to fork\n");
				exit(1);
			}
			else if(tmpPID > 0){
				wordfree(&(wordArray[i]));
			}
		}
		
		/***********************************************************
		 * These three loops wake the child up, stop the 
		 * child process, and then continue the process. When 
		 * continuing the process waits for it to finish before 
		 * starting the next one. 
		 ***********************************************************/		
		for(i = 0; i < numProgs; i++){ //Wake up

			printf("Process %d is waking up\n", pid[i]);
			kill(pid[i], SIGUSR1); //Wakes up sigwait from above loop
			//wait(&pid[i]);	//From part 2
		}
		for(i = 0; i < numProgs; i++){ //Stop
			printf("Stopping Process %d\n", pid[i]);
			kill(pid[i], SIGSTOP);
		}
		
		/***********************************************************
		 * For part 2, this loop continued the 
		 * process. Now the while loop below is what
		 * controls the process to continue. 
		 ***********************************************************/
//		for(i = 0; i < numProgs; i++){ //Continue
//			printf("Continuing %d ...\n", i);
//			kill(pid[i], SIGCONT);
//			//wait(&pid[i]);
//		}
		
		/***********************************************************
		 *					 MCP Scheduler				
		 ***********************************************************/
		int process_running = 1; 	//Instead of using Boolean
		while(process_running == 1 ){
			process_running = 0;	//Flag
			for(i = 0; i < numProgs ; i++){
				if(waitpid(pid[i], &r, WNOHANG) == 0){
					process_running  = 1;
					printf("Continuing Process: %d\n", pid[i]);
					kill(pid[i], SIGCONT);
					print_proc(pid);
					alarm(1);
					while(alarm_on == 0){
						;
					}
					alarm_on = 0;
					printf("Stopping Process: %d\n", pid[i]);
					kill(pid[i], SIGSTOP);
				}	
			}
		}
		printf("MCP exited succesfully\n");
		fclose(f);
	}
	return 0;
}
