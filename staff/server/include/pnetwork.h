/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：pthread.h
*   创 建 者：zhoubeibei
*   创建日期：2021年03月08日
*   描    述：
*
================================================================*/


#ifndef PTHREAD_H__
#define PTHREAD_H__

typedef struct{
	int newfd;
	struct sockaddr_in cin;
}Info;

int network_pthread();
void* pnetwork(void* arg);
int register_account(int newfd);
int data_detection();

#endif //PTHREAD_H__

