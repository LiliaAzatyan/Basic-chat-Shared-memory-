#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
  
#define Ready 1
#define NotReady -1
  
struct Memory {
  
    char buffer[100];
    int pid1, pid2, state;
};
  
struct Memory* shm_ptr;
  
//print message received from user2
  
void handler(int signum)
{
    // if signum is SIGUSR1, then user 1 received a message from user2
  
    if (signum == SIGUSR1) {
        printf("Received From User2: ");
        puts(shm_ptr->buffer);
    }
}
  
int main()
{
    // user1 pid
    int pid = getpid();
    int shm_id;
    //shared memory key value
    int shm_key = 12345;
    // shared memory create
    shm_id = shmget(shm_key, sizeof(struct Memory), IPC_CREAT | 0666);
  
    //shared memory attaching
    shm_ptr = (struct Memory*)shmat(shm_id, NULL, 0);
    // store the process id of user1 in shared memory
    shm_ptr->pid1 = pid;
    shm_ptr->state = NotReady;
    // calling the signal function using signal type SIGUSER1
    signal(SIGUSR1, handler);
  
    while (1) {
        //while (shm_ptr->state != Ready)?
          //  continue;?
        sleep(1);
        //input from user1
        printf("User1: ");
        fgets(shm_ptr->buffer, 100, stdin); 
        shm_ptr->state = 0;
  
        // sending the message to user2 
  
        kill(shm_ptr->pid2, SIGUSR2);
    }
  
    shmdt((void*)shm_ptr);
    shmctl(shm_id, IPC_RMID, NULL);
    return 0;
}
