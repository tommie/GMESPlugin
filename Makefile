CPPFLAGS = -Igme
CFLAGS = -O3 -Wall -fPIC -ggdb -std=gnu99
CXXFLAGS = -O3 -Wall -fPIC -ggdb
LDFLAGS = -Wl,--no-undefined -fPIC -shared

GME_OBJS = $(patsubst %.cpp,%.o,$(wildcard gme/gme/*.cpp))
OBJS = $(patsubst %.c,%.o,$(wildcard *.c)) $(GME_OBJS)

.PHONY: all
all: gmes.splugin

.PHONY: clean
clean:
	rm -fr $(OBJS)

.PHONY: distclean
distclean: clean
	rm -fr gmes.splugin

gmes.splugin: $(OBJS)
	$(CXX) $(LDFLAGS) $(CXXFLAGS) -o $@ $^
