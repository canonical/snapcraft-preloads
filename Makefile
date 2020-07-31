.PHONY: all
all:
	make -C chromium-content-api $@
	make -C semaphores $@
	make -C bindtextdomain $@

.PHONY: install
install:
	make -C chromium-content-api $@
	make -C semaphores $@
	make -C bindtextdomain $@

.PHONY: clean
clean:
	make -C chromium-content-api $@
	make -C semaphores $@
	make -C bindtextdomain $@
