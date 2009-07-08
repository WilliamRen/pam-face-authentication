#include <verifier.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <pwd.h> /* getpwdid */
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "pam_face_defines.h"

#include <iostream>
#include <list>
#include <string>
#include <cctype>

using namespace std;
typedef struct
{
    int filterMaceFacePSLR;
    int filterMaceEyePSLR;
    int filterMaceInsideFacePSLR;
}config;

void setConfig(config *configuration,char * configDirectory);
config * getConfig(char *configDirectory);
int file_exists(const char* filename);


int peakToSideLobeRatio(CvMat*maceFilterVisualize,IplImage *img);
void cvShiftDFT(CvArr * src_arr, CvArr * dst_arr );
CvMat *computeMace(IplImage **img,int size);

char * verifier::createSetDir()
{
    time_t ltime;
    struct tm *Tm;
    struct timeval detail_time;

    char* setDir  = new char[200];
    char* uniqueName  = new char[200];
    ltime=time(NULL);
    Tm=localtime(&ltime);
    gettimeofday(&detail_time,NULL);
    sprintf(uniqueName,"%d%d%d%d%d%d%d%d%d",Tm->tm_year,Tm->tm_mon,Tm->tm_mday,Tm->tm_hour,Tm->tm_min,Tm->tm_sec,(detail_time.tv_usec/1000),detail_time.tv_usec,Tm->tm_wday);
    sprintf(setDir,"%s/.pam-face-authentication/faces/%s", userStruct->pw_dir,uniqueName);
    mkdir(setDir, S_IRWXU );
    return uniqueName;
}
verifier::verifier()
{
    uid_t   userID=getuid();
    userStruct=getpwuid(getuid());
    char init[300];
    sprintf(init,"%s/.pam-face-authentication", userStruct->pw_dir);

    sprintf(facesDirectory,"%s/.pam-face-authentication/faces", userStruct->pw_dir);
    sprintf(modelDirectory,"%s/.pam-face-authentication/model", userStruct->pw_dir);
    sprintf(configDirectory,"%s/.pam-face-authentication/config", userStruct->pw_dir);
    char maceConfig[300];
    sprintf(maceConfig,"%s/mace.xml", configDirectory);
    mkdir(init, S_IRWXU );
    mkdir(facesDirectory, S_IRWXU );
    mkdir(modelDirectory, S_IRWXU );
    mkdir(configDirectory, S_IRWXU );
    FILE* fp = fopen(maceConfig, "r");
    if (fp)
    {
        fclose(fp);
    }
    else
    {
// Default Config

        config  newConfig;
        newConfig.filterMaceEyePSLR=MACE_EYE_DEFAULT;
        newConfig.filterMaceFacePSLR=MACE_FACE_DEFAULT;
        newConfig.filterMaceInsideFacePSLR=MACE_INSIDE_FACE_DEFAULT;
        setConfig(&newConfig,configDirectory);
    }

}


verifier::verifier(uid_t   userID)
{
    userStruct=getpwuid(userID);
    sprintf(facesDirectory,"%s/.pam-face-authentication/faces", userStruct->pw_dir);
    sprintf(modelDirectory,"%s/.pam-face-authentication/model", userStruct->pw_dir);
    sprintf(configDirectory,"%s/.pam-face-authentication/config", userStruct->pw_dir);
    char maceConfig[300];
    sprintf(maceConfig,"%s/mace.xml", configDirectory);

    mkdir(facesDirectory, S_IRWXU );
    mkdir(modelDirectory, S_IRWXU );
    mkdir(configDirectory, S_IRWXU );
}

void verifier::createMaceFilter()
{
    //cvNamedWindow("eyeRight",1);

    allFaces* temp=getFaceImagesFromAllSet();
    if (temp->count>0)
    {
        IplImage ** eye=new IplImage *[temp->count];
        IplImage ** insideFace=new IplImage *[temp->count];


        CvMat *maceFilterVisualize;
        CvFileStorage* fs ;
        char modelname[300];
        sprintf(modelname,"%s/.pam-face-authentication/model/face_mace.xml", userStruct->pw_dir);
        maceFilterVisualize=computeMace(temp->faceImages,temp->count);
        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );

        sprintf(modelname,"%s/.pam-face-authentication/model/eye_mace.xml", userStruct->pw_dir);
        int index=0;
        for (index=0;index<temp->count;index++)
        {
            cvResetImageROI(temp->faceImages[index]);
            eye[index]=cvCreateImage(cvSize(140,60),8,temp->faceImages[index]->nChannels);
            cvSetImageROI(temp->faceImages[index],cvRect(0,0,140,60));
            cvResize(temp->faceImages[index], eye[index], CV_INTER_LINEAR );
        }
        maceFilterVisualize=computeMace(eye,temp->count);
        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );


        sprintf(modelname,"%s/.pam-face-authentication/model/inside_face_mace.xml", userStruct->pw_dir);

        for (index=0;index<temp->count;index++)
        {

            cvResetImageROI(temp->faceImages[index]);
            insideFace[index]=cvCreateImage(cvSize(80,105),8,temp->faceImages[index]->nChannels);
            cvSetImageROI(temp->faceImages[index],cvRect(30,45,80,105));
            cvResize(temp->faceImages[index], insideFace[index], CV_INTER_LINEAR );
            cvResetImageROI(temp->faceImages[index]);
        }

        maceFilterVisualize=computeMace(insideFace,temp->count);
        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );

        for (index=0;index<temp->count;index++)
        {
            cvReleaseImage(&eye[index]);
            cvReleaseImage(&insideFace[index]);

        }
        delete [] eye;
        delete [] insideFace;

    }
    else
    {
        char dirname[300];
        sprintf(dirname,"%s/face_mace.xml",modelDirectory);
        remove(dirname);
    }
}
void verifier::addFaceSet(IplImage **set,int size)
{
    char* dirNameUnique=createSetDir();
    char  dirName[300];
    sprintf(dirName,"%s/.pam-face-authentication/faces/%s", userStruct->pw_dir,dirNameUnique);


    int i=0;
    for (i=0;i<size;i++)
    {
        char filename[300];
        sprintf(filename,"%s/%d.jpg",dirName,i);
        cvSaveImage(filename,set[i]);
        cvReleaseImage(&set[i]);
    }

    createMaceFilter();
    delete [] set;
}

void verifier::removeFaceSet(char* setName)
{
    char dirname[300],filename[300];
    struct dirent *de=NULL;
    DIR *d=NULL;
    sprintf(dirname,"%s/%s",facesDirectory,setName);

    d=opendir(dirname);
    while (de = readdir(d))
    {
        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {
            sprintf(filename,"%s/%s",dirname,de->d_name);
            remove(filename);
        }
    }

    remove(dirname);
    createMaceFilter();
    //sprintf(dirname,"%s/%s_mace.xml",modelDirectory,setName);
    //remove(dirname);

}




int verifier::verifyFace(IplImage *faceMain)
{
    if (faceMain==0)
        return 0;
char temp[300];
sprintf(temp,"%s/face_mace.xml",modelDirectory);
if(file_exists(temp)==0)
return 0;


    IplImage * face= cvCreateImage( cvSize(140,150),8,faceMain->nChannels);
    cvResize( faceMain,face, CV_INTER_LINEAR ) ;
    struct dirent *de=NULL;
    DIR *d=NULL;
    d=opendir(modelDirectory);
    int k=0;
    while (de = readdir(d))
    {
        if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
        {
            if ((strcmp(de->d_name +strlen(de->d_name)-8, "mace.xml")==0))
            {
                k++;
                //  mylist->push_back (de->d_name);
            }
        }
    }

    if (k<3)
        return 0;


    IplImage * eye=cvCreateImage(cvSize(140,60),8,face->nChannels);
    cvSetImageROI(face,cvRect(0,0,140,60));
    cvResize(face, eye, CV_INTER_LINEAR );
    cvResetImageROI(face);

    IplImage * insideFace=cvCreateImage(cvSize(80,105),8,face->nChannels);
    cvSetImageROI(face,cvRect(30,45,80,105));
    cvResize(face, insideFace, CV_INTER_LINEAR );
    cvResetImageROI(face);

    config * newConfig=getConfig(configDirectory);
    char * macefilternamepath=new char[300];
    CvFileStorage * fileStorage;
    CvMat *maceFilterUser;
    sprintf(macefilternamepath,"%s/%s",modelDirectory,"face_mace.xml");
    fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
    maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
    int faceValue=peakToSideLobeRatio(maceFilterUser,face);
    cvReleaseFileStorage( &fileStorage );
    cvReleaseMat( &maceFilterUser );


    sprintf(macefilternamepath,"%s/%s",modelDirectory,"eye_mace.xml");
    fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
    maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);

    int eyeValue=peakToSideLobeRatio(maceFilterUser,eye);


    cvReleaseFileStorage( &fileStorage );
    cvReleaseMat( &maceFilterUser );

    sprintf(macefilternamepath,"%s/%s",modelDirectory,"inside_face_mace.xml");
    fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
    maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);

    int insideFaceValue=peakToSideLobeRatio(maceFilterUser,insideFace);
    //printf("%d PTSR of INSIDE FACE \n",v);
    cvReleaseFileStorage( &fileStorage );
    cvReleaseMat( &maceFilterUser );
  /*
    FILE* fp = fopen("/home/darksid3hack0r/value.txt", "a");

    if (fp)
    {
        fprintf(fp,"%d %d %d \n",faceValue,eyeValue,insideFaceValue);
    }
    fclose(fp);
    */

  //  printf("%d %d %d \n",insideFaceValue,faceValue,eyeValue);
    int count=0;
    if (newConfig->filterMaceInsideFacePSLR<=insideFaceValue)
        count++;
    if (newConfig->filterMaceFacePSLR<=faceValue)
        count++;
    if (newConfig->filterMaceEyePSLR<=eyeValue)
        count++;

    if (count>1)
        return 1;
    else
        return 0;


}

allFaces* verifier::getFaceImagesFromAllSet()
{
    setFace* faceSetStruct=getFaceSet();
    int i=0;
    char modelDir[300];
    struct dirent *de=NULL;
    DIR *d=NULL;
    list<string> * mylist=new list<string>;
    list<string>::iterator it;
    int k=0;
    for (i=0;i<faceSetStruct->count;i++)
    {
        sprintf(modelDir,"%s/%s",facesDirectory,faceSetStruct->setName[i]);
       // printf("%s \n",modelDir);
        d=opendir(modelDir);
        while (de = readdir(d))
        {
            if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
            {
                k++;
                char fullPath[300];
                sprintf(fullPath,"%s/%s",modelDir,de->d_name);
                mylist->push_back (fullPath);
            }
        }
        mylist->sort();
    }


    allFaces* newAllFaces=new allFaces;
    newAllFaces->count=0;
    if (k==0)
        return newAllFaces;
    newAllFaces->faceImages =new IplImage *[(int) mylist->size()];
    newAllFaces->count =(int) mylist->size();
    int index=0;

    for (it=mylist->begin(); it!=mylist->end(); ++it)
    {

        string l=*it;
        char *p;
        char * path=new char[300];
        p=&l[0];
        sprintf(path,"%s",p);
        newAllFaces->faceImages[index]=cvLoadImage(path,1);
        index++;

    }

    return newAllFaces;
}
setFace* verifier::getFaceSet()
{

    struct dirent *de=NULL;
    DIR *d=NULL;
    list<string> * mylist=new list<string>;
    list<string>::iterator it;
    d=opendir(facesDirectory);
    while (de = readdir(d))
    {
        if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
        {
            mylist->push_back (de->d_name);

        }
    }

    mylist->sort();

    setFace* setFaceStruct= new setFace;
    setFaceStruct->setName =new char *[(int) mylist->size()];
    setFaceStruct->setFilePathThumbnails =new char *[(int) mylist->size()];

    int k=0;
    for (it=mylist->begin(); it!=mylist->end(); ++it)
    {
        string l=*it;
        char *p;
        char * setName=new char[300];
        p=&l[0];
        sprintf(setName,"%s",p);
        setFaceStruct->setName[k]=setName;
        char * fileThumb=new char[300];
        sprintf(fileThumb,"%s/%s/1.jpg",facesDirectory,p);
        setFaceStruct->setFilePathThumbnails[k]=fileThumb;
        k++;
    }
    setFaceStruct->count=k;
    return setFaceStruct;
}
