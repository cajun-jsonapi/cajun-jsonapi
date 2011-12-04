LIB_NAME = cajun
SRCS = \
  reader.cpp \
  writer.cpp \
  elements.cpp

OBJS = $(SRCS:.cpp=.o)

INC_PATH = -I../include

ALL_WARNINGS = -Wall
COMPILE_ONLY = -c
CC_FLAGS = $(INC_PATH) $(ALL_WARNINGS) -fPIC -o

lib$(LIB_NAME).so: $(OBJS)
	g++ -shared -Wl,-soname,libcajun.so $(CC_FLAGS) $@ $^

lib$(LIB_NAME).a: $(OBJS)
	ar rcs $@ $^

%.o: %.cpp
	g++ $(COMPILE_ONLY) $(CC_FLAGS) $@ $<

clean:
	rm $(OBJS) lib$(LIB_NAME).so
