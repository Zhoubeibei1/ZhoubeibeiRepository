/*================================================================
 *   Copyright (C) 2021 hqyj Ltd. All rights reserved.
 *   
 *   文件名称：init.c
 *   创 建 者：zhoubeibei
 *   创建日期：2021年03月08日
 *   描    述：
 *
 ================================================================*/


#include "include.h"

sqlite3 * staff = NULL;
int init(){
	printf("开始初始化\n");
	if(network_init()<0){
		fprintf(stderr,"网络相关初始化失败\n");
		return -1;
	}
	if(sqlite_init()<0){
		fprintf(stderr,"数据库初始化失败\n");
		return -1;
	}
	printf("初始化成功\n");
	return 0;
}

int network_init(){
	int ret;
	sfd = socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0){
		perror("套接字创建失败");
		return -1;
	}
	int reuse = 1;
	ret = setsockopt(sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(reuse));
	if(ret<0){
		perror("设置快速复用失败");
		return -1;
	}
	struct sockaddr_in sin;
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	sin.sin_addr.s_addr = inet_addr(IP);
	ret = bind(sfd,(struct sockaddr*)&sin,sizeof(sin));
	if(ret<0){
		perror("绑定失败");
		return -1;
	}
	ret = listen(sfd,3);
	if(ret < 0){
		perror("监听文件创建失败");
		return -1;
	}
	return 0;
}
int sqlite_init()
{
	int ret;
	ret = sqlite3_open(SQL_FILE,&staff);
	if(ret){
		fprintf(stderr,"数据库打开失败:%s\n",sqlite3_errmsg(staff));
		return -1;
	}
	char sql[256] = "";
	char *errmsg = NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql,"create table if not exists datum(\
		id int primary key,\
			passwd char,\
			name char,\
			address char,\
			age int,\
			sex char,\
			number char,\
			salary int,\
			section char,\
			juris int)");
	ret = sqlite3_exec(staff,sql,NULL,NULL,&errmsg);
	if(ret){
		fprintf(stderr,"%s\n",sqlite3_errmsg(staff));
		return -1;
	}
	bzero(sql,sizeof(sql));
	sprintf(sql,"create table if not exists zaixianyonghu(\
		id int primary key,name char,juris int)");
	ret = sqlite3_exec(staff,sql,NULL,NULL,&errmsg);
	if(ret){
		fprintf(stderr,"%s\n",sqlite3_errmsg(staff));
		return -1;
	}
	//清空在线用户表格
	errmsg = NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql,"delete from zaixianyonghu");
	if(sqlite3_exec(staff,sql,NULL,NULL,&errmsg)!=0){
		fprintf(stderr,"sqlite3_exec:%s\n",errmsg);
		return -1;
	}
	printf("数据库打开成功\n");
	return 0;
}
int Myclose()
{
	int ret;
	close(sfd);
	ret = sqlite3_close(staff);
	if(ret){
		fprintf(stderr,"数据库关闭失败:%s\n",sqlite3_errmsg(staff));
		return -1;
	}
	return 0;
}

