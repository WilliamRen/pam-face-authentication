/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE
    Mentor: Alex Lau

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include <gtk/gtk.h>
#include "cv.h"
#include "highgui.h"
#include "pam_face_defines.h"
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

void resetFlags();
void ipcStart();
char *shared;
key_t ipckey;
int shmid;

// COMMUNICATION SHARED MEMORY
int *commAuth;
key_t ipckeyCommAuth;
int shmidCommAuth;
GtkBuilder *builder;

static gboolean
time_handler(GtkWidget *widget)
{
    if (widget->window == NULL) return FALSE;
    GtkWidget *imgWebcam;
    imgWebcam = GTK_WIDGET (gtk_builder_get_object (builder, "imgWebcam"));

    GdkPixbuf *pixbuf = NULL;
    pixbuf = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE,8,IMAGE_WIDTH,IMAGE_HEIGHT);
    unsigned char *gdata;
    if (pixbuf!=NULL)
    {
        gdata = gdk_pixbuf_get_pixels(pixbuf);
        int m,n,o;
        for (n=0;n<IMAGE_HEIGHT;n++)
        {
            for (m= 0;m<IMAGE_WIDTH;m++)
            {
                gdata[n*IMAGE_WIDTH*3 + m*3 +0]=(unsigned char)*(shared + m*3 + 2+ n*IMAGE_WIDTH*3);
                gdata[n*IMAGE_WIDTH*3 + m*3 +1]=(unsigned char)*(shared + m*3 + 1+ n*IMAGE_WIDTH*3);
                gdata[n*IMAGE_WIDTH*3 + m*3 +2]=(unsigned char)*(shared + m*3 + 0+ n*IMAGE_WIDTH*3);
            }
        }
        gtk_image_set_from_pixbuf(imgWebcam, pixbuf);
        g_object_unref (pixbuf);
    }

    if (*commAuth==EXIT_GUI)
    {
        resetFlags();
        gtk_main_quit();
    }
    return TRUE;
}
void resetFlags()
{
    *commAuth=0;
}

void ipcStart()
{
    /*   IPC   */
    ipckey =  IPC_KEY_IMAGE;
    shmid = shmget(ipckey, IMAGE_SIZE, IPC_CREAT | 0666);
    shared = shmat(shmid, NULL, 0);

    ipckeyCommAuth = IPC_KEY_STATUS;
    shmidCommAuth = shmget(ipckeyCommAuth, sizeof(int), IPC_CREAT | 0666);
    commAuth = shmat(shmidCommAuth, NULL, 0);

    *commAuth=0;
    /*   IPC END  */
}
void
on_gtk_faceauthenticate_destroy (GtkObject *object, gpointer user_data)
{

    gtk_main_quit();

}
void
on_btnCancel_clicked  (GtkButton *button,gpointer user_data)
{
    *commAuth=CANCEL;
    gtk_main_quit();
}
void
on_btnAuthenticate_clicked  (GtkButton *button,gpointer user_data)
{

    if (*commAuth!=EXIT_GUI)
    {
        if (*commAuth==AUTHENTICATE)
        {
            GtkWidget *lblAuth;

            lblAuth = GTK_WIDGET (gtk_builder_get_object (builder, "lblAuth"));
            gtk_label_set_text(GTK_LABEL(lblAuth), "Authenticate");
            *commAuth=0;


        }
        else
        {
            GtkWidget *lblAuth;

            lblAuth = GTK_WIDGET (gtk_builder_get_object (builder, "lblAuth"));
            gtk_label_set_text(GTK_LABEL(lblAuth), "Stop!");
            *commAuth=AUTHENTICATE;

        }
    }

}
int
main (int argc, char *argv[])
{

    GtkWidget               *window;
    FILE *file;
    char host[512];
    char cookie[512];
    char value[512];
    char* strCommand0="xauth add $(hostname)/unix:0 MIT-MAGIC-COOKIE-1 ";
    char* strCommand1;
    ipcStart();
    resetFlags();
    /* CODE FOR CHECKING CONNECTION TO X SERVER */
    if (gtk_init_check(&argc, &argv)==FALSE)
    {
        system(XAUTH_EXTRACT_COMMAND);
        if (file = fopen(XAUTH_EXTRACT_FILE, "r"))
        {
            fscanf(file,"%s  %s  %s", host, cookie,value);
            strCommand1 = (char *)calloc(strlen(strCommand0) +strlen(value)+1,sizeof(char));
            strcat(strCommand1,strCommand0);
            strcat(strCommand1,value);
            system(strCommand1);
        }
    }

    if (gtk_init_check(&argc, &argv)==FALSE)
    {
        /* RUN CANCEL CLICK CODE */
        *commAuth=CANCEL;
        return -1;
    }

    gtk_init (&argc, &argv);


    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, XML_GTK_BUILDER_FACE_AUTHENTICATE, NULL);

    window = GTK_WIDGET (gtk_builder_get_object (builder, "gtk-faceauthenticate"));
    gtk_builder_connect_signals (builder, NULL);
    g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);
    gtk_widget_show (window);
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
    return 0;
}
