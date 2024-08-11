ZK_CAPI_PATH := /usr/local

INC_PATH := ${ZK_CAPI_PATH}/include/zookeeper
SRC_PATH := ./zookeeperTest.cc
LIB_PATH := ${ZK_CAPI_PATH}/lib

LIB_NAME := zookeeper_mt

EXE_NAME := zookeeper_test

main:
	gcc -g -DTHREADED ${SRC_PATH} -I ${INC_PATH} -o ${EXE_NAME} -L ${LIB_PATH} -l ${LIB_NAME}
clean:
	rm ${EXE_NAME}