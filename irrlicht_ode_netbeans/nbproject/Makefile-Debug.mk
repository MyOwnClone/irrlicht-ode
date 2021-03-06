#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=gcc.exe
CCC=g++.exe
CXX=g++.exe
FC=gfortran
AS=as.exe

# Macros
CND_PLATFORM=Cygwin_4.x-Windows
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/_ext/420018985/demo.o \
	${OBJECTDIR}/_ext/420018985/controllableCamera.o \
	${OBJECTDIR}/_ext/420018985/heightMap.o \
	${OBJECTDIR}/_ext/420018985/perlinMath.o \
	${OBJECTDIR}/_ext/420018985/main.o \
	${OBJECTDIR}/_ext/420018985/terrain.o \
	${OBJECTDIR}/_ext/420018985/perlinGenerator.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-std=c++11 -fpermissive
CXXFLAGS=-std=c++11 -fpermissive

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../../libs ../../libs/libIrrlicht.a ../../libs/libode.a

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../irrlicht_ode_netbeans.exe

../irrlicht_ode_netbeans.exe: ../../libs/libIrrlicht.a

../irrlicht_ode_netbeans.exe: ../../libs/libode.a

../irrlicht_ode_netbeans.exe: ${OBJECTFILES}
	${MKDIR} -p ..
	${LINK.cc} -o ../irrlicht_ode_netbeans.exe ${OBJECTFILES} ${LDLIBSOPTIONS} 

${OBJECTDIR}/_ext/420018985/demo.o: ../irrlicht_ode/demo.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/420018985
	${RM} $@.d
	$(COMPILE.cc) -g -DdDOUBLE -I../../headers/include/irrlicht -I../../headers/include/ -I../../headers/ -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/420018985/demo.o ../irrlicht_ode/demo.cpp

${OBJECTDIR}/_ext/420018985/controllableCamera.o: ../irrlicht_ode/controllableCamera.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/420018985
	${RM} $@.d
	$(COMPILE.cc) -g -DdDOUBLE -I../../headers/include/irrlicht -I../../headers/include/ -I../../headers/ -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/420018985/controllableCamera.o ../irrlicht_ode/controllableCamera.cpp

${OBJECTDIR}/_ext/420018985/heightMap.o: ../irrlicht_ode/heightMap.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/420018985
	${RM} $@.d
	$(COMPILE.cc) -g -DdDOUBLE -I../../headers/include/irrlicht -I../../headers/include/ -I../../headers/ -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/420018985/heightMap.o ../irrlicht_ode/heightMap.cpp

${OBJECTDIR}/_ext/420018985/perlinMath.o: ../irrlicht_ode/perlinMath.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/420018985
	${RM} $@.d
	$(COMPILE.cc) -g -DdDOUBLE -I../../headers/include/irrlicht -I../../headers/include/ -I../../headers/ -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/420018985/perlinMath.o ../irrlicht_ode/perlinMath.cpp

${OBJECTDIR}/_ext/420018985/main.o: ../irrlicht_ode/main.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/420018985
	${RM} $@.d
	$(COMPILE.cc) -g -DdDOUBLE -I../../headers/include/irrlicht -I../../headers/include/ -I../../headers/ -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/420018985/main.o ../irrlicht_ode/main.cpp

${OBJECTDIR}/_ext/420018985/terrain.o: ../irrlicht_ode/terrain.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/420018985
	${RM} $@.d
	$(COMPILE.cc) -g -DdDOUBLE -I../../headers/include/irrlicht -I../../headers/include/ -I../../headers/ -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/420018985/terrain.o ../irrlicht_ode/terrain.cpp

${OBJECTDIR}/_ext/420018985/perlinGenerator.o: ../irrlicht_ode/perlinGenerator.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/420018985
	${RM} $@.d
	$(COMPILE.cc) -g -DdDOUBLE -I../../headers/include/irrlicht -I../../headers/include/ -I../../headers/ -MMD -MP -MF $@.d -o ${OBJECTDIR}/_ext/420018985/perlinGenerator.o ../irrlicht_ode/perlinGenerator.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../irrlicht_ode_netbeans.exe

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
