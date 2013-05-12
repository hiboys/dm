#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <algorithm>
using namespace std;

const int user_size = 5000;						//定义用户数
const int item_size = 1000;						//定义项数
double sim[item_size][item_size] = {0};			//定义项目之间的相似度
const int question_num = 145081;				//定义需要推荐的用户在文件中的行数
char user[user_size][item_size] = {NULL};		//定义用户信息，即用户对于每部电影的评分
vector<double> ex,ex2;							//定义在每一列中的项的期望/均值


/*
	定义将要进行推荐的用户集合
	包括用户在index.csv中的位置
	是哪一个用户
	以及对哪一部电影进行评分
	以及最后计算出来的评分数
*/
struct user_question{
	int positionInIndex;
	int whichUser;
	int whichMovie;
	char theWantScore;
};

/*定义一个结构体，显示的是电影id，即相似度*/
struct simStruct{
	int movie_id;
	double sim_level;
};

/*读用户数据文件*/
void readDataFile(){
	FILE *inFile;
	fopen_s(&inFile,"data.csv","r");

	char score = NULL;
	
	for (int i=0;i<user_size;i++){
		for (int j=0;j<item_size;j++){
			fscanf_s(inFile,"%c",&score);
			user[i][j] = score;
			fscanf_s(inFile,"%c",&score);
		}
	}

	fclose(inFile);
}

/*
	对从index.csv中读取的数据进行排序的函数
	将用户id较小的放在前面
	将同一用户的较小电影id放前面
*/
bool cmp(user_question a,user_question b){
	bool result = true;
	if ((a.whichUser > b.whichUser) || (a.whichUser == b.whichUser && a.whichMovie > b.whichMovie) )
		result = false;
	return result;
}

/*
	从index.csv中读取将要评比的用户数据
	并最终进行排序
*/
void read_question_set(vector<user_question> &indexContent){
	FILE *infile;
	fopen_s(&infile,"index.csv","r");

	int temp = 0;
	char ch = NULL;
	user_question question;

	for (int i=0;i<question_num;i++){
		question.positionInIndex = i;

		fscanf_s(infile,"%d",&temp);
		fscanf_s(infile,"%c",&ch);
		question.whichUser = temp-1;				//用户id是从0开始定义的

		fscanf_s(infile,"%d",&temp);
		fscanf_s(infile,"%c",&ch);
		question.whichMovie = temp-1;				//电影id是从0开始定义的

		indexContent.push_back(question);
	}

	sort(indexContent.begin(),indexContent.end(),cmp);

	fclose(infile);
}

/*基于项的协同过滤*/

/*计算相似度*/
void getSim(){
	//获取每一个项的均值，以及平方后的均值，即E(x),E(x2)
	for (int i=0;i<item_size;i++){
		double sum1 = 0.00,sum2 = 0.00,temp = 0.00;
		for (int j=0;j<user_size;j++){
			temp = (double) (user[j][i]-'0');
			sum1+= temp;
			sum2 += pow(temp,2);
		}
		ex.push_back(sum1/user_size);
		ex2.push_back(sum2/user_size);
	}
	FILE *outfile;
	fopen_s(&outfile,"exy.csv","w");

	for (int i=0;i<item_size;i++){
		for (int j=i+1;j<item_size;j++){
			double sum1 = 0.00,exy = 0.00;
			for (int k=0;k<user_size;k++){
				sum1 += (user[k][i]-'0')*(user[k][j]-'0');
			}
			exy = sum1/user_size;

			sim[i][j] = (exy-ex[i]*ex[j])/(sqrt((ex2[i]-pow(ex[i],2))*(ex2[j]-pow(ex[j],2))));
			sim[j][i] = sim[i][j];
			fprintf(outfile,"%lf,",exy);
		}
		fprintf(outfile,"\n");
	}

	fclose(outfile);
}

/*将相似度写入磁盘*/
void writeSimToDisk(){
	FILE *outFile;
	fopen_s(&outFile,"sim.csv","w");

	for (int i=0;i<item_size;i++){
		for (int j=0;j<item_size-1;j++)
			fprintf_s(outFile,"%lf,",sim[i][j]);
		fprintf_s(outFile,"%lf\n",sim[i][item_size-1]);
	}

	fclose(outFile);

	fopen_s(&outFile,"ex.csv","w");

	for (int i=0;i<item_size;i++){
		fprintf_s(outFile,"%lf\n",ex[i]);
	}

	fclose(outFile);

	fopen_s(&outFile,"ex2.csv","w");

	for (int i=0;i<item_size;i++){
		fprintf_s(outFile,"%lf\n",ex2[i]);
	}

	fclose(outFile);
}

/*用于相似度的排序*/
bool cmpp(simStruct a,simStruct b){
	return a.sim_level - b.sim_level > 0.0000001;
}

/*用于结果的排序*/
bool cmpp1(user_question a,user_question b){
	return a.positionInIndex < b.positionInIndex;
}
void getNeighbor(vector<user_question> &indexContent){
	FILE *infile;
	char ch = NULL;
	double temp = 0.00;

	vector<simStruct> simSet[item_size];
	simStruct tempSim;
	fopen_s(&infile,"sim.csv","r");
	for (int i=0;i<item_size;i++){
		for (int j=0;j<item_size;j++){
			fscanf_s(infile,"%lf%c",&temp,&ch);
			tempSim.movie_id = j;
			tempSim.sim_level = temp;
			simSet[i].push_back(tempSim);
		}
		sort(simSet[i].begin(),simSet[i].end(),cmpp);
	}

	fclose(infile);

	for (int i=0;i<question_num;i++){
		int whichUser = indexContent[i].whichUser;
		int whichMovie = indexContent[i].whichMovie;

		int count = 0;
		double sum1 = 0.00,sum2 = 0.00,result = 0.00;

		for (int j=0;j<item_size-1 && count <= 100;j++){
			int simMovie = simSet[whichMovie][j].movie_id;
			double simLevel = simSet[whichMovie][j].sim_level;

			if (user[whichUser][simMovie] != '0'){
				count ++;
				sum1 += (simLevel*(user[whichUser][simMovie]-'0'-ex[simMovie]));
				sum2 += abs(simLevel);
			}
		}
		
		result = ex[whichMovie] + sum1/sum2;
		indexContent[i].theWantScore = ((int) (result+0.5))+'0';
		if (indexContent[i].theWantScore == '6')
			indexContent[i].theWantScore = '5';
	}

	sort(indexContent.begin(),indexContent.end(),cmpp1);

	FILE *outfile;
	fopen_s(&outfile,"test.csv","w");
	for (int i=0;i<question_num;i++){
		fprintf_s(outfile,"%c\n",indexContent[i].theWantScore);
	}
	fclose(outfile);

	for (int i= 0;i<item_size;i++)
		simSet[i].clear();
}
int main(){
	/*读取数据集合*/
	readDataFile();

	/*读取将要评论的集合*/
	vector<user_question> indexContent;
	read_question_set(indexContent);

	getSim();
	writeSimToDisk();
	getNeighbor(indexContent);

	indexContent.clear();
	ex.clear();
	ex2.clear();
	return 0;
}