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

int init(){
	printf("开始初始化\n");
	if(network_init()<0){
		fprintf(stderr,"网络相关初始化失败\n");
		return -1;
	}
	if(pthread_init()<0){
		fprintf(stderr,"线程初始化失败\n");
		return -1;
	}
	printf("初始化成功\n");
	return 0;
}

int network_init(){
	int ret;
	sfd = socket(AF_INET,SOCK_STREAM,0);
	printf("ret:%d",sfd);
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
	socklen_t sin_len = sizeof(sin);
	ret = connect(sfd,(struct sockaddr*)(&sin),sin_len);
	if(ret){
		perror("connect:");
		return -1;
	}
	return 0;
}
int Myclose()
{
	close(sfd);
	return 0;
}
int pthread_init()                               
{                                                
    //创建线程                                   
	pthread_t tid = -1;                          
	if(pthread_create(&tid,NULL,my_recv,NULL)<0){
		perror("pthread_create");                
		return -1;                               
	}                                            
	return 0;                                    
}                                                

