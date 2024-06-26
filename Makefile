# 定义源文件和目标文件
SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))

# 定义头文件
DEPS := $(wildcard *.h)

# 编译规则
%.o: %.cpp $(DEPS)
	g++ -g -c $< -o $@ -lmysqlclient

# 链接规则
tcpepoll: $(OBJS)
	g++ -g -o tcpepoll $^ -lmysqlclient

# 清理规则
clean:
	rm -rf tcpepoll $(OBJS)
