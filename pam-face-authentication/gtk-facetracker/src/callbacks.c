#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include <sys/types.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <string.h>
#include <stdio.h>


void
on_authenticate_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
    int shmidCommAuth;
    key_t ipckeyCommAuth;
    int *commAuth;
    key_t ipckey;
    ipckeyCommAuth =  567813;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);
    *commAuth =7;
}


void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
        int shmidCommAuth;
    key_t ipckeyCommAuth;
    int *commAuth;
    key_t ipckey;
    ipckeyCommAuth =  567813;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);
    *commAuth =14;
gtk_main_quit();
}

