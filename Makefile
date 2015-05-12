all :
INCLUDE = -I./ -I./include

FLAGS = -g -Wall $(INCLUDE)
FLAGS += -fPIC

LIBDIR = -L./lib_64 -lz -lm -lpthread

LINK = $(LIBDIR) -lpthread

GCC = g++

# for C++ language
CODE.cpp = listen.cpp \
			main.cpp \
			public_func.cpp

CPP.o = $(CODE.cpp:.cpp=.o)
OBJS.d = $(CODE.cpp:.cpp=.d)
OBJS.o = $(CPP.o)

# 解决头文件依赖
-include $(subst .cpp,.d,$(CODE.cpp))

%.d: %.cpp
	$(GCC) -M $(FLAGS) $< > $@.$$$$;		\
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@;	\
	rm -f $@.$$$$

# rule for C++ language
%.o : %.cpp
	$(GCC) $(FLAGS) -o $@ -c $<	
	@echo $*.o build successfully!......

TARGET = listen_app 
	
$(TARGET) : $(OBJS.o) 
	$(GCC) $(OBJS.o) -o $(TARGET) $(LINK)
	@echo $(TARGET) BUILD OK!.........

all : $(TARGET)

.PHONY:
clean:
	rm -rf $(TARGET)
	rm -rf $(OBJS.o)
	rm -rf $(OBJS.d)
	rm -rf *.d
