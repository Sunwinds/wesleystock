VERSION=1.0
DIST_DIR_BASE=wstock-$(VERSION)
DIST_DIR=/tmp/$(DIST_DIR_BASE)/
wstock_bin: 
	`wx-config --cxx --cflags --libs` `xml2-config --cflags --libs` `curl-config --cflags --libs` *.cpp -o wstock
install:
	install wstock /usr/bin/wstock
	install wstock_cfg_linux ~/.wstock
	install -m  777 -d ~/.wstock_data
	install stocks.xml ~/.wstock_data/stocks.xml
	install -d /usr/share/locale
	install zh_CN/wstock.mo /usr/share/locale/wstock.mo
uninstall:
	rm -f /usr/bin/wstock
	rm -rf ~/.wstock
	rm -rf ~/.wstock_data
	rm -f /usr/share/locale/wstock.mo
dist:
	mkdir -p $(DIST_DIR)
	cp -r *.cpp $(DIST_DIR)
	cp -r *.h $(DIST_DIR)
	cp -r zh_CN $(DIST_DIR)
	cp makefile *.xml *.wxg *.cbp *.dsp *.dsw *.layout *.spec wstock_cfg_linux $(DIST_DIR)
	cd /tmp && tar zcvf $(DIST_DIR_BASE).tar.gz $(DIST_DIR_BASE)
