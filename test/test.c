#include "stdio.h"
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>



int main(int argc, char* argv[]){
	int pipefd[2], status, done=0;
	pid_t cpid;

	pipe(pipefd);

	cpid = fork();
	if(cpid == 0) {
		dup2(pipefd[1], STDOUT_FILENO);
		execlp(argv[1], argv[1], (char *) NULL);
	}
	cpid = fork();
	if(cpid == 0){
		dup2(pipefd[0], STDIN_FILENO);
		execlp(argv[2], argv[2], (char *) NULL);
	}

	close(pipefd[0]);
	close(pipefd[1]);

	waitpid(-1, &status, 0);
	waitpid(-1, &status, 0);


}