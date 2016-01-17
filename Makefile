BINARY := jenkins-bf

CFLAGS := -std=gnu11 -Wall -Wextra -Wpedantic -Winit-self -Wshadow \
	  -Wcast-qual -Wstrict-prototypes -Wmissing-prototypes \
	  -MMD -MP
LDFLAGS := -Wl,--as-needed

SRCS := $(wildcard ./*.c)

INSTALL := install -m755

PREFIX := /usr/local

DEBUG ?= 0
ifeq ($(DEBUG), 1)
	CFLAGS  += -Og -g
else
	CFLAGS  += -O2 -DNDEBUG
	LDFLAGS += -O2
endif

all: $(BINARY)

$(BINARY): $(SRCS:.c=.o)

install: $(BINARY)
	$(INSTALL) $^ $(DESTDIR)$(PREFIX)/bin
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(BINARY)

TAGS:
	etags *.{c,h}
tags:
	ctags -R .

cscope: cscope.out
cscope.out: $(SRCS)
	cscope -b -e -q -R

clean:
	$(RM) $(BINARY) tags TAGS cscope.in.out cscope.out cscope.po.out
	find . -type f -name "*.[od]" -delete

-include $(SRCS:.c=.d)

.PHONY: all install uninstall clean cscope
