CFLAGS=-O0 -g -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -Wno-deprecated-declarations -Iinclude
CPPFLAGS=-MMD
LDFLAGS=
LDLIBS=
 
OBJS=tests/test_allocator.o src/m_allocator.o
DEPS=$(OBJS:.o=.d)
 
.PHONY: all clean
 
all: test_allocator
 
-include $(DEPS)

%.o:  %.c 
	gcc -Wdeprecated-declarations -c $(CFLAGS) $(CPPFLAGS) $< -o $@
	
test_allocator: $(OBJS)
	gcc $(CFLAGS) $(CPPFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

clean:
	rm -f $(OBJS) $(DEPS) test_allocator