/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：pnetwork.c
*   创 建 者：zhoubeibei
*   创建日期：2021年03月08日
*   描    述：
*
================================================================*/


#include "include.h"

extern sqlite3 * staff;

int network_pthread(){
	int ret;
	struct sockaddr_in cin;
	socklen_t cin_len = sizeof(cin);
	int cntfd = -1;
	cntfd = accept(sfd,(struct sockaddr*)&cin,&cin_len);
	printf("%s:%d:%s\n",__FILE__,__LINE__,__func__);
	if(cntfd<0){
		perror("accept error");
		return -1;
	}
	printf("客户端链接成功\n");
	pthread_t tid = -1;
	Info netInfo;
	netInfo.newfd = cntfd;
	netInfo.cin = cin;
	printf("cin1:%d\n",cntfd);
	printf("%s:%d:%s\n",__FILE__,__LINE__,__func__);
	ret = pthread_create(&tid,NULL,pnetwork,(void*)&netInfo);
	if(ret<0){
		perror("pthread create");
		return -1;
	}
	return 0;
}

void* pnetwork(void* arg){
	printf("%s:%d:%s\n",__FILE__,__LINE__,__func__);
	int ret;
	ret = pthread_detach(pthread_self());
	if(ret){
		perror("pthread detach");
		return NULL;
	}
	printf("%s:%d:%s\n",__FILE__,__LINE__,__func__);
	Info cli = *(Info*)arg;
	int newfd = cli.newfd;
	struct sockaddr_in cin = cli.cin;
	printf("cin1:%d\n",newfd);

	char ip[20] = "";
	inet_ntop(AF_INET,&cin.sin_addr,ip,20);
	unsigned int port; 
	port = ntohs(cin.sin_port);
	while(1){
		printf("%s:%d:%s\n",__FILE__,__LINE__,__func__);
		memset(&trans_data,0,sizeof(trans_data));                  
		do{  
			printf("***********\n");
		    ret = recv(newfd,&trans_data,sizeof(trans_data),0);
			printf("-------\n");
		}while(ret < 0 && EINTR == errno);
		if(ret<0){
			perror("recv");
			return NULL;
		}else if(ret == 0){
			printf("用户断开连接\n");
			return NULL;
		}
		printf("flag:%d",trans_data.flag);
		if(trans_data.flag==0x11){
			if(register_account(newfd)<0){
				printf("用户注册失败\n");
			}
		}
	}
}
int register_account(int newfd)
{
	int ret;
	printf("开始注册\n");
#if 0
	if(data_detection()<0){
		trans_data.flag = 0x13;
		fprintf(stderr,"数据非法\n");
		do{                                               
		    ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);                    
		return -1;
	}
#endif
	char sql[256];
	char *errmsg = NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql,"insert into datum (id,passwd,juris) values(%d,\"%s\",%d)",
			trans_data.addre.id,trans_data.addre.passwd,trans_data.addre.juris);
	ret = sqlite3_exec(staff,sql,NULL,NULL,&errmsg);
	if(ret){
		bzero(trans_data.myerrno,sizeof(trans_data.myerrno));
		fprintf(stderr,"%s\n",sqlite3_errmsg(staff));
		trans_data.flag = 0x13;
		sprintf(trans_data.myerrno,"用户已存在\n");
		do{                                               
		    ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);                    
		return -1;
	}
	memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
	trans_data.flag = 0x12;
	do{                                               
		ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
	}while(ret<0&&errno == EINTR);    
	return 0;
}
int data_detection()
{
	int ret=0;
	memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
	if(trans_data.addre.age<0||trans_data.addre.age>150){
		trans_data.flag=0xff;
		sprintf(trans_data.myerrno,"年龄超出范围\n");
		ret = -1;
	}
	if(strlen(trans_data.addre.number)!=11){
		strcpy(trans_data.myerrno,"号码非法\n");
		ret = -1;
	}
	if(ret){
		return -1;
	}
	return 0;
}





