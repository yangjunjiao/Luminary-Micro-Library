#******************************************************************************
#
# Makefile - Rules for building the oscilloscope quickstart application.
#
# Copyright (c) 2008-2012 Texas Instruments Incorporated.  All rights reserved.
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
# This is part of revision 9453 of the EK-LM3S3748 Firmware Package.
#
#******************************************************************************

#
# Defines the part type that this project uses.
#
PART=LM3S3748

#
# Set the processor variant.
#
VARIANT=cm3

#
# The base directory for StellarisWare.
#
ROOT=../../..

#
# Include the common make definitions.
#
include ${ROOT}/makedefs

#
# Where to find source files that do not live in this directory.
#
VPATH=../drivers
VPATH+=../../../third_party/fatfs/port
VPATH+=../../../third_party/fatfs/src
VPATH+=../../../utils

#
# Where to find header files that do not live in the source directory.
#
IPATH=..
IPATH+=../../..
IPATH+=../../../third_party

#
# The default rule, which causes the oscilloscope quickstart application to be built.
#
all: ${COMPILER}
all: ${COMPILER}/qs-scope.axf

#
# The rule to clean out all the build products.
#
clean:
	@rm -rf ${COMPILER} ${wildcard *~}

#
# The rule to create the target directory.
#
${COMPILER}:
	@mkdir -p ${COMPILER}

#
# Rules for building the oscilloscope quickstart application.
#
${COMPILER}/qs-scope.axf: ${COMPILER}/buttons.o
${COMPILER}/qs-scope.axf: ${COMPILER}/class-d.o
${COMPILER}/qs-scope.axf: ${COMPILER}/cmdline.o
${COMPILER}/qs-scope.axf: ${COMPILER}/commands.o
${COMPILER}/qs-scope.axf: ${COMPILER}/data-acq.o
${COMPILER}/qs-scope.axf: ${COMPILER}/dual-disk-driver.o
${COMPILER}/qs-scope.axf: ${COMPILER}/ff.o
${COMPILER}/qs-scope.axf: ${COMPILER}/file.o
${COMPILER}/qs-scope.axf: ${COMPILER}/formike128x128x16.o
${COMPILER}/qs-scope.axf: ${COMPILER}/images.o
${COMPILER}/qs-scope.axf: ${COMPILER}/menu-controls.o
${COMPILER}/qs-scope.axf: ${COMPILER}/menu.o
${COMPILER}/qs-scope.axf: ${COMPILER}/qs-scope.o
${COMPILER}/qs-scope.axf: ${COMPILER}/renderer.o
${COMPILER}/qs-scope.axf: ${COMPILER}/startup_${COMPILER}.o
${COMPILER}/qs-scope.axf: ${COMPILER}/uartstdio.o
${COMPILER}/qs-scope.axf: ${COMPILER}/usb_device.o
${COMPILER}/qs-scope.axf: ${COMPILER}/usb_host.o
${COMPILER}/qs-scope.axf: ${COMPILER}/usbdescriptors.o
${COMPILER}/qs-scope.axf: ${COMPILER}/ustdlib.o
${COMPILER}/qs-scope.axf: ${ROOT}/usblib/${COMPILER}-cm3/libusb-cm3.a
${COMPILER}/qs-scope.axf: ${ROOT}/grlib/${COMPILER}-cm3/libgr-cm3.a
${COMPILER}/qs-scope.axf: ${ROOT}/driverlib/${COMPILER}-cm3/libdriver-cm3.a
${COMPILER}/qs-scope.axf: qs-scope.ld
SCATTERgcc_qs-scope=qs-scope.ld
ENTRY_qs-scope=ResetISR
CFLAGSgcc=-DDISK0_EK_LM3S3748 -DDISK1_USB_MSC -DUART_BUFFERED -DTARGET_IS_DUSTDEVIL_RA0

#
# Include the automatically generated dependency files.
#
ifneq (${MAKECMDGOALS},clean)
-include ${wildcard ${COMPILER}/*.d} __dummy__
endif
