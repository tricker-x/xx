/*================================================================
*   Copyright (C) 2021 hqyj Ltd. All rights reclived.
*   
*   文件名称：cli_MMS_main.c
*   创 建 者：xx
*   创建日期：2021年03月09日
*   描    述：客户端
*
================================================================*/


#include "MMS.h"





int main(int argc, char *argv[])
{
	int ret;
	ret=cli_ipconfig();
	if(ret<0){
		return -1;
	}

	

	cli_load_ui(sfd);



	
	return 0;
}

