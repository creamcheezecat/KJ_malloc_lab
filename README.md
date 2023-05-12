#####################################################################
# CS:APP Malloc Lab
# Handout files for students
#
# Copyright (c) 2002, R. Bryant and D. O'Hallaron, All rights reserved.
# May not be used, modified, or copied without permission.
#
######################################################################

***********
Main Files:
***********

mm.{c,h}	
	Your solution malloc package. mm.c is the file that you
	will be handing in, and is the only file you should modify.

	당신의 솔루션 malloc 패키지입니다. mm.c는
	제출할 파일이며, 수정해야하는 유일한 파일입니다.

mdriver.c	
	The malloc driver that tests your mm.c file
	당신의 mm.c 파일을 테스트하는 malloc 드라이버

short{1,2}-bal.rep
	Two tiny tracefiles to help you get started. 
	시작하기 위한 두 개의 작은 추적 파일입니다.

Makefile	
	Builds the driver

**********************************
Other support files for the driver
**********************************

config.h	Configures the malloc lab driver  
malloc lab 드라이버 구성

fsecs.{c,h}	Wrapper function for the different timer packages 
다른 타이머 패키지를 위한 래퍼 함수

Wrapper = 다른 함수나 모듈의 기능을 추가하거나, 변경하거나, 특정한 목적에 맞게 사용하기 위한 인터페이스 역할

clock.{c,h}	Routines for accessing the Pentium and Alpha cycle counters
Pentium 및 Alpha 사이클 카운터에 대한 루틴

fcyc.{c,h}	Timer functions based on cycle counters
사이클 카운터를 기반으로한 타이머 함수

ftimer.{c,h}	Timer functions based on interval timers and gettimeofday() 
간격 타이머 및 gettimeofday() 기반 타이머 함수

memlib.{c,h}	Models the heap and sbrk function 
힙과 sbrk 함수를 모델링합니다.

*******************************
Building and running the driver
*******************************
To build the driver, type "make" to the shell.

To run the driver on a tiny test trace:

	unix> mdriver -V -f short1-bal.rep

The -V option prints out helpful tracing and summary information.

To get a list of the driver flags:

	unix> mdriver -h

