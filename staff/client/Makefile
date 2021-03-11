#================================================================
#   Copyright (C) 2021 hqyj Ltd. All rights reserved.
#   
#   文件名称：Makefile
#   创 建 者：zhoubeibei
#   创建日期：2021年03月08日
#   描    述：
#
#================================================================


APP=test
CFLAGS=-c -g -Wall -I ../include
OBJS=main src obj

export CFLAGS APP

ALL:$(OBJS)

$(OBJS):ECHO
	make -C $@ 
	#进入子目录下的makefile

ECHO:
	@echo $(OBJS)
	#@可去除打印一遍
	#
	#@伪目标,在文件下有同名文件使用
.PHONY:clean test
clean:
	rm ./obj/*.o ./bin/test
test:
	./bin/test






