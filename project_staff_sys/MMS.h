/*================================================================
 *   Copyright (C) 2021 hqyj Ltd. All rights reclived.
 *   
 *   文件名称：MMS.h
 *   创 建 者：xx
 *   创建日期：2021年03月09日
 *   描    述：客户端.h
 *
 ================================================================*/


#ifndef MMS_H__
#define MMS_H__

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <string.h>
#include <errno.h>
#include <netinet/ip.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>



#define COMMAND 123456


typedef struct modbus{
	char order;       //regist load quit exit
	//add del update find 
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
int sfd;

int cli_ipconfig();

int cli_load_ui();
int cli_regist();

void cli_user_ui();

#endif //MMS_H__

