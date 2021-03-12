/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reserved.
*   
*   文件名称：menu.h
*   创 建 者：zhoubeibei
*   创建日期：2021年03月09日
*   描    述：
*
================================================================*/


#ifndef MENU_H__
#define MENU_H__

#define MIKEY 10000


struct shuju{                                  
	int id;            //账号                                  
	char passwd[20];   //密码                           
	char name[20];     //姓名                          
	char address[100]; //地址                    
	int age;           //年龄                        
	char sex;          //性别                        
	char number[20];   //手机号码                        
	int salary;        //工资             
	char section[20];  //部门
	int juris;         //权限                        
}__attribute__((packed));                      
                                               
/*---------------trans_data.flag状态介绍-----------------------      
 * 0x00:初始状态,0x01:退出                                              
 * 0x10:注册初始状态,0x11:申请注册,0x12:完成注册,0x13:注册失败
 * 0x20:登录初始状态,0x21:申请登录,0x22:登录成功,0x23:登录失败
 * 0x30:普通用户登录态,0x31:查询信息,0x32:修改信息  
 * 0x34:账号查找,0x35:姓名查找,0x36:号码查找
 * 0x38:循环接受数据,0x39:查询成功
 * 0x3A:请求修改信息
 * 0x40:管理员用户登录态,0x41:账号查找,0x42:姓名查找,0x43:号码查找
 * 0x50:添加用户,0x51:申请添加,0x52:添加成功,0x53:添加失败
 * 0x54:删除用户,0x55:申请删除,0x56:删除成功,0x57:删除失败
 * 0x58:修改信息,0x59:申请修改,0x5A:修改成功,0x5B:修改失败
 * 0x5C:查询信息,0x5D:申请查询,0x5E:查询成功,0x5F:查询成功
 *
 * *----------------------------------------------*/          

struct trans{                                  
	unsigned char flag;                                  
	char myerrno[256];
	char search[20];
	struct shuju addre;                        
}__attribute__((packed)) trans_data;           

int menu();
int info_login();
void* my_recv(void* arg);
int print(struct shuju pdata);
int print_all(struct shuju pdata);
int feedbook(int sflag,int dflag,char* result,int yn);

#endif //MENU_H__

