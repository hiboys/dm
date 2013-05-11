#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
using namespace std;

/*
	�����û���Ϣ
	��Ӱid
	��Ӱ����
*/
struct user_message{
	int movie_id;
	char movie_score;
};


/*
	���彫Ҫ�����Ƽ����û�����
	�����û���index.csv�е�λ��
	����һ���û�
	�Լ�����һ����Ӱ��������
*/
struct user_question{
	int positionInIndex;
	int whichUser;
	int whichMovie;
};

const int user_num = 5000;				//�����û���
const int user_dimension = 1000;		//�����Ӱά��
const int question_num = 145081;		//������Ҫ�Ƽ����û����ļ��е�����

/*
	��ȡ�û���Ϣ�������û���ʶ���Լ�����ÿ���û��������е�Ӱ������
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
	�Դ�index.csv�ж�ȡ�����ݽ�������ĺ���
	���û�id��С�ķ���ǰ��
	��ͬһ�û��Ľ�С��Ӱid��ǰ��
*/
bool cmp(user_question a,user_question b){
	bool result = true;
	if ((a.whichUser > b.whichUser) || (a.whichUser == b.whichUser && a.whichMovie > b.whichMovie) )
		result = false;
	return result;
}

/*
	��index.csv�ж�ȡ��Ҫ���ȵ��û�����
	�����ս�������
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
		question.whichUser = temp-1;				//�û�id�Ǵ�0��ʼ�����

		fscanf_s(infile,"%d",&temp);
		fscanf_s(infile,"%c",&ch);
		question.whichMovie = temp-1;				//��Ӱid�Ǵ�0��ʼ�����

		indexContent.push_back(question);
	}

	sort(indexContent.begin(),indexContent.end(),cmp);

	fclose(infile);
}


/*
	����������Эͬ����
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