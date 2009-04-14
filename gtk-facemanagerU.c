#include <sys/types.h>
#include <sys/stat.h>
#include <gtk/gtk.h>
#include <pwd.h> /* getpwdid */
#include <dirent.h>
#include <assert.h>
#include "cv.h"
#include <math.h>
#include <float.h>
#include <limits.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include "pam_face_defines.h"
#include <sys/time.h>
#include <time.h>
#include <gst/gst.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <gtk-facemanagerU.h>

extern void  featureDctMod2(IplImage * img,float*  features_final);
extern void  featureLBPHist(IplImage * img,float*  features_final);
extern void allocateMemory();
extern void intialize();
extern int faceDetect(IplImage*,CvPoint *,CvPoint *);
extern int  preprocess(IplImage*,CvPoint ,CvPoint ,IplImage*);
IplImage *currentFrame,*currentFrameDetected =0;
int showWhiteEffect=0;
/*
static gboolean expose_cb(GtkWidget * widget, GdkEventExpose * event, gpointer data)
{
    // printf("BLAH BLAH \n");
    gst_x_overlay_set_xwindow_id(GST_X_OVERLAY(data), GDK_WINDOW_XID(widget->window));

    unsigned char *data_photo;

    GdkPixbuf *pixbufUserFace= NULL;
    pixbufUserFace = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE,8,IMAGE_WIDTH,IMAGE_HEIGHT);
GdkColormap *cmap =gdk_rgb_get_cmap();
    gdk_pixbuf_get_from_drawable(pixbufUserFace,gtkWebcamImage->window,NULL,0,0,0,0,IMAGE_WIDTH,IMAGE_HEIGHT);
    data_photo = gdk_pixbuf_get_pixels(pixbufUserFace);
    IplImage *currentFrame=cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U,3);
    IplImage *frameNew=cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U,3);



    int m,n;
    for (n=0;n<IMAGE_HEIGHT;n++)
    {
        for (m= 0;m<IMAGE_WIDTH;m++)
        {

            CvScalar s;
         //   printf("%d \n",data_photo[m*3 + 0+ n*IMAGE_WIDTH*3]);
            s.val[2]=data_photo[m*3 + 0+ n*IMAGE_WIDTH*3];
            s.val[1]=data_photo[m*3 + 1+ n*IMAGE_WIDTH*3];

            s.val[0]=data_photo[m*3 + 2+ n*IMAGE_WIDTH*3];
            cvSet2D(currentFrame,n,m,s);

        }
    }
        cvSaveImage("/home/darksid3hack0r/abc.jpg",currentFrame);

    cvCopy( currentFrame, frameNew, 0 );
 allocateMemory();
    int k= faceDetect(currentFrame,&pLeftEye,&pRightEye);
    if (k==1 && numberOfFaces>0)
    {
        numberOfFaces--;
        IplImage *face;
        face = cvCreateImage( cvSize(120,140),8,1);
        int j= preprocess(frameNew,pLeftEye,pRightEye,face);

        char buffer[30];
        struct timeval tv;
        time_t curtime;
        gettimeofday(&tv, NULL);
        curtime=tv.tv_sec;
        strftime(buffer,30,"%m%d%y%H%M%S",localtime(&curtime));

        char imagepath[150];
        struct passwd *passwd;
        passwd = getpwuid (getuid());
        sprintf(imagepath,"%s/.pam-face-authentication/train/%s%d.jpg", passwd->pw_dir,buffer,numberOfFaces+1);
        // printf("%s \n",imagepath);
        cvSaveImage(imagepath,face);
        setGtkWebcamImageWhite(gtkWebcamImage);
        intializeGtkIconView();

    }

    loadCVPIXBUF(gtkWebcamImage,currentFrame);
    cvReleaseImage( &frameNew );
    cvReleaseImage( &currentFrame );

    return TRUE;
}
*/
void
on_gtkfacemanager_destroy  (GtkObject       *object,
                            gpointer         user_data)
{
    exit(0);
}
void     setGtkWebcamImageWhite(GtkWidget *imgCapturedFace)
{
    unsigned char *gdataUserFace;
    GdkPixbuf *pixbufUserFace= NULL;
    pixbufUserFace = gdk_pixbuf_new(GDK_COLORSPACE_RGB, FALSE,8,IMAGE_WIDTH,IMAGE_HEIGHT);
    gdataUserFace = gdk_pixbuf_get_pixels(pixbufUserFace);
    int m,n;
    for (n=0;n<IMAGE_HEIGHT;n++)
    {
        for (m= 0;m<IMAGE_WIDTH;m++)
        {
            gdataUserFace[n*IMAGE_WIDTH*3 + m*3 +0]=255;
            gdataUserFace[n*IMAGE_WIDTH*3 + m*3 +1]=255;
            gdataUserFace[n*IMAGE_WIDTH*3 + m*3 +2]=255;
        }
    }
    gdk_draw_pixbuf(imgCapturedFace->window,NULL,pixbufUserFace,0,0,0,0,IMAGE_WIDTH,IMAGE_HEIGHT,GDK_RGB_DITHER_NORMAL,0,0);
    g_object_unref (pixbufUserFace);
}


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
    gdk_draw_pixbuf(imgCapturedFace->window,NULL,pixbufUserFace,0,0,0,0,IMAGE_WIDTH,IMAGE_HEIGHT,GDK_RGB_DITHER_NORMAL,0,0);
//    gtk_image_set_from_pixbuf(imgCapturedFace, pixbufUserFace);
    g_object_unref (pixbufUserFace);
}
void
on_window_destroy (GtkObject *object, gpointer user_data)
{
    gtk_main_quit();
}

GtkTreeModel *
readFilesAndLoadGtkIconView()
{

    GtkListStore *list_store;
    GdkPixbuf *p1;
    GtkTreeIter iter;
    struct dirent *de=NULL;
    DIR *d=NULL;
    char dirpath[120];
    char fullimagepath[150];
    struct passwd *passwd;
    passwd = getpwuid (getuid());
    int status;
    sprintf(dirpath,"%s/.pam-face-authentication", passwd->pw_dir);
    status = mkdir(dirpath, S_IRWXU );
    sprintf(dirpath,"%s/.pam-face-authentication/train", passwd->pw_dir);
    status = mkdir(dirpath, S_IRWXU );
    sprintf(dirpath,"%s/.pam-face-authentication/train/", passwd->pw_dir);
    list_store = gtk_list_store_new(NUM_COLS,G_TYPE_STRING, GDK_TYPE_PIXBUF);
    d=opendir(dirpath);
    int fileCount=0;
    while (de = readdir(d))
    {
        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {
            fileCount++;

        }
    }
    int index=0;
    d=opendir(dirpath);

    if (fileCount>0)
    {
        double *fileNamesD=( double *)calloc(fileCount,sizeof( double));
        while (de = readdir(d))
        {
            if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
            {
                char *a=(char *)calloc(strlen(de->d_name)-4,sizeof(char));

                strncpy(a, de->d_name,strlen(de->d_name)-4);
                fileNamesD[index]=atof(a);
                index++;
                free(a);
            }
        }


        int i,j=0;
        for (i=0;i<fileCount;i++)
        {
            for (j=0;j<fileCount-i-1;j++)
            {
                double t;
                if (fileNamesD[j]>fileNamesD[j+1])
                {
                    t=fileNamesD[j];
                    fileNamesD[j]=fileNamesD[j+1];
                    fileNamesD[j+1]=t;
                }
            }

        }
//printf("%d Filecount \n",fileCount);
        for (i=0;i<fileCount;i++)

        {
            char aa[30];
            sprintf(aa,"%.0f.jpg",fileNamesD[i]);
            //  printf("%s Meow\n",aa);
            sprintf(fullimagepath,"%s/.pam-face-authentication/train/%s", passwd->pw_dir,aa);

            GError *err = NULL;
            //  printf("%s \n",fullimagepath);
            p1 =  gdk_pixbuf_new_from_file_at_size  (fullimagepath,73,100,&err);
            /* to be compatible with pam module installed from svn before april 11 2009 */
            if (err!=NULL)
            {

                err=NULL;

                sprintf(aa,"0%.0f.jpg",fileNamesD[i]);
                sprintf(fullimagepath,"%s/.pam-face-authentication/train/%s", passwd->pw_dir,aa);
                p1 =  gdk_pixbuf_new_from_file_at_size  (fullimagepath,73,100,&err);
            }
            if (err!=NULL)
                printf("%s \n",(char *)err->message);
            gtk_list_store_append(list_store, &iter);
            gtk_list_store_set(list_store, &iter, COL_DISPLAY_NAME,aa, COL_PIXBUF, p1, -1);
            //printf("%s SET \n");
        }
    }


    closedir(d);
    return GTK_TREE_MODEL(list_store);
}


void
view_popup_menu_onDelete (GtkWidget *menuitem, gpointer userdata)
{

    char imagepath[150];
    struct passwd *passwd;
    passwd = getpwuid (getuid());

    sprintf(imagepath,"%s/.pam-face-authentication/train/%s", passwd->pw_dir,userdata);
    remove(imagepath);
    intializeGtkIconView();

}



void
view_popup_menu (GtkWidget *iconview,GdkEventButton *event, char * filename)
{

    GtkWidget *menu, *menuitem;
    menu = gtk_menu_new();
    menuitem = gtk_menu_item_new_with_label("Delete Face");
    g_signal_connect(menuitem, "activate",(GCallback) view_popup_menu_onDelete, filename);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem);
    gtk_widget_show_all(menu);
    gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL,(event != NULL) ? event->button : 0,gdk_event_get_time((GdkEvent*)event));

}



gboolean
view_onButtonPressed (GtkWidget *iconview, GdkEventButton *event, gpointer userdata)
{
    gtk_icon_view_unselect_all (iconview);
    GtkTreePath *path;
    GtkTreePath *cell;
    if (gtk_icon_view_get_item_at_pos(iconview,(gint) event->x,event->y,&path,&cell))
    {
        gtk_icon_view_set_cursor(iconview,path,cell,FALSE);

        GtkTreeModel *model;
        GtkTreeIter   iter;
        char         *file;
        model = gtk_icon_view_get_model (GTK_ICON_VIEW (iconview));
        gtk_tree_model_get_iter (model, &iter, path);
        gtk_tree_model_get (model, &iter, COL_DISPLAY_NAME, &file, -1);
        // printf("%s \n",file);
        view_popup_menu(iconview,event,file);
    }
    return FALSE;
}
void intializeGtkIconView()
{
    gtk_icon_view_set_model (GTK_ICON_VIEW(gtkIconView),readFilesAndLoadGtkIconView());
    // gtk_icon_view_set_text_column(GTK_ICON_VIEW(gtkIconView),COL_DISPLAY_NAME);
    gtk_icon_view_set_pixbuf_column(GTK_ICON_VIEW(gtkIconView),COL_PIXBUF);

}



static gboolean
cb_have_data(GstElement *element, GstBuffer * buffer, GstPad* pad, gpointer user_data)
{

    unsigned char *data_photo = (unsigned char *) GST_BUFFER_DATA(buffer);
    IplImage *frameNew=cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U,3);



    int m,n;
    for (n=0;n<IMAGE_HEIGHT;n++)
    {
        for (m= 0;m<IMAGE_WIDTH;m++)
        {

            CvScalar s;
            s.val[2]=data_photo[m*3 + 0+ n*IMAGE_WIDTH*3];
            s.val[1]=data_photo[m*3 + 1+ n*IMAGE_WIDTH*3];

            s.val[0]=data_photo[m*3 + 2+ n*IMAGE_WIDTH*3];
            cvSet2D(currentFrame,n,m,s);

        }
    }
    cvCopy( currentFrame, frameNew, 0 );
    allocateMemory();
    int k= faceDetect(currentFrame,&pLeftEye,&pRightEye);
    cvCopy( currentFrame, currentFrameDetected, 0 );

    if (k==1 && numberOfFaces>0)
    {
        numberOfFaces--;
        IplImage *face;
        face = cvCreateImage( cvSize(120,140),8,1);
        int j= preprocess(frameNew,pLeftEye,pRightEye,face);

        char buffer[30];
        struct timeval tv;
        time_t curtime;
        gettimeofday(&tv, NULL);
        curtime=tv.tv_sec;
        strftime(buffer,30,"%m%d%y%H%M%S",localtime(&curtime));

        char imagepath[150];
        struct passwd *passwd;
        passwd = getpwuid (getuid());
        sprintf(imagepath,"%s/.pam-face-authentication/train/%s%d.jpg", passwd->pw_dir,buffer,numberOfFaces+1);
        // printf("%s \n",imagepath);
        cvSaveImage(imagepath,face);

        showWhiteEffect=1;
    }

    //loadCVPIXBUF(gtkWebcamImage,currentFrame);
    cvReleaseImage( &frameNew );
//    cvReleaseImage( &currentFrame );
    // printf("in the loop");
    return TRUE;
//   cvReleaseCapture( &currentFrame );

}

static gboolean time_handler(GtkWidget *widget)
{
    //printf("timer handler \n");
    if (showWhiteEffect==1)
    {
        setGtkWebcamImageWhite(gtkWebcamImage);
        showWhiteEffect=0;
        intializeGtkIconView();
    }
    else
    {
        if (currentFrameDetected!=NULL)
        {

            loadCVPIXBUF(gtkWebcamImage,currentFrameDetected);

        }
    }
    return TRUE;
}

/*
    if (widget->window == NULL) return FALSE;
    orginalFrame = cvQueryFrame( capture );
    if(orginalFrame==NULL) return FALSE;
    frame = cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U, orginalFrame->nChannels );
    cvResize(orginalFrame,frame, CV_INTER_LINEAR);
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
    //printf("aaa 1\n");
    allocateMemory();
    int k= faceDetect(frame_copy,&pLeftEye,&pRightEye);
    if (k==1 && numberOfFaces>0)
    {
        numberOfFaces--;
        IplImage *face;
        face = cvCreateImage( cvSize(120,140),8,1);
        int j= preprocess(frameNew,pLeftEye,pRightEye,face);

        char buffer[30];
        struct timeval tv;
        time_t curtime;
        gettimeofday(&tv, NULL);
        curtime=tv.tv_sec;
        strftime(buffer,30,"%m%d%y%H%M%S",localtime(&curtime));

        char imagepath[150];
        struct passwd *passwd;
        passwd = getpwuid (getuid());
        sprintf(imagepath,"%s/.pam-face-authentication/train/%s%d.jpg", passwd->pw_dir,buffer,numberOfFaces+1);
        // printf("%s \n",imagepath);
        cvSaveImage(imagepath,face);
        setGtkWebcamImageWhite(gtkWebcamImage);
        cvWaitKey(100);
        intializeGtkIconView();

    }

    loadCVPIXBUF(gtkWebcamImage,frame_copy);


    if ( cvWaitKey( 1 ) >= 0 )
        {}

    return TRUE;
}
*/
void
on_gtkAbout_clicked  (GtkButton *button,gpointer user_data)
{

    static const char *authors[] =
    {
        "Rohan Anil <rohan.anil@gmail.com>",
        "Birla Institute of Technology & Science Pilani - Goa Campus",
        " ",
        "Alex Lau <avengermojo@gmail.com>",
        "Novell, China",
        " ",
        "Nickolay V. Shmyrev <nshmyrev@yandex.ru>",
        "",

        NULL
    };

    static const char *artists[] = {"Anyone Interested? :) ",
                                    NULL
                                   };

    static const char *documenters[] =
    {
        "Anyone Interested? :) ",
        NULL
    };

    const char *translators;

    translators = "translator-credits";

    const char *license[] =
    {
        "This program is free software; you can redistribute it and/or modify "
        "it under the terms of the GNU General Public License as published by "
        "the Free Software Foundation; either version 3 of the License, or "
        "(at your option) any later version.\n",
        "This program is distributed in the hope that it will be useful, "
        "but WITHOUT ANY WARRANTY; without even the implied warranty of "
        "MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the "
        "GNU General Public License for more details.\n",
        "You should have received a copy of the GNU General Public License "
        "along with this program. If not, see <http://www.gnu.org/licenses/>."
    };

    char *license_trans;

    license_trans = g_strconcat (license[0], "\n", license[1], "\n", license[2], "\n", NULL);
    GdkPixbuf *logo;

    GError *err = NULL;
    logo =  gdk_pixbuf_new_from_file_at_size  (PKGDATADIR "/logo.png",182,182,&err);
    gtk_show_about_dialog (GTK_WINDOW (window),
                          "version", VERSION,
                           "name" , "Face Authentication \n",
                           "comments", "A Pluggable Face Authentication Module",
                           "authors", authors,
                           "translator-credits", translators,
                           "artists", artists,
                           "documenters", documenters,
                           "website", "http://code.google.com/p/pam-face-authentication/",
                           "website-label", "pam-face-authentication",
                           "logo", logo,
                           "wrap-license", TRUE,
                           "license", license_trans,
                           NULL);

    g_free (license_trans);
}
void
on_gtkSave_clicked  (GtkButton *button,gpointer user_data)
{
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(window,
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Training will take ~ (0 - 5) min's \n Press OK to Train", "title");
    gtk_window_set_title(GTK_WINDOW(dialog), "Face Training Alert");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);

    int i,j,k=0;
    float* featuresLBPAverage1 = (float*)calloc(  7*4 * 59 , sizeof(float) );
    float* featuresLBPAverage2 = (float*)calloc(  7*4 * 59 , sizeof(float) );
    float* featuresLBPAverage3 = (float*)calloc(  7*4 * 59 , sizeof(float) );

    float numberOfImages1=0;
    float numberOfImages2=0;
    float numberOfImages3=0;

    struct dirent *de=NULL;
    DIR *d=NULL;
    char dirpath[120];
    char temp[200];
    char featuresDCTpath[120];
    char featuresLBPpath[120];
    char featuresConfig[120];
    char fullimagepath[150];
    char LBPfullimagepath[150];
    struct passwd *passwd;
    passwd = getpwuid (getuid());
    int status;
    sprintf(dirpath,"%s/.pam-face-authentication/features", passwd->pw_dir);
    status = mkdir(dirpath, S_IRWXU );
    sprintf(featuresDCTpath,"%s/.pam-face-authentication/features/featuresDCT", passwd->pw_dir);
    sprintf(featuresLBPpath,"%s/.pam-face-authentication/features/featuresLBP", passwd->pw_dir);
    FILE *f1 =fopen(featuresDCTpath,"w");
    FILE *f2 =fopen(featuresLBPpath,"w");
    sprintf(dirpath,"%s/.pam-face-authentication/train/", passwd->pw_dir);
    d=opendir(dirpath);
    int fileCount=0;
    while (de = readdir(d))
    {
        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {
            fileCount++;
        }
    }

    int loopIndex=-1;
    // printf("%d \n",fileCount);
    double* featuresTotal = (double*)calloc(  fileCount , sizeof(double) );
    char** featuresTotalFilePath = (char**)calloc(  fileCount , sizeof(char *) );
    d=opendir(dirpath);
    while (de = readdir(d))
    {
        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {
            loopIndex++;
            featuresTotal[loopIndex]=0;
            sprintf(fullimagepath,"%s/.pam-face-authentication/train/%s", passwd->pw_dir,de->d_name);
            IplImage * img = cvLoadImage(fullimagepath,0);
            featureLBPSum(img,&featuresTotal[loopIndex]);
//printf("%e \n",featuresTotal[loopIndex]);
            featuresTotalFilePath[loopIndex]=(char*)calloc(strlen(fullimagepath), sizeof(char));


        }
    }
    double min=0;
    int minIndex=0;
    double max=0;
    int maxIndex=-1;
    if (fileCount>0)
        min=featuresTotal[0];
    for (i=0;i<fileCount;i++)
    {
        if (featuresTotal[i]>=max)
        {
            max=featuresTotal[i];
            //  printf("%e  \n",featuresTotal[i]);
// printf("%e  \n",max);

            maxIndex=i;
        }
        if (featuresTotal[i]<=min)
        {
            min=featuresTotal[i];
            //      printf("%e  \n",featuresTotal[i]);
// printf("%e  \n",min);

            minIndex=i;
        }
    }
    // printf("%e %e \n",min,max);
    double div1=min + (max-min)/3;
    double div2=min + (2*(max-min))/3;

    //   printf("%e %e \n",div1,div2);

    d=opendir(dirpath);
    double tempTotal=0;
    while (de = readdir(d))
    {
        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {


            sprintf(fullimagepath,"%s/.pam-face-authentication/train/%s", passwd->pw_dir,de->d_name);
            IplImage * img = cvLoadImage(fullimagepath,0);
            featureLBPSum(img,&tempTotal);
            if (tempTotal<=div1)
                numberOfImages1++;

            if (tempTotal>div1 && tempTotal<=div2)
                numberOfImages2++;
            if (tempTotal>div2)
                numberOfImages3++;
            // printf("%e %d loopIndex\n",featuresTotal[loopIndex],loopIndex);
            // printf("%s \n",fullimagepath);

            int Nx = floor((img->width - 4)/4);
            int Ny= floor((img->height - 4)/4);
            float* features = (float*)malloc(  Nx*Ny * 18 * sizeof(float) );
            featureDctMod2(img,features);

            int Nx1 = floor((img->width)/30);
            int Ny1= floor((img->height)/20);
            float* featuresLBP = (float*)malloc(  Nx1*Ny1 * 59 * sizeof(float) );
            featureLBPHist(img,featuresLBP);

            sprintf(temp,"%d",getuid());
            fputs(temp,f1);
            fputs(" ",f1);
            fputs(temp,f2);
            fputs(" ",f2);
            //  printf("%e %d \n",featuresTotal[loopIndex],loopIndex);

            for (i=0;i<Ny1;i++)
            {
                for (j=0;j<Nx1;j++)
                {
                    for (k=0;k<59;k++)
                    {
                        sprintf(temp,"%d", (i*59*Nx1 + j*59 +k));
                        fputs(temp,f2);
                        fputs(":",f2);
                        sprintf(temp,"%f",featuresLBP[i*59*Nx1 + j*59 +k]);
                        if (tempTotal<=div1)
                            featuresLBPAverage1[i*59*Nx1 + j*59 +k]+=featuresLBP[i*59*Nx1 + j*59 +k];

                        if (tempTotal>div1 && tempTotal<=div2)
                            featuresLBPAverage2[i*59*Nx1 + j*59 +k]+=featuresLBP[i*59*Nx1 + j*59 +k];

                        if (tempTotal>div2)
                            featuresLBPAverage3[i*59*Nx1 + j*59 +k]+=featuresLBP[i*59*Nx1 + j*59 +k];

                        fputs(temp,f2);
                        fputs(" ",f2);
                    }
                }
            }
//printf("%e %e %e \n",numberOfImages1,numberOfImages2,numberOfImages3);


            for (i=0;i<Ny;i++)
            {
                for (j=0;j<Nx;j++)
                {
                    for (k=0;k<6;k++)
                    {
                        sprintf(temp,"%d", (i*18*Nx + j*18 +k));
                        fputs(temp,f1);
                        fputs(":",f1);
                        sprintf(temp,"%f",features[i*18*Nx + j*18 +k]);
                        fputs(temp,f1);
                        fputs(" ",f1);
                    }

                }
            }
            fputs("\n",f1);
            fputs("\n",f2);
        }

    }

    fclose(f1);
    fclose(f2);

    for (i=0;i<7;i++)
    {
        for (j=0;j<4;j++)
        {
            for (k=0;k<59;k++)
            {
                if (numberOfImages1>0)
                    featuresLBPAverage1[i*59*4 + j*59 +k]=   featuresLBPAverage1[i*59*4 + j*59 +k]/numberOfImages1;
                if (numberOfImages2>0)
                    featuresLBPAverage2[i*59*4 + j*59 +k]=   featuresLBPAverage2[i*59*4 + j*59 +k]/numberOfImages2;
                if (numberOfImages3>0)
                    featuresLBPAverage3[i*59*4 + j*59 +k]=   featuresLBPAverage3[i*59*4 + j*59 +k]/numberOfImages3;
            }
        }
    }
    double MaxDistance1=-1;
    double MaxDistance2=-1;
    double MaxDistance3=-1;
    d=opendir(dirpath);
    while (de = readdir(d))
    {
        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {

            sprintf(fullimagepath,"%s/.pam-face-authentication/train/%s", passwd->pw_dir,de->d_name);
            IplImage * img = cvLoadImage(fullimagepath,0);
            featureLBPSum(img,&tempTotal);

            int Nx1 = floor((img->width)/30);
            int Ny1= floor((img->height)/20);
            float* featuresLBP = (float*)malloc(  Nx1*Ny1 * 59 * sizeof(float) );
            featureLBPHist(img,featuresLBP);
            double weights[7][4]  =
            {
                {.2,   1.75,  1.75, .2},
                { 1,  1.5, 1.5,  1},
                { 1,  2  ,   2,  1},
                {.5,  1.2, 1.2, .5},
                {.3,  1.2, 1.2, .3},
                {.3,  1.1, 1.1, .3},
                {.1,  .5,   .5, .1}
            };
            double distance1=0;
            double distance2=0;
            double distance3=0;

            for (i=0;i<7;i++)

            {
                for (j=0;j<4;j++)
                {
                    for (k=0;k<59;k++)
                    {
                        if (tempTotal<=div1)
                        {
                            distance1+=(weights[i][j]*pow((featuresLBPAverage1[i*59*4 + j*59 +k]-featuresLBP[i*59*4 + j*59 +k]),2))/((featuresLBPAverage1[i*59*4 + j*59 +k]+featuresLBP[i*59*4 + j*59 +k])+1);
                        }
                        if (tempTotal>div1 && tempTotal<=div2)
                        {
                            distance2+=(weights[i][j]*pow((featuresLBPAverage2[i*59*4 + j*59 +k]-featuresLBP[i*59*4 + j*59 +k]),2))/((featuresLBPAverage2[i*59*4 + j*59 +k]+featuresLBP[i*59*4 + j*59 +k])+1);
                        }
                        if (tempTotal>div2)
                        {
                            distance3+=(weights[i][j]*pow((featuresLBPAverage3[i*59*4 + j*59 +k]-featuresLBP[i*59*4 + j*59 +k]),2))/((featuresLBPAverage3[i*59*4 + j*59 +k]+featuresLBP[i*59*4 + j*59 +k])+1);
                        }

                    }
                }
            }
            //   printf("%e DIST \n",distance1);

            if (sqrt(distance1)>MaxDistance1)
            {
                //printf("TRUE \n");

                MaxDistance1=sqrt(distance1);
            }

            if (sqrt(distance2)>MaxDistance2)
                MaxDistance2=sqrt(distance2);

            if (sqrt(distance3)>MaxDistance3)
                MaxDistance3=sqrt(distance3);
//   printf("%e %e %e\n",MaxDistance1,MaxDistance2,MaxDistance3);

        }
    }

    sprintf(featuresConfig,"%s/.pam-face-authentication/features/featuresDistance", passwd->pw_dir);
    FILE *fd=fopen(featuresConfig,"w");
    sprintf(temp,"%e %e %e\n",MaxDistance1,MaxDistance2,MaxDistance3);
    fputs(temp,fd);
    fclose(fd);

    sprintf(featuresConfig,"%s/.pam-face-authentication/features/featuresAverage", passwd->pw_dir);
    fd=fopen(featuresConfig,"w");
    for (i=0;i<7;i++)
    {
        for (j=0;j<4;j++)
        {
            for (k=0;k<59;k++)
            {
                sprintf(temp,"%f",featuresLBPAverage1[i*59*4 + j*59 +k]);
                fputs(temp,fd);
                fputs(" ",fd);
            }
        }
    }
    fputs("\n",fd);
    for (i=0;i<7;i++)
    {
        for (j=0;j<4;j++)
        {
            for (k=0;k<59;k++)
            {
                sprintf(temp,"%f",featuresLBPAverage2[i*59*4 + j*59 +k]);
                fputs(temp,fd);
                fputs(" ",fd);
            }
        }
    }
    fputs("\n",fd);
    for (i=0;i<7;i++)
    {
        for (j=0;j<4;j++)
        {
            for (k=0;k<59;k++)
            {
                sprintf(temp,"%f",featuresLBPAverage3[i*59*4 + j*59 +k]);
                fputs(temp,fd);
                fputs(" ",fd);
            }
        }
    }
    fputs("\n",fd);
    fclose(fd);

    /*
    Append feature directory to  SYSCONFDIR "/pam-face-authentication/db.lst
    */

    sprintf(dirpath,"%s/.pam-face-authentication/features", passwd->pw_dir);

    char temptrainer[300];
    sprintf(temptrainer,"%s/pfatrainer %s",BINDIR,dirpath);
    system(temptrainer);

    /* NEED PROGRESS BAR */

    dialog = gtk_message_dialog_new(window,
                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                    GTK_MESSAGE_INFO,
                                    GTK_BUTTONS_OK,
                                    "Training is Complete! =)");
    gtk_window_set_title(GTK_WINDOW(dialog), "Face Training Alert");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);


}
/*

*/
void
gtkCaptureFace_clicked_cb  (GtkButton *button,gpointer user_data)
{
    double val;
    val=gtk_spin_button_get_value ((GtkSpinButton*)gtkCountFace);
    numberOfFaces=(int)val;
    //ŗ  printf("%d \n",numberOfFaces);
}
int
main (int argc, char *argv[])
{

    intialize();
    currentFrame=cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U,3);
    currentFrameDetected=cvCreateImage( cvSize(IMAGE_WIDTH,IMAGE_HEIGHT),IPL_DEPTH_8U,3);

    GstStateChangeReturn ret;
    GstElement *pipeline,*filter, *src,*csp,*queue1,*fakesink;
    gboolean link_ok;
    GMainLoop *loop;
    gtk_init(&argc, &argv);
    gst_init(&argc, &argv);


    char welcomeMessage[100];
    struct passwd *passwd;
    passwd = getpwuid ( getuid());
    sprintf(welcomeMessage,"Welcome %s!", passwd->pw_gecos);
    gtk_init (&argc, &argv);
    builder = gtk_builder_new ();
    gtk_builder_add_from_file (builder, XML_GTK_BUILDER_FACE_MANAGER, NULL);
    window = GTK_WIDGET (gtk_builder_get_object (builder, "gtk-facemanager"));
    gtkWelcome= (GTK_WIDGET (gtk_builder_get_object (builder, "gtkWelcome")));
    gtkIconView=GTK_ICON_VIEW(GTK_WIDGET (gtk_builder_get_object (builder, "gtkIconView")));
    gtkWebcamImage = GTK_WIDGET (gtk_builder_get_object (builder, "gtkWebcamImage"));
    gtkCountFace = (GTK_WIDGET (gtk_builder_get_object (builder, "gtkCountFace")));
    intializeGtkIconView();
    gtk_label_set_label (GTK_LABEL(gtkWelcome),welcomeMessage);
    //setGtkWebcamImageWhite(gtkWebcamImage);
    loop = g_main_loop_new (NULL, FALSE);

    pipeline = gst_pipeline_new ("my_pipeline");
    src = gst_element_factory_make ("v4l2src", NULL);
    if (!src)
        src = gst_element_factory_make ("v4lsrc", NULL);
    csp = gst_element_factory_make("ffmpegcolorspace", "csp");
    queue1 = gst_element_factory_make("queue", "queue1");
    fakesink = gst_element_factory_make("fakesink", "fakesink");
    gst_bin_add_many (GST_BIN (pipeline), src,csp,fakesink, NULL);
    filter = gst_caps_new_simple("video/x-raw-rgb",
                                 "width", G_TYPE_INT, IMAGE_WIDTH,
                                 "height", G_TYPE_INT, IMAGE_HEIGHT,NULL);
    if (!gst_element_link(src, csp))
        g_print ("Failed to link one or more elements!\n");
    link_ok=gst_element_link_filtered(csp, fakesink, filter);
    //if (!gst_element_link(queue1, fakesink))
    //  g_print ("Failed to link one or more elements!\n");

    g_timeout_add(40, (GSourceFunc) time_handler, (gpointer) window);

    g_object_set (G_OBJECT (fakesink), "signal-handoffs", TRUE, NULL);
    g_signal_connect (fakesink, "handoff", G_CALLBACK (cb_have_data), NULL);
    //g_signal_connect(G_OBJECT(gtkWebcamImage), "expose-event", G_CALLBACK(expose_cb), fakesink);
    //g_object_set(G_OBJECT(fakesink), "sync", FALSE, NULL);
    gtk_builder_connect_signals (builder, NULL);
    gtk_widget_show_all (window);

    ret = gst_element_set_state (pipeline, GST_STATE_PLAYING);
    gtk_main ();
    gst_element_set_state (pipeline, GST_STATE_NULL);
    gst_object_unref (pipeline);

    g_object_unref (G_OBJECT (builder));
    return 0;
}
