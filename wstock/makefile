VERSION=1.0
DIST_DIR_BASE=wstock-$(VERSION)
DIST_DIR=/tmp/$(DIST_DIR_BASE)/
wstock_bin: 
	`wx-config --cxx --cflags --libs` `xml2-config --cflags --libs` `curl-config --cflags --libs` *.cpp -o wstock.bin
install:
	install -d $(DESTDIR)/usr/bin/
	install wstock $(DESTDIR)/usr/bin/wstock
	install wstock.bin $(DESTDIR)/usr/bin/wstock.bin
	install -d $(DESTDIR)/usr/share/wstock
	install wstock_cfg_linux $(DESTDIR)/usr/share/wstock/wstock_cfg_linux
	install stocks.xml $(DESTDIR)/usr/share/wstock/stocks.xml
	install -d $(DESTDIR)/usr/share/locale
	install zh_CN/wstock.mo $(DESTDIR)/usr/share/locale/wstock.mo
uninstall:
	rm -f /usr/bin/wstock
	rm -rf /usr/share/wstock/
	rm -f /usr/share/locale/wstock.mo
dist:
	mkdir -p $(DIST_DIR)
	cp -r *.cpp $(DIST_DIR)
	cp -r *.h $(DIST_DIR)
	cp -r zh_CN $(DIST_DIR)
	cp wstock makefile *.xml *.wxg *.cbp *.dsp *.dsw *.layout *.spec wstock_cfg_linux $(DIST_DIR)
	cd /tmp && tar zcvf $(DIST_DIR_BASE).tar.gz $(DIST_DIR_BASE)
