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
#include "utils.h"

#include <iostream>
#include <list>
#include <string>
#include <cctype>


#define FACE_MACE_SIZE 32
#define EYE_MACE_SIZE 42
#define INSIDE_FACE_MACE_SIZE 42


using namespace std;

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

    setFace* temp=getFaceSet();
    for (int i=0;i<temp->count;i++)
    {


        IplImage ** eye=new IplImage *[temp->faceImages[i].count];
        IplImage ** insideFace=new IplImage *[temp->faceImages[i].count];
        CvMat *maceFilterVisualize;
        CvFileStorage* fs ;
        char modelname[300];
        sprintf(modelname,"%s/.pam-face-authentication/model/%s_face_mace.xml", userStruct->pw_dir,temp->setName[i]);
        maceFilterVisualize=computeMace(temp->faceImages[i].faces,temp->faceImages[i].count,FACE_MACE_SIZE);
        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );

        sprintf(modelname,"%s/.pam-face-authentication/model/%s_eye_mace.xml", userStruct->pw_dir,temp->setName[i]);
        int index=0;
        for (index=0;index<temp->faceImages[i].count;index++)
        {
            cvResetImageROI( temp->faceImages[i].faces[index]);
            eye[index]=cvCreateImage(cvSize(EYE_MACE_SIZE,EYE_MACE_SIZE),8, temp->faceImages[i].faces[index]->nChannels);
            cvSetImageROI( temp->faceImages[i].faces[index],cvRect(0,0,140,60));
            cvResize(  temp->faceImages[i].faces[index], eye[index], CV_INTER_LINEAR );
        }
        maceFilterVisualize=computeMace(eye,temp->faceImages[i].count,EYE_MACE_SIZE);
        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );


        sprintf(modelname,"%s/.pam-face-authentication/model/%s_inside_face_mace.xml", userStruct->pw_dir,temp->setName[i]);

        for (index=0;index<temp->faceImages[i].count;index++)
        {

            cvResetImageROI( temp->faceImages[i].faces[index]);
            insideFace[index]=cvCreateImage(cvSize(INSIDE_FACE_MACE_SIZE,INSIDE_FACE_MACE_SIZE),8, temp->faceImages[i].faces[index]->nChannels);
            cvSetImageROI( temp->faceImages[i].faces[index],cvRect(30,45,80,105));
            cvResize( temp->faceImages[i].faces[index], insideFace[index], CV_INTER_LINEAR );
            cvResetImageROI( temp->faceImages[i].faces[index]);
        }

        maceFilterVisualize=computeMace(insideFace,temp->faceImages[i].count,INSIDE_FACE_MACE_SIZE);
        fs = cvOpenFileStorage( modelname, 0, CV_STORAGE_WRITE );
        cvWrite( fs, "maceFilter", maceFilterVisualize, cvAttrList(0,0) );
        cvReleaseFileStorage( &fs );
        cvReleaseMat( &maceFilterVisualize );

        for (index=0;index<temp->faceImages[i].count;index++)
        {
            cvReleaseImage(&eye[index]);
            cvReleaseImage(&insideFace[index]);

        }
        delete [] eye;
        delete [] insideFace;

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
    while (de= readdir(d))
    {

        if (strcmp(de->d_name+strlen(de->d_name)-3, "jpg")==0)
        {

            sprintf(filename,"%s/%s",dirname,de->d_name);
           // printf("%s \n",filename);
            remove(filename);
        }
    }
    //  printf("No seggy \n");
    char maceFilters[300];
    sprintf(maceFilters,"%s/%s_face_mace.xml",modelDirectory,setName);
    remove(maceFilters);
    sprintf(maceFilters,"%s/%s_eye_mace.xml",modelDirectory,setName);
    remove(maceFilters);
    sprintf(maceFilters,"%s/%s_inside_face_mace.xml",modelDirectory,setName);
    remove(maceFilters);


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
    //  sprintf(temp,"%s/face_mace.xml",modelDirectory);
    //  if (file_exists(temp)==0)
    //     return 0;
    list<string> * mylistFace=new list<string>;
    list<string> * mylistEye=new list<string>;
    list<string> * mylistInsideFace=new list<string>;

    list<string>::iterator it;

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
            // printf("%s \n",de->d_name);
            if (((strcmp(de->d_name +strlen(de->d_name)-14, "_face_mace.xml")==0) && !(strcmp(de->d_name +strlen(de->d_name)-20, "inside_face_mace.xml")==0)))
            {
                k++;
                mylistFace->push_back (de->d_name);
            }
            if ((strcmp(de->d_name +strlen(de->d_name)-20, "inside_face_mace.xml")==0))
            {
                mylistInsideFace->push_back (de->d_name);
            }
            if ((strcmp(de->d_name +strlen(de->d_name)-12, "eye_mace.xml")==0))
            {
                mylistEye->push_back (de->d_name);
            }
        }
    }

    if (k==0)
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

    int maxFaceValue=0,maxEyeValue=0,maxInsideFaceValue=0;

    for (it=mylistFace->begin(); it!=mylistFace->end(); ++it)
    {
        string l=*it;
        char *p;
        char * fileName=new char[300];
        p=&l[0];
        sprintf(fileName,"%s",p);
        sprintf(macefilternamepath,"%s/%s",modelDirectory,fileName);
        fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
        maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
        int faceValue=peakToSideLobeRatio(maceFilterUser,face,FACE_MACE_SIZE);
        cvReleaseFileStorage( &fileStorage );
        cvReleaseMat( &maceFilterUser );
        if (maxFaceValue<faceValue)
            maxFaceValue=faceValue;

    }


    for (it=mylistEye->begin(); it!=mylistEye->end(); ++it)
    {
        string l=*it;
        char *p;
        char * fileName=new char[300];
        p=&l[0];
        sprintf(fileName,"%s",p);
        sprintf(macefilternamepath,"%s/%s",modelDirectory,fileName);
        fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
        maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
        int eyeValue=peakToSideLobeRatio(maceFilterUser,eye,EYE_MACE_SIZE);
        cvReleaseFileStorage( &fileStorage );
        cvReleaseMat( &maceFilterUser );
        if (maxEyeValue<eyeValue)
            maxEyeValue=eyeValue;
    }


    for (it=mylistInsideFace->begin(); it!=mylistInsideFace->end(); ++it)
    {
        string l=*it;
        char *p;
        char * fileName=new char[300];
        p=&l[0];
        sprintf(fileName,"%s",p);
        sprintf(macefilternamepath,"%s/%s",modelDirectory,fileName);
        fileStorage = cvOpenFileStorage(macefilternamepath, 0, CV_STORAGE_READ );
        maceFilterUser = (CvMat *)cvReadByName(fileStorage, 0, "maceFilter", 0);
        int insideFaceValue=peakToSideLobeRatio(maceFilterUser,insideFace,INSIDE_FACE_MACE_SIZE);
        //printf("%d PTSR of INSIDE FACE \n",v);
        cvReleaseFileStorage( &fileStorage );
        cvReleaseMat( &maceFilterUser );
        if (maxInsideFaceValue<insideFaceValue)
            maxInsideFaceValue=insideFaceValue;
    }


    /*
      FILE* fp = fopen("/home/darksid3hack0r/value.txt", "a");

      if (fp)
      {
          fprintf(fp,"%d %d %d \n",faceValue,eyeValue,insideFaceValue);
      }
      fclose(fp);
      */

    printf("The Values are  Face %d  Eye %d Nose+mouth  %d \n",maxFaceValue,maxEyeValue,maxInsideFaceValue);

    int count=0;
    if (newConfig->filterMaceInsideFacePSLR<=maxInsideFaceValue)
        count++;
    if (newConfig->filterMaceFacePSLR<=maxFaceValue)
        count++;
    if (newConfig->filterMaceEyePSLR<=maxEyeValue)
        count++;


    if (count>1)
    {
        //printf("\n YES \n");
        return 1;
    }
    else
        return 0;


}
/*
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

*/


setFace* verifier::getFaceSet()
{

    struct dirent *de=NULL;
    DIR *d=NULL;
    list<string> * mylist=new list<string>;
    list<string>::iterator it;
    d=opendir(facesDirectory);

    int count=0;
    while (de = readdir(d))
    {
        if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
        {
            mylist->push_back (de->d_name);
            count++;

        }
    }

    mylist->sort();

    setFace* setFaceStruct= new setFace;
    setFaceStruct->setName =new char *[(int) mylist->size()];
    setFaceStruct->setFilePathThumbnails =new char *[(int) mylist->size()];
    setFaceStruct->faceImages = new structFaceImages[(int) mylist->size()];
    setFaceStruct->count=count;


///./setFaceStruct->faceImages->count=imageK;




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
        char imagesDir[300];
        sprintf(imagesDir,"%s/%s",facesDirectory,p);
        int imageK=0;

        struct dirent *de=NULL;
        DIR *d=NULL;
        list<string> * mylistImages=new list<string>;
        list<string>::iterator itImages;
        d=opendir(imagesDir);


        while (de = readdir(d))
        {
            if (!((strcmp(de->d_name, ".")==0) || (strcmp(de->d_name, "..")==0)))
            {
                imageK++;
                char fullPath[300];
                sprintf(fullPath,"%s/%s",imagesDir,de->d_name);
                mylistImages->push_back (fullPath);
            }

        }

        mylistImages->sort();
        setFaceStruct->faceImages[k].faces= new IplImage* [imageK];
        setFaceStruct->faceImages[k].count=imageK;
        int imageIndex=0;
        for (itImages=mylistImages->begin(); itImages!=mylistImages->end(); ++itImages)
        {
            string l=*itImages;
            char *p;
            char * fileName=new char[300];
            p=&l[0];
            sprintf(fileName,"%s",p);
            setFaceStruct->faceImages[k].faces[imageIndex]=cvLoadImage(fileName,1);
            imageIndex++;
        }

        k++;
    }

    return setFaceStruct;
}
