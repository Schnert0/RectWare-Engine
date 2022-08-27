src=\
$(wildcard src/*.c)\
\
$(wildcard src/engine/*.c)\
$(wildcard src/engine/core/*.c)\
$(wildcard src/lib/*.c)\
\
$(wildcard src/slimemageddon/*.c)\
$(wildcard src/slimemageddon/entities/*.c)\
\

obj=$(src:.c=.o)

LDFLAGS= -lSDL2 -framework OpenGL -lglew -O2 -Wall -I.

RectWare: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(obj)
