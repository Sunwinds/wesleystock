Summary: A program that manage your stocks in China.
Name: wstock
Version: 1.0
Release: 1
Group: Applications/Internet
License:                GPL
Source: wstock-1.0.tar.gz
BuildRoot: /var/tmp/%{name}-buildroot

%description

%prep
%setup

%build
make

%install
%__make DESTDIR=$RPM_BUILD_ROOT install

%clean
make uninstall

%files
/usr/bin/wstock
/usr/bin/wstock.bin
/usr/share/wstock/stocks.xml
/usr/share/wstock/wstock_cfg_linux
/usr/share/locale/wstock.mo

%changelog
