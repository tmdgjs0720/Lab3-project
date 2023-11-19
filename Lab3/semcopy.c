#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>

void main()
{
	int shmid;
	int pid;
	char *shared_txt = (char*)malloc(sizeof(char)*15);
	void *shared_memory = (void *)0;
	
	shmid = shmget((key_t)1234, sizeof(char), 0666|IPC_CREAT);
	if(shmid == -1){
		perror("shmget failed : ");
		exit(0);
	}
	shared_memory = shmat(shmid, (void *)0,0);
	if(shared_memory == (void *)-1){
		perror("shmat failed :");
		exit(0);
	}
	shared_txt = (char *)shared_memory;
	pid = fork();
	if(pid == 0){
		shmid = shmget((key_t)1234, sizeof(char), 0);
		if(shmid == -1){
			perror("shmget failed :");
			exit(0);
		}
		shared_memory = shmat(shmid, (void *)0, 0666|IPC_CREAT);
		if(shared_memory == (void *)-1){
			perror("shmat failed :");
			exit(0);
		}
		shared_txt = (char *)shared_memory;
		FILE *f;
		f = fopen("copy_content.txt", "r");
		fscanf(f, "%[^\n]s", shared_txt);
		printf(" [child process] -> read copy_content.txt\n");
		fclose(f);
	}else if(pid > 0){
		FILE *f;
		f = fopen("copied.txt", "w");
		fprintf(f, "%s", shared_txt);
		printf(" [parent process] -> copy copied.txt\n");
	wait(1);
		fclose(f);
	}
}

