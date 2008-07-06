#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"
#include "string.h"


#include "cv.h"
#include "highgui.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>

extern   GtkWidget *userDetails;
extern    GtkWidget *userList;
 extern GtkWidget *removeUserPrompt;
 extern  GtkWidget *userNamePrompt;
 extern   GtkWidget *removeUser;
extern GtkWidget *username;
extern  GtkWidget *userFace;
char *selectedUser;
 gint selectedRow;
 extern  CvPoint p1;
  extern CvPoint p2;
  extern IplImage *frame;
  extern IplImage *frame_copy;
  extern GdkPixbuf *pixbufUserFace;

unsigned char *gdataUserFace;
  char *path="/lib/pamface/facemanager/";
 extern void (*saveFace)(IplImage*,CvPoint*,CvPoint*,char*,char*);
 extern void (*learn)();
char* userFileName(char * username)
{ char* userFile;
     userFile=(char *)calloc(strlen(path) + strlen(username) + 5,sizeof(char));
 strcat(userFile,path);
  strcat(userFile,username);
 strcat(userFile,".pgm");
 return userFile;
}


void
on_assign_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
      saveFace(frame_copy,&p1,&p2,path,selectedUser);
char *userFile;
userFile=userFileName(selectedUser);
IplImage* image = cvLoadImage( userFile, 1 );
loadImageUserFace(image);
char * t ="Assigned";
 gtk_clist_set_text(GTK_CLIST(userList),selectedRow,1,(gchar *)t);


}

void loadImageUserFace(IplImage *image)
{

if(pixbufUserFace!=NULL)
{
gdataUserFace = gdk_pixbuf_get_pixels(pixbufUserFace);
int m,n,o;
for(n=0;n<112;n++)
 {
for(m= 0;m<92;m++)
 {

     CvScalar s;
s=cvGet2D(image,n,m);

gdataUserFace[n*92*3 + m*3 +0]=(uchar)s.val[2];
gdataUserFace[n*92*3 + m*3 +1]=(uchar)s.val[1];
gdataUserFace[n*92*3 + m*3 +2]=(uchar)s.val[0];
 }
 }

  gtk_image_set_from_pixbuf(userFace, pixbufUserFace);


}
}
void
on_userList_select_row                 (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
  gtk_widget_show (userDetails);
   gtk_widget_show (removeUser);
   selectedRow=row;
   char* text[2];
   gtk_clist_get_text (GTK_CLIST(userList),row,0,(gchar**)text);
   selectedUser=text[0];

char *userFile;
userFile=userFileName(selectedUser);
IplImage* image = cvLoadImage( userFile, 1 );
if(image)
{
    loadImageUserFace(image);
}
else
{
    IplImage* temp=cvCreateImage( cvSize(92,112), 8, 3 );
    loadImageUserFace(temp);
}

}


void
on_userList_unselect_row               (GtkCList        *clist,
                                        gint             row,
                                        gint             column,
                                        GdkEvent        *event,
                                        gpointer         user_data)
{
 gtk_widget_hide (userDetails);
 gtk_widget_hide (removeUser);
}


void
on_addUser_clicked                     (GtkButton       *button,
                                        gpointer         user_data)
{gtk_widget_show (userNamePrompt);
char *t ="";
gtk_entry_set_text (GTK_ENTRY(username),(gchar *)t);

}


void
on_removeUser_clicked                  (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_show (removeUserPrompt);
}


void
on_save_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
FILE *file2;

if(file2 = fopen("/lib/pamface/facemanager/face.key", "w"))
{
  int numberOfRows=(GTK_CLIST(userList)->rows);

        //fprintf(file2,"%d\n", numberOfRows);

int i=0;
for(i=0;i<numberOfRows;i++)
{

char* text[1];
gtk_clist_get_text (GTK_CLIST(userList),i,1,(gchar**)text);

char * t="Assigned";
	if(strcmp(text[0],t)==0)
	{


gtk_clist_get_text (GTK_CLIST(userList),i,0,(gchar**)text);
char *userFile;
userFile=userFileName(text[0]);
        fprintf(file2,"%s %s\n",text[0], userFile);



}





	}

}

fclose(file2);
learn();
}


void
on_exit_clicked                        (GtkButton       *button,
                                        gpointer         user_data)
{
gtk_main_quit();
}


void
on_add_clicked                         (GtkButton       *button,
                                        gpointer         user_data)
{

       char *text[2];
    char *b="Not Assigned";
text[0]=(char *)gtk_entry_get_text (GTK_ENTRY(username));
text[1]=b;
if(strlen(text[0])>0)
gtk_clist_append(GTK_CLIST(userList),(gchar**)text);
 gtk_widget_hide (userNamePrompt);
}


void
on_cancel_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
 gtk_widget_hide (userNamePrompt);
}


void
on_yes_clicked                         (GtkButton       *button,
                                        gpointer         user_data)
{
gtk_widget_hide (removeUserPrompt);

char *userFile;
userFile=userFileName(selectedUser);
remove(userFile);
gtk_clist_remove (GTK_CLIST(userList),selectedRow);
}


void
on_no_clicked                          (GtkButton       *button,
                                        gpointer         user_data)
{
  gtk_widget_hide (removeUserPrompt);
}

