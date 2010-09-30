#include <stdio.h>
#include <stdlib.h>
#include <string.h>
double KC=8;
double Kgamma=0.0001220703125;
int svmScale(int argc,char **argv,FILE* fp1);
void filewrite(FILE *fp1,FILE *fp2,FILE *fp3,FILE *fp4)
{
    char *buffer;
    unsigned long fileLen;
    fseek(fp1, 0, SEEK_END);
    fileLen=ftell(fp1);
    fseek(fp1, 0, SEEK_SET);
    buffer=(char *)malloc(sizeof(char)*(fileLen+1));
    if (!buffer)
    {
        fprintf(stderr, "Memory error!");
        fclose(fp1);
        exit(0);
    }
    //Read file contents into buffer
    fread(buffer, fileLen, 1, fp1);
    fwrite ((char *)buffer ,fileLen , 1 , fp3 );
    //Do what ever with buffer
    free(buffer);
    //Get file length
    fseek(fp2, 0, SEEK_END);
    fileLen=ftell(fp2);
    fseek(fp2, 0, SEEK_SET);
    //Allocate memory
    buffer=(char *)malloc(sizeof(char)*(fileLen+1));
    // printf("%d \n",fileLen);
    if (!buffer)
    {
        fprintf(stderr, "Memory error!");
        fclose(fp2);
        exit(0);
    }
    //Read file contents into buffer
    fread(buffer, fileLen, 1, fp2);
    fwrite ((char *)buffer ,fileLen , 1 , fp4 );
    //Do what ever with buffer
    free(buffer);
}
int
main (int argc, char *argv[])
{
    if (argv[1]==NULL)
        return -1;

    char filepath[250];
    FILE *fileKey;
    int ifExist=0;

    if ( !(fileKey = fopen( SYSCONFDIR "/pam-face-authentication/db.lst", "r")) )
    {
        fprintf(stderr, "Error 1 Occurred Accessing db.lst\n");
        exit(0);
    }
    while (fscanf(fileKey,"%s", filepath)!=EOF )
    {
        if (strcmp(filepath,argv[1])==0)
        {
            ifExist=1;
        }
    }
    fclose(fileKey);

    if (ifExist==0)
    {
        if ( !(fileKey = fopen( SYSCONFDIR "/pam-face-authentication/db.lst", "a")) )
        {
            fprintf(stderr, "Error 2 Occurred Accessing db.lst\n");
            exit(0);
        }
        fprintf(fileKey,argv[1]);
        fprintf(fileKey,"\n");
        fclose(fileKey);
    }

    FILE *fp1,*fp2,*fp3,*fp4,*fp0;
    if ( !(fp0 = fopen( SYSCONFDIR "/pam-face-authentication/db.lst", "r")) )
    {
        fprintf(stderr, "Error 1 Occurred Accessing db.lst\n");
        return 0;
    }

    fp3=fopen( SYSCONFDIR "/pam-face-authentication/featuresDCT","wb");
    fp4=fopen( SYSCONFDIR "/pam-face-authentication/featuresLBP","wb");
    char path[300];
    float ch;
    char LBPpath[300];
    char DCTpath[300];
    while (fscanf(fp0,"%s", path)!=EOF )
    {
        sprintf(LBPpath,"%s/featuresLBP", path);
        sprintf(DCTpath,"%s/featuresDCT", path);
        fp1=fopen(DCTpath,"rb");
        fp2=fopen(LBPpath,"rb");
        filewrite(fp1,fp2,fp3,fp4);
        fclose(fp1);
        fclose(fp2);
    }

    fp1=fopen(SYSCONFDIR "/pam-face-authentication/dummy/featuresDCT","rb");
    fp2=fopen(SYSCONFDIR "/pam-face-authentication/dummy/featuresLBP","rb");
    filewrite(fp1,fp2,fp3,fp4);
    fclose(fp1);
    fclose(fp2);


    fclose(fp3);
    fclose(fp4);


    /*

    */
    char* argv1[4]={"svm-scale","-s", SYSCONFDIR "/pam-face-authentication/featuresDCT.range", SYSCONFDIR "/pam-face-authentication/featuresDCT"};
    FILE *fp5=fopen( SYSCONFDIR "/pam-face-authentication/featuresDCT.scale","w");
    svmScale(4,argv1,fp5);

    char* argv2[4]={"svm-scale","-s", SYSCONFDIR "/pam-face-authentication/featuresLBP.range", SYSCONFDIR "/pam-face-authentication/featuresLBP"};
    FILE *fp6=fopen( SYSCONFDIR "/pam-face-authentication/featuresLBP.scale","w");
    svmScale(4,argv2,fp6);

    char* argv3[11]={"svm-train","-t","2","-c","8","-g","0.0001220703125","-b","1", SYSCONFDIR "/pam-face-authentication/featuresDCT.scale", SYSCONFDIR "/pam-face-authentication/featuresDCT.scale.model"};
    svmTrain(11,argv3);

    char* argv4[11]={"svm-train","-t","2","-c","8","-g","0.0001220703125","-b","1", SYSCONFDIR "/pam-face-authentication/featuresLBP.scale", SYSCONFDIR "/pam-face-authentication/featuresLBP.scale.model"};
    svmTrain(11,argv4);
}
