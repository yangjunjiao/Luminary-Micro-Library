#******************************************************************************
#
# Makefile - Rules for building the driver library and examples.
#
# Copyright (c) 2005-2012 Texas Instruments Incorporated.  All rights reserved.
# Software License Agreement
# 
# Texas Instruments (TI) is supplying this software for use solely and
# exclusively on TI's microcontroller products. The software is owned by
# TI and/or its suppliers, and is protected under applicable copyright
# laws. You may not combine this software with "viral" open-source
# software in order to form a larger program.
# 
# THIS SOFTWARE IS PROVIDED "AS IS" AND WITH ALL FAULTS.
# NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, INCLUDING, BUT
# NOT LIMITED TO, IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE. TI SHALL NOT, UNDER ANY
# CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR CONSEQUENTIAL
# DAMAGES, FOR ANY REASON WHATSOEVER.
# 
# This is part of revision 8555 of the Stellaris Firmware Development Package.
#
#******************************************************************************

DIRS=driverlib \
     grlib     \
     usblib    \
     boards

all::
	@for i in ${DIRS};                  \
	 do                                 \
	     if [ -f $${i}/Makefile ];      \
	     then                           \
	         make -C $${i} || exit $$?; \
	     fi;                            \
	 done

clean::
	@rm -f ${wildcard *~} __dummy__
	@rm -f ${wildcard utils/*~} __dummy__
	@for i in ${DIRS};             \
	 do                            \
	     if [ -f $${i}/Makefile ]; \
	     then                      \
	         make -C $${i} clean;  \
	     fi;                       \
	 done
