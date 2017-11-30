#include <fstream>
#include <string>
#include <iostream>
#include <cstring>

#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#include <sys/wait.h>             /* Needed to use wait() */
#include <stdio.h>                  
#include <stdlib.h>
#include <unistd.h>               /* UNIX and POSIX constants and functions (fork, pipe) */
#include <string.h>

using namespace std;




string testpipe(string s){
   	int pfd[2];
   	char buff[128];
   	int status[3] = {0};
   	int pid[3];
   	pid_t wpid;
	int ret_val = pipe(pfd);                 /* Create pipe */
	if (ret_val != 0) {             /* Test for success */
	    printf("Unable to create a pipe; errno=%d\n",errno);
	    exit(1);  /* Print error message and exit */
	}
	for(int i =0 ; i < 3 ; i++ )
	{
		pid[i] = fork();
		if (pid[i]) {
			/* child program */
			close(pfd[0]); /* close the read end */
			ret_val = write(pfd[1],s.c_str(),strlen(s.c_str())); /*Write to pipe*/
			if (ret_val != strlen(s.c_str())) {
				printf("Write did not return expected value\n");
				exit(2);/* Print error message and exit */
			}
			exit(0);
		}
	}
	/* parent program */
	// while((wpid = wait(&status)) > 0);
	for(int i = 0; i < 3; i++)
		waitpid(pid[i], &status[i], 0);
	close(pfd[1]); /* close the write end of pipe */
	char bigbuf[1024];
	strcpy(bigbuf, "");

	ret_val = read(pfd[0],buff,sizeof buff); /* Read from pipe */
	if (ret_val != strlen(s.c_str())) {
		printf("Read did not return expected value\n");
		printf("%s\n", buff);
		exit(3);                /* Print error message and exit */
	}
	strcat(bigbuf, buff);
	printf("parent read %s from the child program\n",buff);
	
	return bigbuf;
}

int main(){
	printf("%s\n", testpipe("ABC").c_str())	;
	return 0;
}