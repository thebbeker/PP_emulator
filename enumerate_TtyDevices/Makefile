CXXFLAGS = \
	-Wall \
	-Wextra \
	-Werror \
	-pedantic \
	-std=c++17 \


OBJS =	\
	main.o \
	enumerateTtyDevices.o \


LDLIBS = 	\
	-ludev \


TARGET =	pp-client

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LDLIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
