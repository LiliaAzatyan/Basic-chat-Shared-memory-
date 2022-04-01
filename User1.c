#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
  
#define Ready 1
#define NotReady -1
#define SIZE 100
  
struct Memory {
  
    char buffer[SIZE];
    int pid1, pid2, state;
};
  
struct Memory* shm_ptr;
  
//print message received from user2
  
void handler(int signum)
{
    if (signum == SIGUSR1) {
        printf("Received Message From User2: ");
        puts(shm_ptr->buffer);
    }
}
  
int main()
{
    // user1 pid
    int pid = getpid();
    int shm_id;
    
  const char* shm_name = "Memory";
    // shared memory create
    shm_id = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
    //checking if shm_open worked
  if(shm_id == -1){
    perror("Could not create shared memory");
		exit(1);
  }
  ftruncate(shm_id, SIZE);
    //shared memory attaching
    shm_ptr = (struct Memory*)mmap(0, 100, PROT_READ | PROT_WRITE, MAP_SHARED, shm_id, 0);
  if(shm_ptr == (void*)-1)
	{
		perror("Could not attach to shared memory");
		exit(1);
	}
    // store the process id of user1 in shared memory
    shm_ptr->pid1 = pid;
    shm_ptr->state = NotReady;
    // calling the signal function using signal type SIGUSER1
    signal(SIGUSR1, handler);
  
    while (1) {
        do
		{
			sleep(1);
			fgets(shm_ptr->buffer, SIZE, stdin);
			shm_ptr->state = 0;
			kill(shm_ptr->pid2, SIGUSR2);
		}while(shm_ptr->state == NotReady); //0
    }
  if(shm_unlink(shm_name) != 0)
	{
		perror(" Could not detach");
		exit(1);
	}
    //shmdt((void*)shm_ptr);
  if(shmctl(shm_id, IPC_RMID, NULL) != 0)
	{
		perror("Not deleted");
		exit(1);
	}
    //shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
