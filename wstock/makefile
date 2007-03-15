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
