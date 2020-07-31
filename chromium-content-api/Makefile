TARGET = $(shell basename $(CURDIR))
CFLAGS += -g -O0 -Wall -Wstrict-prototypes

.PHONY: clean

all: libpreload-$(TARGET).so

libpreload-$(TARGET).so: preload-$(TARGET).c
	${CC} \
		${CFLAGS} -fPIC -shared \
		${LDFLAGS} preload-$(TARGET).c \
		-o $@ -ldl

install: libpreload-$(TARGET).so
	install -m644 -D libpreload-$(TARGET).so ${DESTDIR}/libpreload-$(TARGET).so

clean:
	rm -f \
		./libpreload-$(TARGET).so
