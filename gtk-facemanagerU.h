
void intializeGtkIconView();
void on_gtkSave_clicked  (GtkButton *button,gpointer user_data);

int numberOfFaces=0;

IplImage *frame,*orginalFrame,*frameNew, *frame_copy = 0;
CvPoint pLeftEye;
CvPoint pRightEye;
char *XML_GTK_BUILDER_FACE_MANAGER=PKGDATADIR "/gtk-facemanagerU.xml";

enum
{
    COL_DISPLAY_NAME,
    COL_PIXBUF,
    NUM_COLS
};

GtkBuilder *builder;
GtkWidget *window;
GtkWidget *gtkWelcome;
GtkIconView *gtkIconView;
GtkWidget *gtkWebcamImage;
GtkWidget *gtkCountFace;
