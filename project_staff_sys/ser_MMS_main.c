/*================================================================
 *   Copyright (C) 2021 hqyj Ltd. All rights reserved.
 *   
 *   文件名称：MMS.c
 *   创 建 者：xx
 *   创建日期：2021年03月09日
 *   描    述：服务器 main
 *
 ================================================================*/


#include <stdio.h>
#include <sys/epoll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <sqlite3.h>

#define EMAX 20
typedef struct modbus{
	char order;       //regist load quit exit
	char user_name[20];
	char user_password[20];
	int permission;   //1 / 0
	int ret;

}mod;
typedef struct number{
	char name[20];
	int age;
	int salary;
	int ret;
}num;


struct modbus mod_xx;
struct number num_xx;



int ser_ipconfig(int * sfd);

int do_cli_request(int fd,sqlite3 *db,struct epoll_event *event,int epfd);

int regist(sqlite3 *db);

int load(sqlite3 *db);
int quit();
int exit_();
int add(int sfd,sqlite3 *db);
int del(int sfd,sqlite3 *db);
int update(int sfd,sqlite3 *db);
int find(int sfd,sqlite3 *db);


int main(int argc, char *argv[])
{
	//sqlite
	sqlite3 *db =NULL;
	int ret;
	ret=sqlite3_open("./sys.db",&db);
	if(ret!=0)
	{
		fprintf(stderr,"sqlite open error\n");
		fprintf(stderr,"%s\n",sqlite3_errmsg(db));
		return -1;
	}
	printf("sys sqlite opend\n");
	//create table user_msg
	char sql[256]="";  //operate
	char *errmsg =NULL;
	bzero(sql,sizeof(sql));
	sprintf(sql,"create table if not exists user_msg(user_name char,user_password char,permission int)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=0)
	{
		fprintf(stderr,"sqlite3_exec%s\n",errmsg);
		goto ERR1;
	}
	printf("create table succeed\n");
	//create table staff_msg
	bzero(sql,sizeof(sql));
	sprintf(sql,"create table if not exists staff_msg(name char,age int,salary int)");
	if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)!=0)
	{
		fprintf(stderr,"sqlite3_exec%s\n",errmsg);
		goto ERR2;
	}
	//ipconfig
	int sfd,i;
	if(ser_ipconfig(&sfd)<0){
		return -1;
	}
	//epoll_create
	int epfd;
	struct epoll_event event;
	struct epoll_event l_events[EMAX];
	epfd=epoll_create(100); //100 
	if(epfd==-1){
		perror("epoll create error\n");
		return -1;
	}
	//epoll_ctl
	event.events=EPOLLIN;
	event.data.fd=sfd;
	ret=epoll_ctl(epfd,EPOLL_CTL_ADD,sfd,&event);
	if(ret){
		printf("ss\n");

		perror("epoll ctl error\n");
		return -1;
	}
	int cfd;
	struct sockaddr_in cin;
	int rets;
	socklen_t len =sizeof(cin);
	while(1)
	{
		//epoll_wait
		ret=epoll_wait(epfd,l_events,EMAX,-1);
		if(ret<0){
			perror("epoll wait\n");
			return -1;
		}
		for(i=0;i<ret;i++){
			if((l_events[i].events &EPOLLIN)&&(l_events[i].data.fd==sfd)){   //客户端连接事件
				cfd=accept(sfd,(struct sockaddr *)&cin,&len);
				if(cfd<0){
					perror("accept\n");
					return -1;
				}
				printf("new cli connected\n");
				event.events=EPOLLIN;
				event.data.fd=cfd;
				rets=epoll_ctl(epfd,EPOLL_CTL_ADD,cfd,&event);
				if(rets){
					perror("epoll_ctl\n");
					return -1;

				}

			}
			else if(l_events[i].events&EPOLLIN)
			{

				rets=do_cli_request(l_events[i].data.fd,db,&l_events[i],epfd);
				if(rets<0){
					return -1;
				}
				printf("--------------------------------------------\n");

			}



		}
	}
	sqlite3_close(db);

	return 0;
ERR2:
ERR1:
	sqlite3_close(db);
	return 0;

}





int ser_ipconfig(int *sfd)
{
	int ret;

	*sfd=socket(AF_INET,SOCK_STREAM,0);
	if(*sfd<0)
	{
		perror("sli_socket error\n");
		return -1;
	}
	int reuse=1;
	setsockopt(*sfd,SOL_SOCKET,SO_REUSEADDR,&reuse,sizeof(int));

	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=htons(3116);
	sin.sin_addr.s_addr=inet_addr("0.0.0.0");
	if(bind(*sfd,(struct sockaddr *)&sin,sizeof(sin))<0){
		perror("bind error\n");
		return -1;
	}

	if(listen(*sfd,3)<0){
		perror("listen error\n");
		return -1;
	}
	printf("ser listened\n");
	return 0;

}



int do_cli_request(int fd,sqlite3 *db,struct epoll_event *event,int epfd)
{
	printf("%s\t%s\t%d\t\n",__FILE__,__func__,__LINE__);
	int ret;
	ret=recv(fd,&mod_xx,sizeof(mod),0);
	if(ret<0){
		perror("recv error 206\n");
		return -1;
	}
	else if(ret==0){
		printf("cli closed\n");           //用epoll_ctl从 event中删除该用户流


		ret=epoll_ctl(epfd,EPOLL_CTL_DEL,fd,event);

		return 0;
	}
	printf("recv order %c\n",mod_xx.order);
	switch(mod_xx.order)
	{
	case 'r':
		regist(db);
		ret=send(fd,&mod_xx,sizeof(mod),0);
		if(ret<0){
			perror("send error\n");
			return -1;
		}

		break;
	case 'l':
		load(db);	
		ret=send(fd,&mod_xx,sizeof(mod),0);
		if(ret<0){
			perror("send error\n");
			return -1;
		}


		break;
	case 'q':
		quit();

		break;
	case 'e':
		exit_();

		break;
	case 'a':
		add(fd,db);

		break;
	case 'd':
		del(fd,db);

		break;
	case 'u':
		update(fd,db);

		break;
	case 'f':
		find(fd,db);

		break;

	}

}





int regist(sqlite3 *db)
{
	printf("%s\t%s\t%d\t\n",__FILE__,__func__,__LINE__);

	int ret;
	char zSql[128]="";
	bzero(zSql,sizeof(zSql));
	char **pResult;
	int pnRow;
	int pnCloumn;
	char *pzErrmsg=NULL;
	sprintf(zSql,"select * from user_msg where user_name=\"%s\"",mod_xx.user_name);
	if(sqlite3_get_table(db,zSql,&pResult,&pnRow,&pnCloumn,&pzErrmsg)!=0)
	{
		printf("%s\n",pzErrmsg);
		return -1;
	}
	if(pnRow==0)
	{
		char sql[128]="";
		char *errmsg=NULL;
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into user_msg values(\"%s\",\"%s\",%d)",mod_xx.user_name,mod_xx.user_password,mod_xx.permission);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)<0)
		{
			printf("%s\n",errmsg);
			return -1;
		}
		printf("regist success\n");
		mod_xx.ret=1;


	}
	else{
		mod_xx.ret=-1;
		printf("regist failed\n");

	}

}


int load(sqlite3 *db)
{
	printf("%s\t%s\t%d\t\n",__FILE__,__func__,__LINE__);
	int ret;
	char zSql[128]="";
	bzero(zSql,sizeof(zSql));
	char **pResult;
	int pnRow;
	int pnCloumn;
	char *pzErrmsg=NULL;
	sprintf(zSql,"select * from user_msg where user_name=\"%s\" ",mod_xx.user_name);
	if(sqlite3_get_table(db,zSql,&pResult,&pnRow,&pnCloumn,&pzErrmsg)!=0)
	{
		printf("%s\n",pzErrmsg);
		return -1;
	}
	if(pnRow==0)
	{
		printf("not regist\n");
		mod_xx.ret=-1;


	}
	else{
		printf("2\n");
		sprintf(zSql,"select * from user_msg where user_name=\"%s\" and user_password =\"%s\"",mod_xx.user_name,mod_xx.user_password);
		if(sqlite3_get_table(db,zSql,&pResult,&pnRow,&pnCloumn,&pzErrmsg)!=0)
		{
			printf("%s\n",pzErrmsg);
			return -1;
		}
		if(pnRow==0)
		{
			printf("password error\n");
			mod_xx.ret=-2;
		}
		else{
			mod_xx.ret=1;
			printf("load successed\n");
		}
	}

}









int quit()
{

	printf("%s%s%d\n",__FILE__,__func__,__LINE__);
}
int exit_()
{
	printf("%s%s%d\n",__FILE__,__func__,__LINE__);

}
int add(int sfd,sqlite3 *db)
{



	printf("%s\t%s\t%d\t\n",__FILE__,__func__,__LINE__);
	int ret;
	char zSql[128]="";
	bzero(zSql,sizeof(zSql));
	char **pResult;
	int pnRow;
	int pnCloumn;
	char *pzErrmsg=NULL;
	sprintf(zSql,"select * from user_msg where user_name=\"%s\" and permission =1",mod_xx.user_name);
	if(sqlite3_get_table(db,zSql,&pResult,&pnRow,&pnCloumn,&pzErrmsg)!=0)
	{
		printf("%s\n",pzErrmsg);
		return -1;
	}
	if(pnRow==0)
	{
		mod_xx.permission=0;

		ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
		if(ret<0){
			perror("load recv error\n");
			return -1;
		}
		return 0;


	}
	else{
		mod_xx.permission=1;

		ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
		if(ret<0){
			perror("load recv error\n");
			return -1;
		}
		//-----------------------------------------------------------------------------

		ret=recv(sfd,(void *)&num_xx,sizeof(num),0);
		if(ret<0)	{
			perror("load recv error\n");
			return -1;
		}


		char sql[128]="";
		char *errmsg=NULL;
		bzero(sql,sizeof(sql));
		sprintf(sql,"insert into staff_msg values(\"%s\",%d,%d)",num_xx.name,num_xx.age,num_xx.salary);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)<0)
		{
			printf("%s\n",errmsg);
			return -1;
		}
		num_xx.ret=1;
		ret=send(sfd,(void *)&num_xx,sizeof(num),0);
		if(ret<0){
			perror("load recv error\n");
			return -1;
		}


	}

}
int del(int sfd,sqlite3 *db)
{
	printf("%s  %s  %d\n",__FILE__,__func__,__LINE__);

	int ret;
	char zSql[128]="";
	bzero(zSql,sizeof(zSql));
	char **pResult;
	int pnRow;
	int pnCloumn;
	char *pzErrmsg=NULL;
	sprintf(zSql,"select * from user_msg where user_name=\"%s\" and permission =1",mod_xx.user_name);
	if(sqlite3_get_table(db,zSql,&pResult,&pnRow,&pnCloumn,&pzErrmsg)!=0)
	{
		printf("%s\n",pzErrmsg);
		return -1;
	}
	if(pnRow==0)
	{
		mod_xx.permission=0;

		ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
		if(ret<0){
			perror("load recv error\n");
			return -1;
		}
		return 0;
	}
	else{
		mod_xx.permission=1;
		ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
		if(ret<0){
			perror("del recv error\n");
			return -1;
		}

		//-----------------------------------------------------------------------------
		ret=recv(sfd,(void *)&num_xx,sizeof(num),0);
		if(ret<0)	{
			perror("del recv error\n");
			return -1;
		}


		char sql[128]="";
		char *errmsg=NULL;
		bzero(sql,sizeof(sql));
		sprintf(sql,"delete from staff_msg where name=\"%s\"",num_xx.name);
		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)<0)
		{
			printf("%s\n",errmsg);
			return -1;
		}
		num_xx.ret=1;
		ret=send(sfd,(void *)&num_xx,sizeof(num),0);
		if(ret<0){
			perror("del recv error\n");
			return -1;
		}


	}

}








int update(int sfd,sqlite3 *db)
{
	printf("%s    %s   %d\n",__FILE__,__func__,__LINE__);



	int ret;
	char zSql[128]="";
	bzero(zSql,sizeof(zSql));
	char **pResult;
	int pnRow;
	int pnCloumn;
	char *pzErrmsg=NULL;
	sprintf(zSql,"select * from user_msg where user_name=\"%s\" and permission =1",mod_xx.user_name);
	if(sqlite3_get_table(db,zSql,&pResult,&pnRow,&pnCloumn,&pzErrmsg)!=0)
	{
		printf("%s\n",pzErrmsg);
		return -1;
	}
	if(pnRow==0)
	{
		mod_xx.permission=0;

		ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);          //send 1
		if(ret<0){
			perror("load recv error\n");
			return -1;
		}
		return 0;
	}
	else{
		mod_xx.permission=1;
		ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);            //send 1
		if(ret<0){
			perror("del recv error\n");
			return -1;
		}

		//-----------------------------------------------------------------------------


		ret=recv(sfd,(void *)&num_xx,sizeof(num),0);			//recv 2
		if(ret<0){
			perror("del recv error\n");
			return -1;
		}
		char **presult;
		int pnrow;
		int pncloumn;
		char *pzerrmsg=NULL;

		bzero(zSql,sizeof(zSql));
		sprintf(zSql,"select * from staff_msg where name=\"%s\"",num_xx.name);
		if(sqlite3_get_table(db,zSql,&presult,&pnrow,&pncloumn,&pzerrmsg)!=0)
		{
			printf("%s\n",pzerrmsg);
			return -1;
		}
		if(pnrow==0)
		{
			printf("not find name\n");
			num_xx.ret=-1;
			ret=send(sfd,&num_xx,sizeof(num),0);					//send 2
			if(ret<0){
				perror("update recv error\n");
				return -1;
			}
			return 0;
		}
		else{
			//	printf("presult[0],[1][2],%s %s %s\n",presult[0],presult[1],presult[2]);
			num_xx.ret=1;
			num_xx.age=atoi(presult[4]);
			num_xx.salary=atoi(presult[5]);

			ret=send(sfd,(void *)&num_xx,sizeof(num),0);				//send 2
			if(ret<0){
				perror("update send error\n");
				return -1;
			}
		}

		//-----------------------------------------------------------------------------

		num num_xx_used=num_xx;

		ret=recv(sfd,(void *)&num_xx,sizeof(num),0);			//recv 3
		if(ret<0){
			perror("update recv error\n");
			return -1;
		}




		char sql[128]="";
		char *errmsg=NULL;
		bzero(sql,sizeof(sql));


		if(strcmp(num_xx.name,num_xx_used.name)!=0){

			sprintf(sql,"update staff_msg set name=\"%s\" where name=\"%s\"",num_xx.name,num_xx_used.name);
		}
		else if(num_xx.age!=num_xx_used.age){
			sprintf(sql,"update staff_msg set age=%d where age=%d and name=\"%s\"",num_xx.age,num_xx_used.age,num_xx.name);

		}
		else if(num_xx.salary!=num_xx_used.salary){
			sprintf(sql,"update staff_msg set salary=%d where salary=%d and name =\"%s\"",num_xx.salary,num_xx_used.salary,num_xx.name);

		}
		else{
			num_xx.ret=-1;
			goto p7;
		}






		if(sqlite3_exec(db,sql,NULL,NULL,&errmsg)<0)
		{
			printf("%s\n",errmsg);
			return -1;
		}
		num_xx.ret=1;
p7:	ret=send(sfd,(void *)&num_xx,sizeof(num),0);					//send 3
	if(ret<0){
		perror("load send error\n");
		return -1;
	}


	}
}

//-----------------------------------------------------------------------------














int find(int sfd,sqlite3 *db)
{
	printf("%s   %s    %d\n",__FILE__,__func__,__LINE__);


	char msg[256]="";
	int ret,i,j;
	char zSql[128]="";
	bzero(zSql,sizeof(zSql));
	char **pResult;
	int pnRow;
	int pnCloumn;
	char *pzErrmsg=NULL;
	sprintf(zSql,"select * from user_msg where user_name=\"%s\" and permission =1",mod_xx.user_name);
	if(sqlite3_get_table(db,zSql,&pResult,&pnRow,&pnCloumn,&pzErrmsg)!=0)
	{
		printf("%s\n",pzErrmsg);
		return -1;
	}

	//-----------------------------------------------------------------------------
	if(pnRow==0)							//发送给普通用户
	{



		char **presult;
		int pnrow;
		int pncloumn;
		char *pzerrmsg=NULL;

		bzero(zSql,sizeof(zSql));
		sprintf(zSql,"select * from staff_msg");
		if(sqlite3_get_table(db,zSql,&presult,&pnrow,&pncloumn,&pzerrmsg)!=0)
		{
			printf("%s\n",pzerrmsg);
			return -1;
		}

		//-----------------------------------------------------------------------------

		//发送给普通用户


		int index=pncloumn;



		for(i=0;i<pnrow;i++)
		{
			bzero(msg,sizeof(msg));
			sprintf(msg,"name:%s age:%s \n",presult[index],presult[index+1]);
			index=index+pncloumn;
			ret=send(sfd,msg,sizeof(msg),0);
			if(ret<0){
				perror("find send error\n");
				return -1;
			}
		
		}
		bzero(msg,sizeof(msg));
		strcpy(msg,"finish");
		ret=send(sfd,msg,sizeof(msg),0);
		if(ret<0){
			perror("find send error\n");
			return -1;
		}






		return 0;
	}
	//-----------------------------------------------------------------------------
	//发送给管理
	else{
		char **presult;
		int pnrow;
		int pncloumn;
		char *pzerrmsg=NULL;

		bzero(zSql,sizeof(zSql));
		sprintf(zSql,"select * from staff_msg");
		if(sqlite3_get_table(db,zSql,&presult,&pnrow,&pncloumn,&pzerrmsg)!=0)
		{
			printf("%s\n",pzerrmsg);
			return -1;
		}

		//-----------------------------------------------------------------------------

		//发送给管理


		int index=pncloumn;




		for(i=0;i<pnrow;i++)
		{
			bzero(msg,sizeof(msg));
			sprintf(msg,"name:%s age:%s salary:%s \n",presult[index],presult[index+1],presult[index+2]);
			index=index+pncloumn;
			ret=send(sfd,msg,sizeof(msg),0);
			if(ret<0){
				perror("find send error\n");
				return -1;
			}
			printf("pnRow%d pnclown%d i%d index %d\n",pnRow,pnCloumn,i,index);
		}
		bzero(msg,sizeof(msg));

		strcpy(msg,"finish");
		ret=send(sfd,msg,sizeof(msg),0);
		if(ret<0){
			perror("find send error\n");
			return -1;
		}


		//-----------------------------------------------------------------------------


	}

}



















