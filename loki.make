VER = 0.1.5

all: loki/lib/libloki.a

loki-$(VER).tar.gz:
	wget http://surfnet.dl.sourceforge.net/sourceforge/loki-lib/loki-$(VER).tar.gz

loki-$(VER): loki-$(VER).tar.gz
	tar zxf loki-$(VER).tar.gz

loki: loki-$(VER)
	mv loki-$(VER) loki

loki/lib/libloki.a: loki
	make -C loki build-static
