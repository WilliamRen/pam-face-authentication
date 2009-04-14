




static CvScalar colors[] =
{
    {{0,0,255}},
    {{0,128,255}},
    {{0,255,255}},
    {{0,255,0}},
    {{255,128,0}},
    {{255,255,0}},
    {{255,0,0}},
    {{255,0,255}}
};




char *HAAR_CASCADE_FACE=PKGDATADIR "/haarcascade.xml";
char *HAAR_CASCADE_EYE=PKGDATADIR "/haarcascade_eye_tree_eyeglasses.xml";
char *HAAR_CASCADE_EYE_2=PKGDATADIR "/haarcascade_eye.xml";

static CvMemStorage* storage = 0;
static CvHaarClassifierCascade* cascade = 0;
static CvHaarClassifierCascade* nested_cascade = 0;
static CvHaarClassifierCascade* nested_cascade_2 = 0;

double scale = 1;
int widthEye=0;
int widthFace=0;


void allocateMemory();
void intialize();
int faceDetect( IplImage* ,CvPoint *,CvPoint *);
void rotate(double angle, float centreX, float centreY,IplImage * img,IplImage * dstimg);
void rotatePoint(CvPoint* srcP,CvPoint* dstP,float angle);
int preprocess(IplImage * img,CvPoint plefteye,CvPoint prighteye,IplImage * face);
double CenterofMass(IplImage* src,int flagXY);
int CheckImageROI(IplImage* img,double x,double y,double width,double height,double fun);
