#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#define maxrownum 200000
#define maxcolnum 800
#define maxurllen 100
#define maxcoosize 5000000 //以COO存储的节点个数
#define a 0.15      //阻尼系数
#define e 0.0000001   //精度

//COO稀疏矩阵,ell溢出的数据按序存储在coo
typedef struct{
    int row[maxcoosize],col[maxcoosize];
    double value[maxcoosize];
    int totalnum; //总节点数
}Triple;


Triple G1;  //coo压缩矩阵
int    g[maxrownum][maxcolnum]={{0,},};    //ELL压缩矩阵索引
double G[maxrownum][maxcolnum]={{0,},};   //ELL压缩矩阵value
int indegree[maxrownum]={0};           //入度矩阵
int outdegree[maxrownum]={0};          //出度矩阵
double V[maxrownum]={0};                //pagerank向量
double Vtemp[maxrownum]={0};             //作乘积用
char urls[maxrownum][maxurllen];          //存储url，下标为编号

void topTen(char* wfilename,int N);

int main(int argc, char** argv)
{

	memset(&G1,0,sizeof(G1));
	int row_num=0;           //实际行数
	int edges_num=0;      //总边数
    FILE *fpu;

    if(argc<3)
    {
        printf("parameter error!\n");
        return 0;
    }
	if((fpu=fopen(argv[1],"r"))==NULL)
	{
		printf("file %s open failed!\n",argv[1]);
		return 0;
	}
	//读取文件
	int flag=0;          //flag=0读取url部分，flag=1读取编号部分
	char line[200];      //存储一行数据
	char url[maxurllen];  //url
	int url_num;         //url编号
	int i,j,col;
	while(!feof(fpu))
	{
        if(flag==0)
		{
 			fgets(line,200,fpu);
			if(strcmp(line,"\n")!=0)
			{
				sscanf(line,"%s %d",url,&url_num);
				strcpy(urls[url_num],url);
				row_num++;
			}
			else
				flag=1;
		}
		else
		{
		    fgets(line,200,fpu);
		    if(strcmp(line,"\n")==0)
                continue;
			sscanf(line,"%d %d",&i,&j);
			col=indegree[i];
			if(col<maxcolnum)  //列数没超maxcolnum,存到ell
            {
                g[i][col]=j;
                G[i][col]=1;
            }
			else       //否则存到coo
            {
                G1.row[G1.totalnum]=i;
                G1.col[G1.totalnum]=j;
                G1.value[G1.totalnum]=1;
                G1.totalnum++;
            }
            indegree[i]+=1;
            outdegree[j]+=1;
            edges_num+=1;
		}
	}

    //初始化V
    for(i=0;i<row_num;i++)
    {
            V[i]=(double)indegree[i]/edges_num;
            Vtemp[i]=V[i];
    }

	//turn G to Gm to A
	int offset=0;  //计数coo位移
	for(i=0;i<row_num;i++)
    {
        for(j=0;j<indegree[i];j++)
        {
            if(j<maxcolnum)     //ell
            {
                G[i][j]=G[i][j]/outdegree[g[i][j]];
                G[i][j]=(1-a)*G[i][j]+a/row_num;
            }
            else               //coo
            {
                G1.value[offset]=G1.value[offset]/outdegree[G1.col[offset]];
                G1.value[offset]=(1-a)*G1.value[offset]+a/row_num;
                offset++;
            }
        }
    }


    //迭代计算V，Power Iteration幂迭代
	double E=1,Etemp;
	int num=0;   //计数power次数
	while(E>e)
	{
		num++;
		for(i=0;i<row_num;i++)
		{
			Vtemp[i]=V[i];
			V[i]=0;
		}
		E=0;
		offset=0;
		for(i=0;i<row_num;i++)
        {
            int k=0;
            for(j=0;j<row_num;j++)
            {
                if(k<maxcolnum && j==g[i][k])
                {
                    V[i]+=G[i][k]*Vtemp[j];
                    k++;
                }
                else if(k>=maxcolnum && j==G1.col[offset])
                {
                    V[i]+=G1.value[offset]*Vtemp[j];
                    offset++;
                }
                else
                {
                    V[i]+=a/row_num*Vtemp[j];
                }
            }
            //计算精度
            Etemp=fabs(V[i]-Vtemp[i]);
            if(Etemp>E)
                E=Etemp;
        }
        printf("%d:%.10f\n",num,E);
	}
    printf("power次数: %d\n",num);
    topTen(argv[2],row_num);

	return 0;
}

void topTen(char* wfilename,int N)
{
    FILE *fpt;
    if((fpt=fopen(wfilename,"w"))==NULL)
	{
		printf("file %s open failed!\n",wfilename);
		return;
	}

    int i,j,top;
    double top_value;
    for(i=0;i<10;i++)
    {
        top_value=0;
        for(j=0;j<N;j++)
        {
            if(V[j]>top_value)
            {
                top=j;
                top_value=V[j];
            }
        }
        fprintf(fpt,"%-80s %.10f\n",urls[top],top_value);
        V[top]=0;
    }
    fclose(fpt);
}
