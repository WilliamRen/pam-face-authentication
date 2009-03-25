#include <stdio.h>
#include <stdlib.h>
int parseSvmPrediction(int *ans,double *percent)
{
    char line[1000];
    int userid[200];
    int answer;
    double percentage;
 FILE *f;
 int i=0;
 f=fopen(SYSCONFDIR "/pam-face-authentication/prediction","r");
 fgets(line, 1000,f);
 //printf("%s \n",line);

 char* word1;
 word1=strtok(line,"  \n");
 word1=strtok(NULL,"  \n");


while(word1!=NULL)
{
  userid[i]=atoi(word1);
  i++;
  word1=strtok(NULL,"  \n");
}


fgets(line, 1000,f);
//printf("%s \n",line);
word1=strtok(line,"  \n");
answer=atoi(word1);

 int k;
 for(k=0;k<i;k++)
 {
     if(answer==userid[k])
     break;
 }

 int j=0;
while(j<k)
{word1=strtok(NULL,"  \n");
j++;
}

word1=strtok(NULL,"  \n");
//printf("%s \n",word1);
percentage=(double)atof(word1);
*ans=answer;
*percent=percentage;
return i;

//printf("answer %d percentage %e \n",answer,percentage);

}
