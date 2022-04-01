#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>


#define Ready 1
#define NotReady -1
#define SIZE 100
  
struct Memory {
    char buffer[SIZE];
    int state, pid1, pid2;
};
  
struct Memory* shm_ptr; 

void handler(int signum)
{
    
    if (signum == SIGUSR2) {
        printf("Received Message From User1: ");
        puts(shm_ptr->buffer);
    }
}
  
int main()
{
    // pid of user2
    int pid = getpid();
    int shm_id;
    const char* shm_name = "Memory";
  
    //creating shared memory 
    shm_id = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
  
    // attaching the shared memory
    shm_ptr = (struct Memory*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  if(shm_ptr == (void*)-1)
	{
		perror("Could not attach shared memory");
		exit(1);
	}
    // store the process id of user2 in shared memory
    shm_ptr->pid2 = pid;
    shm_ptr->state = NotReady;
    // calling the signal function using signal type SIGUSR2
    signal(SIGUSR2, handler);
  
    while (1) {
        sleep(1);
  
        // taking input from user2
        printf("User2: ");
        fgets(shm_ptr->buffer, 100, stdin);
        shm_ptr->state = Ready;
  
        // sending the message to user1 using kill function
        kill(shm_ptr->pid1, SIGUSR1);
        while (shm_ptr->state == Ready)
            continue;
    }
    if(shm_unlink(shm_name) != 0)
	{
		perror("Could not detach");
		exit(1);
	}
    //shmdt((void*)shm_ptr);
    return 0;
}
