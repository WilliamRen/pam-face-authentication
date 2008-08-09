#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void main()
{
    char *c;
    int shmidCommAuth;
    key_t ipckeyCommAuth;
    int *commAuth;
    key_t ipckey;
    ipckeyCommAuth =  567813;

    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);
    printf("---------------------------------------------- \n");
    printf("\n (A)uthenticate or  (C)ancel ? : \n");

    scanf("%c",c);
    *commAuth =14;

    if ((*c=='A')||(*c=='a'))
    {
        *commAuth =7;
        //break;
    }

}







