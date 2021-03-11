#================================================================
#   Copyright (C) 2021 hqyj Ltd. All rights reserved.
#   
#   文件名称：Makefile
#   创 建 者：zhoubeibei
#   创建日期：2021年03月08日
#   描    述：
#
#================================================================

../bin/$(APP):main.o init.o epoll_work.o
	    gcc $^ -o $@ -pthread -lsqlite3

