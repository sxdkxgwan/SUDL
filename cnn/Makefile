MACRO=DEBUGALL 
CFLAGS+=-w -g -std=c++11
	SRCPATH=.
	INCLUDEPATH=. ../matrix ../layers ../utils
	TARGET=test_conv_layer 
SOURCES=$(wildcard $(SRCPATH)/*.cpp)
ALIAS_PATH=../layers
ALIAS_O=$(wildcard $(ALIAS_PATH)/*.o)

#OBJS := $(patsubst %.cpp, %.o,$(SOURCES))
OBJS=$(SOURCES:.cpp=.o)

CFLAGS+=$(foreach dir, $(INCLUDEPATH), -I$(dir))

all: build desc
	@echo "Build Done!"

desc:
	@echo ${ALIAS_O}

clean:
	$(RM) $(OBJS) $(TARGET)

build:
	g++ -c ${CFLAGS} ${SOURCES}
	g++ ${CFLAGS} -o ${TARGET} ${OBJS} ${ALIAS_O} 
