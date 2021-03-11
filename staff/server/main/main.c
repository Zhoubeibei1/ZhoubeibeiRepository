/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：main.c
*   创 建 者：zhoubeibei
*   创建日期：2021年03月08日
*   描    述：
*
================================================================*/


#include "include.h"

void my_func(int sign_no){                
	if(sign_no==SIGINT||sign_no==SIGQUIT){
		printf("已完全关闭服务端\n");     
		Myclose();                        
		kill(getpid(),9);                 
	}                                     
}                                         

int main(int argc, char *argv[])
{
	signal(SIGINT,my_func); 
	signal(SIGQUIT,my_func);
	printf("代码开始运行\n");
	if(init()<0){
		perror("初始化失败");
		return -1;
	}
	while(1){
		if(network_epoll()<0){
			perror("客户端链接失败");
			return -1;
		}
	}
	Myclose();
	return 0;
}

