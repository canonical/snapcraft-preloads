TARGETS := semaphores

all:
	$(foreach dir, $(TARGETS), $(MAKE) -C $(dir);)

install:
	$(foreach dir, $(TARGETS), $(MAKE) install -C $(dir);)

clean:
	$(foreach dir, $(TARGETS), $(MAKE) clean -C $(dir);)
