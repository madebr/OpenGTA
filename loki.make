VER = 0.1.7

all: loki/lib/libloki.a

loki-$(VER).tar.gz:
	wget -O loki-$(VER).tar.gz https://sourceforge.net/projects/loki-lib/files/Loki/Loki%20$(VER)/loki-$(VER).tar.gz/download

loki-$(VER): loki-$(VER).tar.gz
	tar zxf loki-$(VER).tar.gz

loki: loki-$(VER)
	mv loki-$(VER) loki

loki/lib/libloki.a: loki
	make -C loki build-static
