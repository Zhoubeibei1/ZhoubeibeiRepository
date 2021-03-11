/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：epoll_work.h
*   创 建 者：zhoubeibei
*   创建日期：2021年03月09日
*   描    述：
*
================================================================*/


#ifndef EPOLL_WORK_H__
#define EPOLL_WORK_H__
typedef struct{
	int newfd;
	struct sockaddr_in cin;
}Info;

int network_epoll();
int register_account(int newfd);
int inquire_message(int newfd);
int modify_message(int newfd);
int login_account(int newfd);
int add_data(int newfd);
int delete_data(int newfd);
int modify_data(revents[i].data.fd);
int inquire_data(int newfd);
int copy_data(struct shuju *sdata,char* resultp);
int copy_data_all(struct shuju *sdata,char** resultp);

#endif //EPOLL_WORK_H__

