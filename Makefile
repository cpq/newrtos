EXAMPLES = $(wildcard examples/*)
ARCHITECTURES = stm32f0 stm32f1 stm32f3 #stm32f7

# Build firmwares for all supported architectures
all:
	for A in $(ARCHITECTURES) ; do \
    for E in $(EXAMPLES) ; do \
      make -C $$E ARCH=$$A clean all ; \
    done; \
  done

clean:
	for E in $(EXAMPLES) ; do make -C $$E clean ; done
