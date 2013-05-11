#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace std;

/*
	定义用户信息
	电影id
	电影评分
*/
struct user_message{
	int movie_id;
	char movie_score;
};


/*
	定义将要进行推荐的用户集合
	包括用户在index.csv中的位置
	是哪一个用户
	以及对哪一部电影进行评分
*/
struct user_question{
	int positionInIndex;
	int whichUser;
	int whichMovie;
};

const int user_num = 5000;				//定义用户数
const int user_dimension = 1000;		//定义电影维度
const int question_num = 145081;		//定义需要推荐的用户在文件中的行数

/*
	读取用户信息，包括用户标识，以及对于每个用户，对所有电影的评分
*/
void readUserMessage(vector<user_message> &users){
	FILE *inFile;
	fopen_s(&inFile,"data.csv","r");

	char score = NULL;
	user_message user_temp;
	
	for (int i=0;i<user_num;i++){
		for (int j=0;j<user_dimension;j++){
			fscanf_s(inFile,"%c",&score);
			user_temp.movie_id = j;
			user_temp.movie_score = score;
			users.push_back(user_temp);
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


/*
	计算基于项的协同过滤
*/
int main(){
	vector<user_message> users;
	vector<user_question> indexContent;

	read_question_set(indexContent);
	readUserMessage(users);

	

	users.clear();
	indexContent.clear();
	return 0;
}