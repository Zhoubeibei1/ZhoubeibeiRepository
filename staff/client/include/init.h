/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：init.h
*   创 建 者：zhoubeibei
*   创建日期：2021年03月08日
*   描    述：
*
================================================================*/


#ifndef INIT_H__
#define INIT_H__

#define PORT 8888
#define IP "192.168.43.150"


int init();
int network_init();
int Myclose();
int pthread_init();                              

#endif //INIT_H__

