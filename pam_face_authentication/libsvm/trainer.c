#include <stdio.h>
#include <stdlib.h>
#include <string.h>
double KC=8;
double Kgamma=0.0001220703125;
int svmScale(int argc,char **argv,FILE* fp1);
int
main (int argc, char *argv[])
{
    FILE *fp1,*fp2,*fp3,*fp4,*fp0;
    if ( !(fp0 = fopen("/etc/pam-face-authentication/db.lst", "r")) )
    {
        fprintf(stderr, "Error 1 Occurred Accessing db.lst\n");
        return 0;
    }

    fp3=fopen("/etc/pam-face-authentication/featuresDCT","wb");
    fp4=fopen("/etc/pam-face-authentication/featuresLBP","wb");
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



        char *buffer;
        unsigned long fileLen;

        //Open file


        //Get file length
        fseek(fp1, 0, SEEK_END);
        fileLen=ftell(fp1);
        fseek(fp1, 0, SEEK_SET);

        //Allocate memory
        buffer=(char *)malloc(sizeof(char)*(fileLen+1));
        printf("%d \n",fileLen);
        if (!buffer)
        {
            fprintf(stderr, "Memory error!");
            fclose(fp1);
            return -1;
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
        printf("%d \n",fileLen);
        if (!buffer)
        {
            fprintf(stderr, "Memory error!");
            fclose(fp2);
            return -1;
        }

        //Read file contents into buffer
        fread(buffer, fileLen, 1, fp2);
        fwrite ((char *)buffer ,fileLen , 1 , fp4 );

        //Do what ever with buffer

        free(buffer);





        fclose(fp1);
        fclose(fp2);
    }
    fclose(fp3);
    fclose(fp4);


/*

*/
char* argv1[4]={"svm-scale","-s","/etc/pam-face-authentication/featuresDCT.range","/etc/pam-face-authentication/featuresDCT"};
FILE *fp5=fopen("/etc/pam-face-authentication/featuresDCT.scale","w");
svmScale(4,argv1,fp5);

char* argv2[4]={"svm-scale","-s","/etc/pam-face-authentication/featuresLBP.range","/etc/pam-face-authentication/featuresLBP"};
FILE *fp6=fopen("/etc/pam-face-authentication/featuresLBP.scale","w");
svmScale(4,argv2,fp6);

//RUN SAVE MODEL MODULE
system("svm-train -t 2 -c 8 -g 0.0001220703125 -b 1 /etc/pam-face-authentication/featuresDCT.scale /etc/pam-face-authentication/featuresDCT.scale.model");
system("svm-train -t 2 -c 8 -g 0.0001220703125 -b 1 /etc/pam-face-authentication/featuresLBP.scale /etc/pam-face-authentication/featuresLBP.scale.model");


}
