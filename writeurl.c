#include<stdio.h>
#include<stdlib.h>
#include<time.h>

int main(int argc,char** argv)
{
	FILE *fp;
	if((fp=fopen(argv[1],"w"))==NULL)
	{
		printf("file open error!\n");
		return 0;
	}
	int i=0,j;
	while(i<1000)
	{
		fprintf(fp,"%d.html %d\n",i,i);
		i++;
	}
	fprintf(fp,"\n");
	srand((int)time(0));
 	for(i=0;i<1000;i++)
		fprintf(fp,"%d %d\n",0,i);
	for(i=1;i<1000;i++)
		for(j=0;j<1000;j++)
		{
			double ran=(double)rand()/RAND_MAX;
			if(ran<0.1)
				fprintf(fp,"%d %d\n",i,j);
		}
	return 0;
}
