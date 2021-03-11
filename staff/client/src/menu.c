/*================================================================
 *   Copyright (C) 2021 hqyj Ltd. All rights reserved.
 *   
 *   文件名称：menu.c
 *   创 建 者：zhoubeibei
 *   创建日期：2021年03月09日
 *   描    述：
 *
 ================================================================*/

#include "include.h"

int menu()
{
	int select=0;
	char select_c[20];
	while(1){
		switch(trans_data.flag){
		case 0x00:
			printf("输入想要执行的功能:\n1.注册\n2.登录\n3.退出\n>");
			scanf("%d",&select);
			while(getchar()!='\n');
			if(select<1||select>3){
				printf("无此项功能,请重新选择\n");
				return 0;
			}else if(select == 3){
				printf("退出\n");
				return -1;
			}else if(select == 1){
				trans_data.flag = 0x10;
			}else if(select == 2){
				trans_data.flag = 0x20;
			}
			break;
		case 0x10:
			printf("请问你想要注册管理员账号还是普通账号?\n");
			printf("0:管理员\n");
			printf("其他:普通账号\n");
			if(!scanf("%d",&select)){
				fprintf(stderr,"格式错误\n");
				continue;
			}
			if(!select){
				trans_data.addre.juris = 0;
				printf("申请管理员账户请输入秘钥>");
				scanf("%d",&select);
				if(select!=MIKEY){
					continue;
				}
			}else{
				trans_data.addre.juris = 1;
			}
			while(getchar()!='\n');
			bzero(trans_data.addre.passwd,sizeof(trans_data.addre.passwd));
			printf("请输入账号:");
			if(!scanf("%d",&trans_data.addre.id)){
				fprintf(stderr,"账号格式错误\n");
				continue;
			}
			while(getchar()!='\n');
			printf("请输入密码:");
			if(scanf("%s",trans_data.addre.passwd)>19){
				fprintf(stderr,"密码格式错误\n");
				continue;
			}
			while(getchar()!='\n');              
			trans_data.flag = 0x11;
			info_login();
			break;
		case 0x12:
			printf("注册成功\n");
			printf("是否用当前账号直接登录(y)\n");
			scanf("%s",select_c);
			if(!strncmp(select_c,"y",1)){
				if(trans_data.addre.juris){
					trans_data.flag = 0x30;
				}else{
					trans_data.flag = 0x40;
				}
			}else{
				trans_data.flag = 0x00;
				return 0;
			}
			while(getchar()!='\n');
			break;
		case 0x13:
			printf("注册失败:%s\n",trans_data.myerrno);
			printf("请问是否继续注册:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x10;
			}else{
				trans_data.flag = 0x00;
			}
			while(getchar()!='\n');
			break;
			//登录态                             
		case 0x20:                               
			bzero(trans_data.addre.passwd,sizeof(trans_data.addre.passwd));
			printf("请输入账号:");
			if(!scanf("%d",&trans_data.addre.id)){
				fprintf(stderr,"账号格式错误\n");
				continue;
			}
			while(getchar()!='\n');
			printf("密码:");
			if(scanf("%s",trans_data.addre.passwd)>19){
				fprintf(stderr,"账号格式错误\n");
				continue;
			}
			while(getchar()!='\n');
			trans_data.flag = 0x21;                     
			info_login();
			break;                               
			//完成登录基础态                     
		case 0x22:
			printf("登录成功\n");
			if(trans_data.addre.juris){
				trans_data.flag = 0x30;
				return 0;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
			break;
		case 0x23:
			printf("登录失败:%s\n",trans_data.myerrno);
			printf("请问是否继续登录:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x20;
			}else{
				trans_data.flag = 0x00;
				return 0;
			}
			break;
		case 0x30:
			printf("进入用户界面\n");
			printf("选择你需要的功能\n");
			printf("1.查询信息\n2.修改信息\n3.退出>\n");
			scanf("%d",&select);
			while(getchar()!='\n');
			if(select==1){
				trans_data.flag = 0x31;
				continue;
			}else if(select == 2){
				trans_data.flag = 0x32;
				continue;
			}else if(select == 3){
				trans_data.flag = 0x00;
				return 0;
			}else{
				continue;
			}
			break;
		case 0x31:
			printf("选择查找方式:1.账号,2.姓名,3.手机号码,4.退出\n>");
			scanf("%d",&select);
			while(getchar()!='\n');
			if(select == 1){
				trans_data.flag = 0x34;
			}else if(select == 2){
				trans_data.flag = 0x35;
			}else if(select == 3){
				trans_data.flag = 0x36;
			}else if(select == 4){
				trans_data.flag = 0x30;
			}
			else{
				printf("无此种查询方式\n");
				continue;
			}
			bzero(trans_data.search,sizeof(trans_data.search));
			printf("请输入你查找的信息:");
			scanf("%s",trans_data.search);
			while(getchar()!='\n');
			info_login();
			break;
		case 0x32:
			printf("请输入姓名>");
			scanf("%s",trans_data.addre.name);
			while(getchar()!='\n');
			printf("请输入住址>");
			scanf("%s",trans_data.addre.address);
			while(getchar()!='\n');
			printf("请输入电话>");
			scanf("%s",trans_data.addre.number);
			while(getchar()!='\n');
			trans_data.flag = 0x3A;
			info_login();
			break;
		case 0x33:
			printf("操作失败:%s\n",trans_data.myerrno);
			printf("是否继续查找(y)\n>");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x31;
			}else{
				trans_data.flag = 0x30;
			}
			return 0;
			break;
		case 0x39:
			print(trans_data.addre);
			trans_data.flag = 0x30;
			break;
		case 0x40:
			printf("进入管理员用户界面\n");
			printf("选择你需要的功能\n");
			printf("1.添加用户\n2.删除用户\n3.修改信息\n4.查询信息\n5.退出>\n");
			scanf("%d",&select);
			while(getchar()!='\n');
			if(select==1){
				trans_data.flag = 0x50;
				continue;
			}else if(select == 2){
				trans_data.flag = 0x54;
				continue;
			}else if(select == 3){
				trans_data.flag = 0x58;
				continue;
			}else if(select == 4){
				trans_data.flag = 0x5C;
				continue;
			}else if(select == 5){
				trans_data.flag = 0x00;
				return 0;
			}else{
				continue;
			}
		case 0x50:
			printf("输入想要添加的用户信息\n");
			printf("请输入用户ID>");
			scanf("%s",trans_data.search);
			while(getchar()!='\n');
			printf("请输入用户密码>");
			scanf("%s",trans_data.addre.passwd);
			while(getchar()!='\n');
			printf("请输入用户姓名>");
			scanf("%s",trans_data.addre.name);
			while(getchar()!='\n');
			printf("请输入用户地址>");
			scanf("%s",trans_data.addre.address);
			while(getchar()!='\n');
			printf("请输入用户年龄>");
			scanf("%d",&trans_data.addre.age);
			while(getchar()!='\n');
			printf("请输入用户性别男(M)女(F)>");
			scanf("%c",&trans_data.addre.sex);
			while(getchar()!='\n');
			printf("请输入用户手机号码>");
			scanf("%s",trans_data.addre.number);
			while(getchar()!='\n');
			printf("请输入用户工资>");
			scanf("%d",&trans_data.addre.salary);
			while(getchar()!='\n');
			printf("请输入用户部门>");
			scanf("%s",trans_data.addre.section);
			while(getchar()!='\n');
			printf("请输入用户权限管理员(0)普通(1)>");
			scanf("%d",&trans_data.addre.juris);
			while(getchar()!='\n');
			trans_data.flag = 0x51;
			info_login();
			break;
		case 0x52:
			printf("是否继续添加账号(y)>");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x50;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
		case 0x53:
			printf("添加失败:%s\n",trans_data.myerrno);
			printf("请问是否继续当前操作:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x50;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
			break;
		case 0x54:
			printf("请输入要删除用户的ID>");
			scanf("%s",trans_data.search);
			while(getchar()!='\n');
			trans_data.flag = 0x55;
			info_login();
			break;
		case 0x56:
			printf("删除用户成功\n");
			printf("请问是否继续当前操作:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x54;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
		case 0x57:
			printf("删除用户失败:%s\n",trans_data.myerrno);
			printf("请问是否继续当前操作:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x54;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
		case 0x58:
			printf("输入要修改用户的信息\n");
			printf("请输入要修改用户的ID>");
			scanf("%s",trans_data.search);
			while(getchar()!='\n');
			printf("请输入用户密码>");
			scanf("%s",trans_data.addre.passwd);
			while(getchar()!='\n');
			printf("请输入用户姓名>");
			scanf("%s",trans_data.addre.name);
			while(getchar()!='\n');
			printf("请输入用户地址>");
			scanf("%s",trans_data.addre.address);
			while(getchar()!='\n');
			printf("请输入用户年龄>");
			scanf("%d",&trans_data.addre.age);
			while(getchar()!='\n');
			printf("请输入用户性别男(M)女(F)>");
			scanf("%c",&trans_data.addre.sex);
			while(getchar()!='\n');
			printf("请输入用户手机号码>");
			scanf("%s",trans_data.addre.number);
			while(getchar()!='\n');
			printf("请输入用户工资>");
			scanf("%d",&trans_data.addre.salary);
			while(getchar()!='\n');
			printf("请输入用户部门>");
			scanf("%s",trans_data.addre.section);
			while(getchar()!='\n');
			printf("请输入用户权限管理员(0)普通(1)>");
			scanf("%d",&trans_data.addre.juris);
			while(getchar()!='\n');
			trans_data.flag = 0x59;
			info_login();
			break;
		case 0x5A:
			printf("修改用户成功\n");
			printf("请问是否继续当前操作:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x58;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
		case 0x5B:
			printf("修改用户失败:%s\n",trans_data.myerrno);
			printf("请问是否继续当前操作:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x54;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
		case 0x5C:
			printf("选择查找方式:1.账号,2.姓名,3.手机号码,4.所有信息,5.退出\n>");
			scanf("%d",&select);
			while(getchar()!='\n');
			if(select == 1){
				trans_data.flag = 0x41;
			}else if(select == 2){
				trans_data.flag = 0x42;
			}else if(select == 3){
				trans_data.flag = 0x43;
			}else if(select == 4){
				trans_data.flag = 0x44;
			}else if(select == 5){
				trans_data.flag = 0x40;
				return 0;
			}
			else{
				printf("无此种查询方式\n");
				continue;
			}
			if(select!=4){
				bzero(trans_data.search,sizeof(trans_data.search));
				printf("请输入你查找的信息:");
				scanf("%s",trans_data.search);
				while(getchar()!='\n');
			}
			info_login();
			break;
		case 0x5E:
			print_all(trans_data.addre);
			printf("查询用户成功\n");
			printf("请问是否继续当前操作:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x5C;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
			break;
		case 0x5F:
			printf("查询用户失败:%s\n",trans_data.myerrno);
			printf("请问是否继续当前操作:是(y)或者否(任意键)\n");
			scanf("%s",select_c);
			while(getchar()!='\n');
			if(!strncmp(select_c,"y",1)){
				trans_data.flag = 0x5C;
			}else{
				trans_data.flag = 0x40;
				return 0;
			}
			break;
		default:
			break;

		}
	}
	return 0;
}
void* my_recv(void* arg)
{
	//char select[128];
	int ret;
	printf("接受数据\n");
	while(1){
		do{                                            
			ret = recv(sfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);
		if(ret < 0){
			perror("recv");
			continue;
		}else if(0 == ret){
			fprintf(stderr, "服务器断开连接\n");
			kill(getpid(),SIGINT);
			return NULL;
		}
		switch(trans_data.flag){
		case 0x38:
			print(trans_data.addre);
			break;
		case 0x45:
			print_all(trans_data.addre);
			break;
		default:
			break;
		}
	}
}

int info_login()
{
	int ret;
	do{
		ret = send(sfd,(void*)&trans_data,sizeof(trans_data),0);
		perror("send");
		printf("发送数据\n");
	}while(ret<0&&errno == EINTR);
	return 0;
}
int print(struct shuju pdata)
{
	printf("ID:%d\tNAME:%s\tAGE:%d\tNUMBER:%s\tSECTION:%s\n",
			pdata.id,pdata.name,pdata.age,
			pdata.number,pdata.section);
	return 0;
}
int print_all(struct shuju pdata)
{
	printf("ID:%d\tPASSWD:%s\tNAME:%s\tADDRESS:%s\t\
			AGE:%d\tSEX:%c\tNUMBER:%s\tSALARY:%d\t\
			SECTION:%s\tJURIS:%d\n",
			pdata.id,pdata.passwd,pdata.name,pdata.address,
			pdata.age,pdata.sex,pdata.number,pdata.salary,
			pdata.section,pdata.juris);
	return 0;
}
