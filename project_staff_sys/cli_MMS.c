/*================================================================
 *   Copyright (C) 2021 hqyj Ltd. All rights reclived.
 *   
 *   文件名称：cli_MMS.c
 *   创 建 者：xx
 *   创建日期：2021年03月09日
 *   描    述：客户端函数
 *
 ================================================================*/


#include "MMS.h"


int cli_ipconfig()
{
	int ret;
	sfd=socket(AF_INET,SOCK_STREAM,0);
	if(sfd<0)
	{
		perror("cli_socket error\n");
	}
	struct sockaddr_in sin;
	sin.sin_family=AF_INET;
	sin.sin_port=htons(3116);
	sin.sin_addr.s_addr=inet_addr("192.168.8.151");
	ret=connect(sfd,(struct sockaddr *)&sin,sizeof(sin));
	if(ret==-1){
		perror("cli_connect error\n");
		return -1;
	}
	printf("conneted\n");
	return 0;
}











//登录界面
int  cli_load_ui()
{
	int ret;
	while(1)
	{
		printf("**********MMS***********\n");
p5:		printf("r for regist\nl for load\ne for exit\n");
		scanf("%c",&mod_xx.order);
		while(getchar()!='\n');

		if(mod_xx.order=='r'||mod_xx.order=='l'||mod_xx.order=='e'){

		}else{
			printf("please enter right order\n");
			goto p5;
		}
		switch(mod_xx.order){
		case 'r':                    //regist
			ret=cli_regist();
			if(ret<0){
				return -1;
			}
			break;

		case 'l':					
			ret=cli_load();
			if(ret<0){
				return -1;
			}
			cli_user_ui();
			break;


		case 'e':
			exit(0);
			break;
		}
	}
	return 0;
}





//管理员界面
void cli_user_ui()
{

	system("clear");
	printf("welcome ,%s\n",mod_xx.user_name);
	while(1)
	{
p4:		printf("a for add\nd for del\nu for update\nf for find\nq for quit\ne for exit\n");

		scanf("%c",&mod_xx.order);
		while(getchar()!='\n');
		if(mod_xx.order=='a'||mod_xx.order=='d'||mod_xx.order=='u'||mod_xx.order=='f'||mod_xx.order=='q'||mod_xx.order=='e'){

		}else{
			printf("please enter right order\n");
			goto p4;
		}
		switch(mod_xx.order){
		case 'a':
			cli_add();
			break;
		case 'd':
			cli_del();
			break;
		case 'u':
			cli_update();
			break;

		case 'f':
			cli_find();
			break;
		case 'q':
			bzero(&mod_xx,sizeof(mod));
			bzero(&num_xx,sizeof(num));
			cli_load_ui();
			break;
		case 'e':
			exit(0);
			break;
		}
	}
}




int cli_regist()
{
	int command,ret;
	char yorn;
p1:	printf("if regist as administrator (y or n) \n");
	scanf("%c",&yorn);
	while(getchar()!='\n');
	if(yorn=='y'){
		printf("plese enter your command\n");
		scanf("%d",&command);
		while(getchar()!='\n');
		if(command==COMMAND){
			mod_xx.permission=1;
		}
		else{
			printf("command error\n");
		}
	}
	else if(yorn=='n'){
	}
	else{
		printf("PLESE ENTER y or n\n");
		goto p1;
	}

	system("clear");

	printf("pless enter your username\n");
	scanf("%s",mod_xx.user_name);
	while(getchar()!='\n');
	printf("pless enter your password\n");
	scanf("%s",mod_xx.user_password);
	while(getchar()!='\n');
	ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
	if(ret==-1){
		perror("register  send error\n");
		return -1;
	}
	printf("sent %d\n",ret);
	ret=recv(sfd,(void *)&mod_xx,sizeof(mod),0);

	if(ret<0){
		perror("register recv error\n");
		return -1;
	}
	printf("mod_xx.ret=%d\n",mod_xx.ret);
	if(mod_xx.ret==-1){
		printf("name exist\n");
	}
	else if(mod_xx.ret==-2){
		printf("password error\n");
	}
	else{
		printf("regist success\n");
	}

	return 0;

}







int cli_load()
{
	int ret;
p2:	system("clear");
	printf("load \nplease enter your username\n");

	scanf("%s",mod_xx.user_name);
	while(getchar()!='\n');
	printf("please enter your password\n");

	scanf("%s",mod_xx.user_password);
	while(getchar()!='\n');


	ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
	if(ret<0)	{
		perror("load send error\n");
		return -1;
	}



	ret=recv(sfd,(void *)&mod_xx,sizeof(mod),0);
	if(ret<0)	{
		perror("load recv error\n");
		return -1;
	}
	if(mod_xx.ret==-1){
		printf("not regist\n");
		goto p2;

	}
	else if(mod_xx.ret==-2){
		printf("password error\n");
		goto p2;
	}

	return 0;
}
int cli_exit()
{

}
int cli_add()
{
	int ret;
	system("clear");
	ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
	if(ret<0)	{
		perror("load send error\n");
		return -1;
	}
	ret=recv(sfd,(void *)&mod_xx,sizeof(mod),0);
	if(ret<0){
		perror("load recv error\n");
		return -1;
	}
	if(mod_xx.permission!=1){
		printf("permission denied\n");
		return 0;
	}
	else{
		printf("enter staff name:\n");
		scanf("%s",num_xx.name);
		while(getchar()!='\n');
		printf("enter staff id:\n");
		scanf("%d",&num_xx.age);
		while(getchar()!='\n');
		printf("enter staff salary:\n");
		scanf("%d",&num_xx.salary);
		while(getchar()!='\n');


		ret=send(sfd,(void *)&num_xx,sizeof(num),0);
		if(ret<0){
			perror("load recv error\n");
			return -1;
		}
		ret=recv(sfd,(void *)&num_xx,sizeof(num),0);
		if(ret<0)	{
			perror("load recv error\n");
			return -1;
		}
		if(num_xx.ret>0){
			printf("add succeed\n");
		}


	}

	return 0;
}
int cli_del()
{
	int ret;
	char cmd;
	system("clear");
	ret=send(sfd,(void *)&mod_xx,sizeof(mod),0);
	if(ret<0)	{
		perror("load send error\n");
		return -1;
	}
	ret=recv(sfd,(void *)&mod_xx,sizeof(mod),0);
	if(ret<0){
		perror("load recv error\n");
		return -1;
	}
	if(mod_xx.permission!=1){
		printf("permission denied\n");
		return 0;
	}
	else{
p3:		printf("enter del staff name:\n");
		scanf("%s",num_xx.name);
		while(getchar()!='\n');
		printf("his/her name,age,salary:%s,%d,%d\nsure del?(y or n)\n",num_xx.name,num_xx.age,num_xx.salary);
		scanf("%c",&cmd);
		while(getchar()!='\n');
		
		if(cmd=='y'){


			ret=send(sfd,(void *)&num_xx,sizeof(num),0);
			if(ret<0){
				perror("del send error\n");
				return -1;
			}


			ret=recv(sfd,(void *)&num_xx,sizeof(num),0);
			if(ret<0){
				perror("del recv error\n");
				return -1;
			}
			printf("del succeed\n");


		}
		else if(cmd=='n'){
			return 0;
		}
		else{
			printf("PLESE ENTER y or n\n");
			goto p3;
		}
	}
	return 0;

}



int cli_update()
{
	system("clear");
	int ret;
	ret=send(sfd,&mod_xx,sizeof(mod),0);				//send 1
	if(ret<0){
		perror("update send error\n");
		return -1;
	}

	ret=recv(sfd,(void *)&mod_xx,sizeof(mod),0);     //recv 1
	if(ret<0){
		perror("update recv error\n");
		return -1;
	}
	if(mod_xx.permission!=1){
		printf("permission denied\n");
		return 0;
	}
	else{
		printf("witch one\n");
		scanf("%s",num_xx.name);
		while(getchar()!='\n');
		ret=send(sfd,&num_xx,sizeof(num),0);				   //send 2
		if(ret<0)
		{
			perror("update send error\n");
			return -1;
		}
		ret=recv(sfd,&num_xx,sizeof(num),0);				//recv 2
		if(ret<0){
			perror("update recv error\n");
			return -1;
		}
		if(num_xx.ret<0){
			printf("no such name\n");
			return 0;
		}

		

p6:		printf("witch of the following\n name:%s,age:%d,salary:%d\n",num_xx.name,num_xx.age,num_xx.salary);
		char option[16];
		scanf("%s",option);
		while(getchar()!='\n');
		if(strcmp(option,"name")==0){
			printf("new name:\n");
			scanf("%s",num_xx.name);
			while(getchar()!='\n');
			ret=send(sfd,&num_xx,sizeof(num),0);			//send 3
			if(ret<0){
				perror("update send error\n");
				return -1;
			}
			ret=recv(sfd,&num_xx,sizeof(num),0);			//recv 3
			if(ret<0){
				perror("update recv error\n");
				return -1;
			}
			if(num_xx.ret<0){
				printf("not modified\n");
				return 0;
			}else if(num_xx.ret>0){
				printf("update succeed\n");
				return 0;
			}

		}
		else if(strcmp(option,"age")==0){
			
			printf("new age:\n");
			scanf("%d",&num_xx.age);
			while(getchar()!='\n');
			ret=send(sfd,&num_xx,sizeof(num),0);			//send 3
			if(ret<0){
				perror("update send error\n");
				return -1;
			}
			ret=recv(sfd,&num_xx,sizeof(num),0);			//recv 3
			if(ret<0){
				perror("update recv error\n");
				return -1;
			}
			if(num_xx.ret<0){
				printf("not modified\n");
				return 0;
			}else if(num_xx.ret>0){
				printf("update succeed\n");
				return 0;
			}


		}
		else if(strcmp(option,"salary")==0){

			printf("new salary:\n");
			scanf("%d",&num_xx.salary);
			while(getchar()!='\n');
			ret=send(sfd,&num_xx,sizeof(num),0);			//send 3
			if(ret<0){
				perror("update send error\n");
				return -1;
			}
			ret=recv(sfd,&num_xx,sizeof(num),0);			//recv 3
			if(ret<0){
				perror("update recv error\n");
				return -1;
			}
			if(num_xx.ret<0){
				printf("not modified\n");
				return 0;
			}else if(num_xx.ret>0){
				printf("update succeed\n");
				return 0;
			}

		}
		else{
			printf("reenter\n");
			goto p6;
		}


	
	
	}
	return 0;

	
	


}
int cli_find()
{
	system("clear");
	int ret;
	char msg[256]="";
	ret=send(sfd,&mod_xx,sizeof(mod),0);				//send 1
	if(ret<0){
		perror("infd send error\n");
		return -1;
	}


	while(1)
	{
		bzero(msg,sizeof(msg));
		ret=recv(sfd,msg,sizeof(msg),0);
		if(ret<0)
		{
			perror("find recv error\n");
			return -1;
		}
		printf("%s \n",msg);
		if(strcmp(msg,"finish")==0){
			return 0;
		}
	}
	return 0;




}








