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
    int state, pid1, pid2;
};
  
struct Memory* shm_ptr; 
//  print message received from user1 
void handler(int signum)
{
    // if signum == SIGUSR2, then user 2 received a message from user1
    if (signum == SIGUSR2) {
        printf("Received From User1: ");
        puts(shm_ptr->buffer);
    }
}
  
int main()
{
    // pid of user2
    int pid = getpid();
    int shm_id;
    //shared memory key value 
    int key = 12345;
    //creating shared memory 
    shm_id = shmget(key, sizeof(struct Memory), IPC_CREAT | 0666);
  
    // attaching the shared memory
    shm_ptr = (struct Memory*)shmat(shm_id, NULL, 0);
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
  
    shmdt((void*)shm_ptr);
    return 0;
}
