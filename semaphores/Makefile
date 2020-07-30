CFLAGS += -g -O0 -Wall -Wstrict-prototypes

.PHONY: clean

all: libpreload-semaphores.so

libpreload-semaphores.so: preload-semaphores.c
	${CC} \
		${CFLAGS} -fPIC -shared \
		${LDFLAGS} preload-semaphores.c \
		-o $@ -ldl

install: libpreload-semaphores.so
	install -D libpreload-semaphores.so ${DESTDIR}/libpreload-semaphores.so

clean:
	rm -f \
		./libpreload-semaphores.so
