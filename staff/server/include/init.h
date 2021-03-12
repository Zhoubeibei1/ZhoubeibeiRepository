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

#include <sqlite3.h>

#define PORT 8888
#define IP "192.168.43.150"

#define SQL_FILE "./staff.db"
#define TABLE_LEN 10
struct shuju{
	int id;
	char passwd[20];
	char name[20];
	char address[100];
	int age;
	char sex;
	char number[20];
	int salary;
	char section[20];
	int juris;
}__attribute__((packed));

/*---------------trans_data.flag状态介绍-----------------------
 * 0x00:初始状态                                               
 * 0x10:注册初始状态,0x11:申请注册,0x12:完成注册,0x13:注册失败 
 * 0x20:登录初始状态,0x21:申请登录,0x22:登录成功,0x23:登录失败 
 * 0x30:普通用户登录态,0x31:查询信息,0x32:修改信息,0x33:返回错误
 * 0x34:账号查找,0x35:姓名查找,0x36:号码查找,
 * 0x38:循环接受数据,0x39:查询成功     
 * 0x3A:请求修改信息,0x3F:更新成功                        
 * 0x40:管理员用户登录态,0x41:账号查找,0x42:姓名查找,0x43:号码查找
 * 0x44:全部打印,0x45:循环接受数据
 * 0x50:添加用户,0x51:申请添加,0x52:添加成功,0x53:添加失败
 * 0x54:删除用户,0x55:申请删除,0x56:删除成功,0x57:删除失败
 * 0x58:修改信息,0x59:申请修改,0x5A:修改成功,0x5B:修改失败
 * 0x5C:查询信息,0x5D:申请查询,0x5E:查询成功,0x5F:查询失败
 *                             
 * *----------------------------------------------*/           


struct trans{
	unsigned char flag;
	char myerrno[256];
	char search[20];
	struct shuju addre;
}__attribute__((packed)) trans_data;

int sfd;

int init();
int network_init();
int sqlite_init();
int Myclose();


#endif //INIT_H__

