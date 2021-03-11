/*================================================================
 *   Copyright (C) 2021 hqyj Ltd. All rights reserved.
 *   
 *   文件名称：epoll_work.c
 *   创 建 者：zhoubeibei
 *   创建日期：2021年03月09日
 *   描    述：
 *
 ================================================================*/

#include "include.h"

extern sqlite3 * staff;
int network_epoll(){
	int ret,num=1,i,nfds;
	char sql[256];
	char *errmsg = NULL;
	struct epoll_event event;
	struct epoll_event revents[10];
	struct sockaddr_in cin;
	socklen_t cin_len = sizeof(cin);
	int cntfd = -1;
	int epfd = epoll_create(10);
	if(epfd<0){
		perror("epoll create");
		return -1;
	}
	event.events = EPOLLIN;
	event.data.fd = sfd;
	ret = epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	if(ret){
		perror("epoll ctl");
		return -1;
	}
	while(1){
		nfds = epoll_wait(epfd,revents,20,-1);
		if(nfds == -1){
			perror("epoll wait error");
			return -1;
		}
		for(i=0;i<nfds;i++){
			if(revents[i].data.fd == sfd){
				cntfd = accept(sfd,(struct sockaddr*)&cin,&cin_len);
				if(cntfd<0){
					perror("accept error");
					return -1;
				}
				printf("连接成功%d\n",__LINE__);
				event.events = EPOLLIN;
				event.data.fd = cntfd;
				ret = epoll_ctl(epfd,EPOLL_CTL_ADD,cntfd,&event);
				if(ret){
					perror("epoll ctl");
					return -1;
				}
			}else{
				do{  
					ret = recv(revents[i].data.fd,&trans_data,sizeof(trans_data),0);
				}while(ret < 0 && EINTR == errno);
				if(ret<0){
					perror("recv");
					return -1;
				}else if(ret == 0){
					printf("用户断开连接\n");
					bzero(sql,sizeof(sql));    
					sprintf(sql,"delete from zaixianyonghu where id=%d",trans_data.addre.id);
					if(sqlite3_exec(staff,sql,NULL,NULL,&errmsg)!=0){  
						fprintf(stderr,"sqlite3_exec:%s\n",errmsg);
						return -1;                                               
					}
					event.events = EPOLLIN;
					event.data.fd = revents[i].data.fd;
					ret = epoll_ctl(epfd,EPOLL_CTL_DEL,event.data.fd,&event);
					if(ret){
						perror("epoll ctl error");
						return -1;
					}
					break;
				}
				printf("flag:%x\n",trans_data.flag);
				printf("*****************\n");
				if(trans_data.flag==0x11){
					if(register_account(revents[i].data.fd)<0){
						printf("用户注册失败\n");
					}
				}else if(trans_data.flag==0x21){
					if(login_account(revents[i].data.fd)<0){
						printf("用户登录失败\n");
					}
				}else if(trans_data.flag>=0x34&&trans_data.flag<=0x36){
					if(inquire_message(revents[i].data.fd)<0){
						printf("查询数据失败\n");
					}
				}else if(trans_data.flag == 0x3A){
					if(modify_message(revents[i].data.fd)<0){
						printf("更新数据失败\n");
					}
				}else if(trans_data.flag == 0x51){
					add_data(revents[i].data.fd);
				}else if(trans_data.flag == 0x55){
					delete_data(revents[i].data.fd);
				}else if(trans_data.flag == 0x59){
					modify_data(revents[i].data.fd);
				}else if(trans_data.flag>=0x41&&trans_data.flag<=0x44){
					printf("*****************\n");
					inquire_data(revents[i].data.fd);
				}

			}

		}
	}
	return 0;
}
int register_account(int newfd)
{
	int ret;
	printf("开始注册\n");
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
	printf("注册成功\n");
	return 0;
}
int login_account(int newfd)
{/*{{{*/
	int ret;
	char * errmsg = NULL;  
	char ** resultp = NULL;
	int row,column;        
	printf("开始登录\n");
	char sql[256];
	bzero(sql,sizeof(sql));
	sprintf(sql,"select * from datum where id=%d",trans_data.addre.id); 
	if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
		fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
		sprintf(trans_data.myerrno,"获取数据库失败\n");
		trans_data.flag = 0x23;
		do{
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno==EINTR);
		return -1;                                                   
	} 
	if(row==0){
		sprintf(trans_data.myerrno,"用户不存在\n");
		trans_data.flag = 0x23;
		do{                                               
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);
		return -1;
	}
	if(strcmp(trans_data.addre.passwd,resultp[TABLE_LEN+1])){                        
	    sprintf(trans_data.myerrno,"密码错误\n");                             
		trans_data.flag = 0x23;                                 
		do{                                               
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);                    
	    sqlite3_free_table(resultp);                      
	    return -1;                                        
	}
	//权限读取
	printf("juris:%d\n",trans_data.addre.juris);
	trans_data.addre.juris = (*resultp[TABLE_LEN+9]-48);
	printf("juris:%d\n",trans_data.addre.juris);
	sqlite3_free_table(resultp);                      
	bzero(sql,sizeof(sql));                                          
	sprintf(sql,"select * from zaixianyonghu where id=%d",trans_data.addre.id); 
	if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
		fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
		sprintf(trans_data.myerrno,"获取数据库失败\n");
		trans_data.flag = 0x23;
		do{
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno==EINTR);
		return -1;                                                   
	}
	if(row!=0){
		sprintf(trans_data.myerrno,"用户在线\n");
		trans_data.flag = 0x23;
		do{                                               
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR); 
		sqlite3_free_table(resultp);                      
	    return -1;                                        
	}
	sqlite3_free_table(resultp);      
	bzero(sql,sizeof(sql));                                      
	sprintf(sql,"insert into zaixianyonghu values(%d,\"%s\",%d)",
			trans_data.addre.id,trans_data.addre.name,trans_data.addre.juris);
	if(sqlite3_exec(staff,sql,NULL,NULL,&errmsg)!=0){  
	    fprintf(stderr,"sqlite3_exec:%s\n",errmsg);
		sprintf(trans_data.myerrno,"用户在线数据写入失败\n");
		trans_data.flag = 0x23;
		do{                                               
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR); 
		return -1;                                               
	}
	memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
	trans_data.flag = 0x22;
	do{                                               
		ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
	}while(ret<0&&errno == EINTR);
	printf("登录成功\n");
	return 0;
}/*}}}*/
int inquire_message(int newfd)
{
	int ret,i;
	char * errmsg = NULL;  
	char ** resultp = NULL;
	int row,column;       
	char sql[256];
	switch(trans_data.flag){
	case 0x34:/*{{{*/
		bzero(sql,sizeof(sql));
		sprintf(sql,"select * from datum where id=%s",trans_data.search); 
		if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
			fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
			sprintf(trans_data.myerrno,"获取数据库失败\n");
			trans_data.flag = 0x33;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return -1;
		}
		if(!row){
			sprintf(trans_data.myerrno,"未找到账号\n");
			trans_data.flag = 0x33;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return 0;
		}
		memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
		copy_data(&trans_data.addre,&resultp[TABLE_LEN]);
		trans_data.flag = 0x39;
		do{
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno==EINTR);
		printf("数据已发送\n");
		break;/*}}}*/
	case 0x35:/*{{{*/
		bzero(sql,sizeof(sql));
		sprintf(sql,"select * from datum where name=\"%s\"",trans_data.search); 
		if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
			fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
			sprintf(trans_data.myerrno,"获取数据库失败\n");
			trans_data.flag = 0x33;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return -1;
		}
		if(!row){
			sprintf(trans_data.myerrno,"未找到账号\n");
			trans_data.flag = 0x33;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return 0;
		}
		memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
		for(i=1;i<=row;i++){
			trans_data.flag = 0x38;
			if(i==row){
				trans_data.flag = 0x39;
			}
			copy_data(&trans_data.addre,&resultp[TABLE_LEN*i]);
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
		}
		printf("数据已发送\n");
		break;/*}}}*/
	case 0x36:/*{{{*/
		bzero(sql,sizeof(sql));
		sprintf(sql,"select * from datum where number=\"%s\"",trans_data.search); 
		if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
			fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
			sprintf(trans_data.myerrno,"获取数据库失败\n");
			trans_data.flag = 0x33;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return -1;
		}
		if(!row){
			sprintf(trans_data.myerrno,"未找到账号\n");
			trans_data.flag = 0x33;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return 0;
		}
		memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
		for(i=1;i<=row;i++){
			trans_data.flag = 0x38;
			if(i==row){
				trans_data.flag = 0x39;
			}
			copy_data(&trans_data.addre,&resultp[TABLE_LEN*i]);
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
		}
		printf("数据已发送\n");
		break;/*}}}*/
	default:
		break;
	}
}
int modify_message(int newfd)
{/*{{{*/
	int ret;
	char * errmsg = NULL;      
	char sql[256];
	bzero(sql,sizeof(sql));                                      
	sprintf(sql,"UPDATE into datum set id=%d where name=\"%s\" \
			and address = \"%s\" and number = \"%s\"",
			trans_data.addre.id,
			trans_data.addre.name,
			trans_data.addre.address,
			trans_data.addre.number);
	if(sqlite3_exec(staff,sql,NULL,NULL,&errmsg)!=0){  
	    fprintf(stderr,"sqlite3_exec:%s\n",errmsg);
		sprintf(trans_data.myerrno,"用户在线数据更新失败\n");
		trans_data.flag = 0x33;
		do{                                               
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR); 
		return -1;                                               
	}
	trans_data.flag = 0x3F;
	do{                                               
		ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
	}while(ret<0&&errno == EINTR);
	printf("数据更新成功\n");
	return 0;
}/*}}}*/
int add_data(int newfd)
{
	int ret;
	char sql[256];
	char *errmsg = NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql,"insert into datum values\
			(%s,\"%s\",\"%s\",\"%s\",%d,\"%c\",\"%s\",%d,\"%s\",%d)",
			trans_data.search,       trans_data.addre.passwd,
			trans_data.addre.name,   trans_data.addre.address,
			trans_data.addre.age,    trans_data.addre.sex,
			trans_data.addre.number, trans_data.addre.salary,
			trans_data.addre.section,trans_data.addre.juris);
	ret = sqlite3_exec(staff,sql,NULL,NULL,&errmsg);
	if(ret){
		bzero(trans_data.myerrno,sizeof(trans_data.myerrno));
		fprintf(stderr,"%s\n",sqlite3_errmsg(staff));
		trans_data.flag = 0x53;
		sprintf(trans_data.myerrno,"用户已存在\n");
		do{                                               
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);                    
		return -1;
	}
	bzero(trans_data.myerrno,sizeof(trans_data.myerrno));
	trans_data.flag = 0x52;
	do{                                               
		ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
	}while(ret<0&&errno == EINTR);
	printf("添加用户成功\n");
	return 0;
}
int delete_data(int newfd)
{
	int ret;
	char sql[256];
	char *errmsg = NULL;
	sprintf(sql,"delete from datum where id=%s",trans_data.search);
	if(sqlite3_exec(staff,sql,NULL,NULL,&errmsg)!=0){
	    fprintf(stderr,"sqlite3_exec:%s\n",errmsg);
		sprintf(trans_data.myerrno,"删除失败,未找到账号\n");
		trans_data.flag = 0x57;
		do{
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);
		return -1;
	}
	bzero(trans_data.myerrno,sizeof(trans_data.myerrno));
	trans_data.flag = 0x56;
	do{                                               
		ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
	}while(ret<0&&errno == EINTR);
	printf("删除用户成功\n");
	return 0;
}
int modify_data(int newfd)
{
	int ret;
	char sql[256];
	char *errmsg = NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql,"UPDATE datum SET passwd=\"%s\",name = \"%s\",\
			address = \"%s\",age = %d,sex = \"%c\",number = \"%s\",\
			salary = %d,section = \"%s\",juris = %d where id = %s",
			trans_data.addre.passwd, trans_data.addre.name,   
			trans_data.addre.address,trans_data.addre.age,    
			trans_data.addre.sex,    trans_data.addre.number, 
			trans_data.addre.salary, trans_data.addre.section,
			trans_data.addre.juris,  trans_data.search);
	ret = sqlite3_exec(staff,sql,NULL,NULL,&errmsg);
	if(ret){
		bzero(trans_data.myerrno,sizeof(trans_data.myerrno));
		fprintf(stderr,"%s\n",sqlite3_errmsg(staff));
		trans_data.flag = 0x5B;
		sprintf(trans_data.myerrno,"用户不存在\n");
		do{                                               
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno == EINTR);                    
		return -1;
	}
	bzero(trans_data.myerrno,sizeof(trans_data.myerrno));
	trans_data.flag = 0x5A;
	do{                                               
		ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
	}while(ret<0&&errno == EINTR);
	printf("修改信息成功\n");
	return 0;
}
int inquire_data(int newfd)
{
	int ret,i;
	char * errmsg = NULL;  
	char ** resultp = NULL;
	int row,column;       
	char sql[256];
	switch(trans_data.flag){
	case 0x41:/*{{{*/
		bzero(sql,sizeof(sql));
		sprintf(sql,"select * from datum where id=%s",trans_data.search); 
		if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
			fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
			sprintf(trans_data.myerrno,"获取数据库失败\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return -1;
		}
		if(!row){
			sprintf(trans_data.myerrno,"未找到账号\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return 0;
		}
		memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
		copy_data_all(&trans_data.addre,&resultp[TABLE_LEN]);
		trans_data.flag = 0x5E;
		do{
			ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
		}while(ret<0&&errno==EINTR);
		printf("数据已发送\n");
		break;/*}}}*/
	case 0x42:/*{{{*/
		bzero(sql,sizeof(sql));
		sprintf(sql,"select * from datum where name=\"%s\"",trans_data.search); 
		if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
			fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
			sprintf(trans_data.myerrno,"获取数据库失败\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return -1;
		}
		if(!row){
			sprintf(trans_data.myerrno,"未找到账号\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return 0;
		}
		memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
		for(i=1;i<=row;i++){
			trans_data.flag = 0x45;
			if(i==row){
				trans_data.flag = 0x5E;
			}
			copy_data_all(&trans_data.addre,&resultp[TABLE_LEN*i]);
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
		}
		printf("数据已发送\n");
		break;/*}}}*/
	case 0x43:/*{{{*/
		bzero(sql,sizeof(sql));
		sprintf(sql,"select * from datum where number=\"%s\"",trans_data.search); 
		if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
			fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
			sprintf(trans_data.myerrno,"获取数据库失败\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return -1;
		}
		if(!row){
			sprintf(trans_data.myerrno,"未找到账号\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return 0;
		}
		memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
		for(i=1;i<=row;i++){
			trans_data.flag = 0x45;
			if(i==row){
				trans_data.flag = 0x5E;
			}
			copy_data_all(&trans_data.addre,&resultp[TABLE_LEN*i]);
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
		}
		printf("数据已发送\n");
		break;/*}}}*/
	case 0x44:/*{{{*/
		printf("++++++++++++++++\n");
		bzero(sql,sizeof(sql));
		sprintf(sql,"select * from datum"); 
		if(sqlite3_get_table(staff,sql,&resultp,&row,&column,&errmsg)!=0){
			fprintf(stderr,"sqlite3_exec:%s\n",errmsg);                  
			sprintf(trans_data.myerrno,"获取数据库失败\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return -1;
		}
		printf("*****************\n");
		if(!row){
			sprintf(trans_data.myerrno,"未找到账号\n");
			trans_data.flag = 0x5F;
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
			return 0;
		}
		printf("++++++++++++++++\n");
		memset(trans_data.myerrno,0,sizeof(trans_data.myerrno));
		for(i=1;i<=row;i++){
			trans_data.flag = 0x45;
			if(i==row){
				trans_data.flag = 0x5E;
			}
			printf("*****************\n");
			copy_data_all(&trans_data.addre,&resultp[TABLE_LEN*i]);
			printf("-----------------\n");
			do{
				ret = send(newfd,(void*)&trans_data,sizeof(trans_data),0);
			}while(ret<0&&errno==EINTR);
		}
		printf("数据已发送\n");
		break;/*}}}*/
	default:
		break;
	}

}
int copy_data(struct shuju *sdata,char** resultp)
{
	// sdata->id = resultp[0];
	// strcpy(sdata->passwd,resultp[1]);
	if(resultp[0]!=NULL){
		strcpy(trans_data.search,resultp[0]);
	}
	if(resultp[2]!=NULL){
	strcpy(sdata->name,resultp[2]);
	}else{
		memset(sdata->name,0,sizeof(sdata->name));
	}
	if(resultp[3]!=NULL){
	strcpy(sdata->address,resultp[3]);
	}else{
		memset(sdata->address,0,sizeof(sdata->address));
	}
	if(resultp[4]!=NULL){
	sdata->age = (*resultp[4]-48);
	}else{
		sdata->age = -1;
	}
	if(resultp[5]!=NULL){
	sdata->sex = *resultp[5];
	}else{
		sdata->sex = 0;
	}
	if(resultp[6]!=NULL){
	strcpy(sdata->number,resultp[6]);
	}else{
		memset(sdata->number,0,sizeof(sdata->number));
	}
	if(resultp[7]!=NULL){
	sdata->salary = (*resultp[7]-48);
	}else{
		sdata->salary = -1;
	}
	if(resultp[8]!=NULL){
	strcpy(sdata->section,resultp[8]);
	}else{
		memset(sdata->section,0,sizeof(sdata->section));
	}
	//sdata->juris = resultp[9];
	return 0;
}
int copy_data_all(struct shuju *sdata,char** resultp)
{
	if(resultp[0]!=NULL){
		strcpy(trans_data.search,resultp[0]);
	}
	if(resultp[1]!=NULL){
		strcpy(sdata->passwd,resultp[1]);
	}else{
		memset(sdata->passwd,0,sizeof(sdata->passwd));
	}
	if(resultp[2]!=NULL){
	strcpy(sdata->name,resultp[2]);
	}else{
		memset(sdata->name,0,sizeof(sdata->name));
	}
	if(resultp[3]!=NULL){
		strcpy(sdata->address,resultp[3]);
	}else{
		memset(sdata->address,0,sizeof(sdata->address));
	}
	if(resultp[4]!=NULL){
		sdata->age = (*resultp[4]-48);
	}else{
		sdata->age = -1;
	}
	if(resultp[5]!=NULL){
		sdata->sex = *resultp[5];
	}else{
		sdata->sex = 0;
	}
	if(resultp[6]!=NULL){
		strcpy(sdata->number,resultp[6]);
	}else{
		memset(sdata->number,0,sizeof(sdata->number));
	}
	if(resultp[7]!=NULL){
		sdata->salary = (*resultp[7]-48);
	}else{
		sdata->salary = 0;
	}
	if(resultp[8]!=NULL){
		strcpy(sdata->section,resultp[8]);
	}else{
		memset(sdata->section,0,sizeof(sdata->section));
	}
	if(resultp[9]!=NULL){
		sdata->juris = (*resultp[9]-48);
	}else{
		sdata->juris = -1;
	}
	return 0;
}
