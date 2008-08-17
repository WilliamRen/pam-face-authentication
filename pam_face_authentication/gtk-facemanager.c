/*
    Copyright (C) 2008 Rohan Anil (rohan.anil@gmail.com) , Alex Lau ( avengermojo@gmail.com)

    Google Summer of Code Program 2008
    Mentoring Organization: openSUSE

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
#include "highgui.h"
#include "cv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <dlfcn.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include "pam_face_defines.h"

extern char *HAAR_CASCADE_FACE;
extern char *HAAR_CASCADE_EYE;
extern char *HAAR_CASCADE_NOSE;
extern char *path;
extern char *imgPath;
extern char *imgExt;
extern char *GTK_FACE_AUTHENTICATE;
extern char *XAUTH_EXTRACT_FILE;
extern char *XAUTHDISPLAY_EXTRACT_FILE;
extern char *GTK_FACE_MANAGER_KEY;
extern char *XML_GTK_BUILDER_FACE_MANAGER;



GtkBuilder *builder;
GtkBuilder *builderIN;

IplImage *frame,*frameNew, *frame_copy = 0;
CvPoint pLeftEye;
CvPoint pRightEye;
CvCapture* capture;
int captureFace=-1;
int numberOfFacesToCapture=6;
void *handleFaceDetect;
void *handleFaceConfigure;
char* NONE_SELECTED="No User Selected";

enum
{
    USER_NAME = 0,
    STATUS,
    N_COLUMNS
};

void loadCVPIXBUF(GtkWidget *imgCapturedFace,IplImage* image)
{
    unsigned char *gdataUserFace;
    GdkPixbuf *pixbufUserFace= NULL;

    pixbufUserFace = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE,8,image->width,image->height);
    gdataUserFace = gdk_pixbuf_get_pixels(pixbufUserFace);

    int m,n;

    for (n=0;n<image->height;n++)
    {
        for (m= 0;m<image->width;m++)
        {
            CvScalar s;
            s=cvGet2D(image,n,m);
            gdataUserFace[n*image->width*3 + m*3 +0]=(uchar)s.val[2];
            gdataUserFace[n*image->width*3 + m*3 +1]=(uchar)s.val[1];
            gdataUserFace[n*image->width*3 + m*3 +2]=(uchar)s.val[0];
        }
    }

    gtk_image_set_from_pixbuf(imgCapturedFace, pixbufUserFace);
    g_object_unref (pixbufUserFace);
}

void loadFaceImage(char *fullPath,GtkWidget *img)
{

    IplImage* image =0;
    image=cvLoadImage( fullPath, 1 );
    if (image!=NULL)
        loadCVPIXBUF(img,image);
    else
    {
        image= cvCreateImage( cvSize(120,140),8,1);
        cvZero(image);
        loadCVPIXBUF(img,image);
    }
}
void saveConfigFile()
{
    FILE *file1;
    GtkWidget *tvList;
    tvList = GTK_WIDGET (gtk_builder_get_object (builder, "tvList"));
    GtkListStore *store;
    GtkTreeModel *model;
    GtkTreeIter  iter;
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (tvList)));
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (tvList));

    if (file1 = fopen(GTK_FACE_MANAGER_KEY, "w"))
    {
        if (gtk_tree_model_get_iter_first(model, &iter) != FALSE)
        {
            char *value,*value1;
            char *fullPath;
            char * t="Assigned";
            gtk_tree_model_get(model, &iter,  USER_NAME, &value,STATUS, &value1,  -1);
            if (strcmp(value1,t)==0)
            {
                // no itoa

                int i;
                for (i=1;i<7;i++)
                {
                    char * ch=(char *)calloc(2,sizeof(char));
                    if (i==1)
                        strcat(ch,"1");
                    if (i==2)
                        strcat(ch,"2");
                    if (i==3)
                        strcat(ch,"3");
                    if (i==4)
                        strcat(ch,"4");
                    if (i==5)
                        strcat(ch,"5");
                    if (i==6)
                        strcat(ch,"6");
                    fullPath=(char *)calloc(  strlen(imgPath) + strlen(value)+strlen(imgExt)+2,sizeof(char));
                    strcat(fullPath,imgPath);
                    strcat(fullPath,value);
                    strcat(fullPath,ch);
                    strcat(fullPath,imgExt);

                    fprintf(file1,"%s %s\n",value, fullPath);

                }

            }
            g_free(value);
            g_free(value1);
            while (gtk_tree_model_iter_next (model, &iter)!=FALSE)
            {
                gtk_tree_model_get(model, &iter,  USER_NAME, &value,STATUS, &value1,  -1);
                if (strcmp(value1,t)==0)
                {

                    int i;
                    for (i=1;i<7;i++)
                    {
                        char * ch=(char *)calloc(2,sizeof(char));
                        if (i==1)
                            strcat(ch,"1");
                        if (i==2)
                            strcat(ch,"2");
                        if (i==3)
                            strcat(ch,"3");
                        if (i==4)
                            strcat(ch,"4");
                        if (i==5)
                            strcat(ch,"5");
                        if (i==6)
                            strcat(ch,"6");
                        fullPath=(char *)calloc(  strlen(imgPath) + strlen(value)+strlen(imgExt)+2,sizeof(char));
                        strcat(fullPath,imgPath);
                        strcat(fullPath,value);
                        strcat(fullPath,ch);
                        strcat(fullPath,imgExt);

                        fprintf(file1,"%s %s\n",value, fullPath);

                    }
                }
            }


        }
    }
//   printf("dleror is %s \n",dlerror());

    fclose(file1);
}
void
on_btnSSave_clicked  (GtkButton *button,gpointer user_data)
{
    saveConfigFile();
    // learn();
    GtkWidget *inpDialog;
    inpDialog = GTK_WIDGET (gtk_builder_get_object (builder, "dlgSampleFace"));
    gtk_widget_hide(inpDialog);
}
void
on_btnInpOk_clicked  (GtkButton *button,gpointer user_data)
{
    GtkWidget *inpDialog;
    inpDialog = GTK_WIDGET (gtk_builder_get_object (builder, "dlgInputUserName"));

    char *text[2];
    char *b="Not Assigned";
    text[0]=(char *)gtk_entry_get_text (GTK_ENTRY(GTK_WIDGET (gtk_builder_get_object (builder, "entUserName"))));
    text[1]=b;
    GtkListStore *store;
    GtkTreeIter iter;
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(GTK_WIDGET (gtk_builder_get_object (builder, "tvList")))));
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, USER_NAME, text[0],STATUS,text[1], -1);
    gtk_widget_hide(inpDialog);

}
void
on_btnSCancel_clicked  (GtkButton *button,gpointer user_data)
{
    GtkWidget *inpDialog;
    inpDialog = GTK_WIDGET (gtk_builder_get_object (builder, "dlgSampleFace"));
    gtk_widget_hide(inpDialog);

}
void
on_btnSFace_clicked  (GtkButton *button,gpointer user_data)
{
    captureFace=6;

}
void
on_btnInpCancel_clicked  (GtkButton *button,gpointer user_data)
{
    GtkWidget *inpDialog;
    inpDialog = GTK_WIDGET (gtk_builder_get_object (builder, "dlgInputUserName"));
    gtk_widget_hide(inpDialog);

}

char * fileNameImage(char *ch,char *value)
{
    char *fullPath;
    fullPath=(char *)calloc(  strlen(imgPath) + strlen(value)+strlen(imgExt)+2,sizeof(char));
    strcat(fullPath,imgPath);
    strcat(fullPath,value);
    strcat(fullPath,ch);
    strcat(fullPath,imgExt);
    return fullPath;
}
void
on_btnTrain_clicked  (GtkButton *button,gpointer user_data)

{
    GtkTreeSelection *selection;
    GtkWidget *tvList;
    tvList = GTK_WIDGET (gtk_builder_get_object (builder, "tvList"));
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tvList));
    GtkTreeIter iter;
    GtkTreeModel *model;
    char *value;
    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter))
    {
        gtk_tree_model_get(model, &iter, USER_NAME, &value,  -1);
        intializePaths(value);
        GtkResponseType val;
        GtkWidget *inpDialog;
        GtkWidget *imgS;
        inpDialog = GTK_WIDGET (gtk_builder_get_object (builder, "dlgSampleFace"));
        IplImage *face;
        face = cvCreateImage( cvSize(120,140),8,3);
        cvZero(face);
        face=cvLoadImage(fileNameImage("1",value), 1 );
        if (face!=0)
        {
            imgS = GTK_WIDGET (gtk_builder_get_object (builder, "imgS1"));
            loadCVPIXBUF(imgS,face);
        }
        face=cvLoadImage(fileNameImage("2",value), 1 );
        if (face!=0)
        {
            imgS = GTK_WIDGET (gtk_builder_get_object (builder, "imgS2"));
            loadCVPIXBUF(imgS,face);
        }

        face=cvLoadImage(fileNameImage("3",value), 1 );
        if (face!=0)
        {
            imgS = GTK_WIDGET (gtk_builder_get_object (builder, "imgS3"));
            loadCVPIXBUF(imgS,face);
        }
        face=cvLoadImage(fileNameImage("4",value), 1 );
        if (face!=0)
        {
            imgS = GTK_WIDGET (gtk_builder_get_object (builder, "imgS4"));
            loadCVPIXBUF(imgS,face);
        }
        face=cvLoadImage(fileNameImage("5",value), 1 );
        if (face!=0)
        {
            imgS = GTK_WIDGET (gtk_builder_get_object (builder, "imgS5"));
            loadCVPIXBUF(imgS,face);
        }
        face=cvLoadImage(fileNameImage("6",value), 1 );
        if (face!=0)
        {
            imgS = GTK_WIDGET (gtk_builder_get_object (builder, "imgS6"));
            loadCVPIXBUF(imgS,face);
        }
        val=gtk_dialog_run(GTK_DIALOG(inpDialog));
    }

}
void
on_btnAbout_clicked  (GtkButton *button,gpointer user_data)
{
    GtkWidget *dialog = gtk_about_dialog_new();
    gtk_about_dialog_set_name(GTK_ABOUT_DIALOG(dialog), "PAM FACE AUTHENTICATION");
    gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog), "0.1");
    gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"Work done during Google Summer of Code 2008 for openSUSE");
    gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),
                                  "PAM Module for Face Recognition.");
    gtk_about_dialog_set_website(GTK_ABOUT_DIALOG(dialog),
                                 "http://code.google.com/p/pam-face-authentication/");
    gtk_dialog_run(GTK_DIALOG (dialog));
    gtk_widget_destroy(dialog);
}

GtkResponseType showYesNo(char * str)
{
    GtkResponseType response;
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,
                                    str);
    gtk_window_set_title(GTK_WINDOW(dialog), "Face Manager");
    response=gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    return response;
}

void
on_btnCaptureFace_clicked  (GtkButton *button,gpointer user_data)
{
    captureFace=6;
}
void
on_btnQuit_clicked  (GtkButton *button,gpointer user_data)
{
    char * str= "Are you sure to quit?";
    if (showYesNo(str)==GTK_RESPONSE_YES)
    {
        gtk_main_quit();
    }
}
void
on_btnAdd_clicked  (GtkButton *button,gpointer user_data)
{
    GtkResponseType val;
    GtkWidget *inpDialog;
    inpDialog = GTK_WIDGET (gtk_builder_get_object (builder, "dlgInputUserName"));
    val=gtk_dialog_run(GTK_DIALOG(inpDialog));


    gtk_widget_hide(inpDialog);
}


void
on_btnRemove_clicked  (GtkButton *button,gpointer user_data)
{
    GtkWidget *tvList;
    tvList = GTK_WIDGET (gtk_builder_get_object (builder, "tvList"));
    GtkListStore *store;
    GtkTreeModel *model;
    GtkTreeIter  iter;
    GtkTreeSelection *selection;
    selection  = gtk_tree_view_get_selection(GTK_TREE_VIEW(tvList));
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW (tvList)));
    model = gtk_tree_view_get_model (GTK_TREE_VIEW (tvList));
    if (gtk_tree_model_get_iter_first(model, &iter) == FALSE)
        return;
    if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection),&model, &iter))
    {
        char * str= "Do you want to remove from the List?";
        if (showYesNo(str)==GTK_RESPONSE_YES)
        {
            gtk_list_store_remove(store, &iter);
            saveConfigFile();
        }
    }
}

static void
init_list(GtkWidget *list)
{
    GtkCellRenderer *renderer;
    GtkTreeViewColumn *column;
    GtkListStore *store;
    renderer = gtk_cell_renderer_text_new();
    column = gtk_tree_view_column_new_with_attributes("User Name",renderer, "text", USER_NAME, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
    column = gtk_tree_view_column_new_with_attributes("Status",renderer, "text", STATUS, NULL);
    gtk_tree_view_append_column(GTK_TREE_VIEW(list), column);
    store = gtk_list_store_new(N_COLUMNS, G_TYPE_STRING,G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(list),GTK_TREE_MODEL(store));
    g_object_unref(store);
}

static void
add_to_list(GtkWidget *list, const gchar *str1,const gchar *str2)
{
    GtkListStore *store;
    GtkTreeIter iter;
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(list)));
    gtk_list_store_append(store, &iter);
    gtk_list_store_set(store, &iter, USER_NAME, str1,STATUS, str2,-1);
}

void createListItems(GtkWidget *tvList)
{
    char tempUserName[512];
    char imgFilename[512];
    FILE* file1;
    if (file1 = fopen(GTK_FACE_MANAGER_KEY, "r"))
    {
        while (fscanf(file1,"%s %s", tempUserName, imgFilename)!=EOF )
        {
            int i=0;
            for (i=0;i<5;i++)
                fscanf(file1,"%s %s", tempUserName, imgFilename);

            char *text[2];
            char *b="Assigned";
            text[0]=tempUserName;
            text[1]=b;
            add_to_list(tvList, text[0],b);
        }
    }
}

static gboolean time_handler(GtkWidget *widget)
{
    if (widget->window == NULL) return FALSE;
    if ( !cvGrabFrame( capture ))
        return FALSE;
    frame = cvRetrieveFrame( capture );
    if ( !frame )
        return FALSE;
    if ( !frame_copy )
    {
        frame_copy = cvCreateImage( cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );
        frameNew = cvCreateImage( cvSize(frame->width,frame->height),IPL_DEPTH_8U, frame->nChannels );
    }
    if ( frame->origin == IPL_ORIGIN_TL )
        cvCopy( frame, frame_copy, 0 );
    else
        cvFlip( frame, frame_copy, 0 );

    cvCopy( frame_copy, frameNew, 0 );
    allocateMemory();
    int k= faceDetect(frame_copy,&pLeftEye,&pRightEye);
    if (k==1 && captureFace>0)
    {
        IplImage *face;
        face = cvCreateImage( cvSize(120,140),8,1);
        int j= preprocess(frameNew,pLeftEye,pRightEye,face);
        GtkTreeIter iter;
        GtkTreeModel *model;
        char *value;
        char *b="Assigned";
        GtkListStore *store;
        GtkWidget *widget;
        GtkWidget *selection;
        widget = GTK_WIDGET (gtk_builder_get_object (builder, "tvList"));
        selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(widget));
        if (gtk_tree_selection_get_selected(GTK_TREE_SELECTION(selection), &model, &iter))
        {
            gtk_tree_model_get(model, &iter, USER_NAME, &value,  -1);
            char *fullPath;
            fullPath=(char *)calloc(  strlen(imgPath) + strlen(value)+strlen(imgExt)+1+1,sizeof(char));
            strcat(fullPath,imgPath);
            strcat(fullPath,value);
            char * ch=(char *)calloc(2,sizeof(char));
            GtkWidget *imgS;
            if ((numberOfFacesToCapture-captureFace)==0)
            {
                strcat(ch,"1");
                imgS = GTK_WIDGET (gtk_builder_get_object (builder,"imgS1"));
            }
            if ((numberOfFacesToCapture-captureFace)==1)
            {
                strcat(ch,"2");
                imgS = GTK_WIDGET (gtk_builder_get_object (builder,"imgS2"));
            }
            if ((numberOfFacesToCapture-captureFace)==2)
            {
                strcat(ch,"3");
                imgS = GTK_WIDGET (gtk_builder_get_object (builder,"imgS3"));
            }
            if ((numberOfFacesToCapture-captureFace)==3)
            {
                strcat(ch,"4");
                imgS = GTK_WIDGET (gtk_builder_get_object (builder,"imgS4"));
            }
            if ((numberOfFacesToCapture-captureFace)==4)
            {
                strcat(ch,"5");
                imgS = GTK_WIDGET (gtk_builder_get_object (builder,"imgS5"));
            }
            if ((numberOfFacesToCapture-captureFace)==5)
            {
                strcat(ch,"6");
                imgS = GTK_WIDGET (gtk_builder_get_object (builder,"imgS6"));
            }
            strcat(fullPath,ch);
            strcat(fullPath,imgExt);
            cvSaveImage(fullPath,face);
            if ((numberOfFacesToCapture-captureFace)>=0 &&(numberOfFacesToCapture-captureFace)<=5)
                loadFaceImage(fullPath,imgS);

            //loadCVPIXBUF(imgS,face);
            //     on_changed(selection,NULL);
        }
        if (j==1)
        {
            //   loadCVPIXBUF(simage,face);
            captureFace--;
            if (captureFace==0)
            {
                store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(GTK_WIDGET (gtk_builder_get_object (builder, "tvList")))));
                gtk_list_store_set(store, &iter, USER_NAME, value,STATUS,b, -1);
            }

        }
    }

    GtkWidget *imgSWebcam;
    imgSWebcam = GTK_WIDGET (gtk_builder_get_object (builder, "imgSWebcam"));
    loadCVPIXBUF(imgSWebcam,frame_copy);

    if ( cvWaitKey( 4 ) >= 0 )
        {}

    return TRUE;
}

int
main (int argc, char *argv[])
{
    intialize();
    capture = cvCaptureFromCAM(0);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH,IMAGE_WIDTH);
    cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT,IMAGE_HEIGHT);

    GtkWidget *window;
    GtkWidget *windowIN;
    GtkWidget *tvList;
    GtkTreeSelection *selection;
    GtkWidget *inpDialogSampleFace;

    GtkWidget *lblUserName;
    GtkWidget *inpDialog;
    gtk_init (&argc, &argv);
    builder = gtk_builder_new ();
    builderIN = gtk_builder_new ();
    gtk_builder_add_from_file (builder, XML_GTK_BUILDER_FACE_MANAGER, NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "gtk-facemanager"));
    tvList = GTK_WIDGET (gtk_builder_get_object (builder, "tvList"));
    inpDialog = GTK_WIDGET (gtk_builder_get_object (builder, "dlgInputUserName"));
    gtk_widget_hide(inpDialog);
    inpDialogSampleFace = GTK_WIDGET (gtk_builder_get_object (builder, "dlgSampleFace"));
    gtk_widget_hide(inpDialogSampleFace);

    g_timeout_add(100, (GSourceFunc) time_handler, (gpointer) window);
    gtk_builder_connect_signals (builder, NULL);
    init_list(tvList);
    createListItems(tvList);
    // hideShowSelectArea(0);
    //  lblUserName = GTK_WIDGET (gtk_builder_get_object (builder, "lblUserName"));
    // gtk_label_set_text(GTK_LABEL(lblUserName),NONE_SELECTED);
    selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(tvList));
    // g_signal_connect(selection, "changed",G_CALLBACK(on_changed),NULL);
    gtk_widget_show (window);
    gtk_main ();
    g_object_unref (G_OBJECT (builder));
    return 0;
}
