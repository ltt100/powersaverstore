Name:       powersaver

Summary:    powersaver daemon
Version:    1.0.0
Release:    1%{?dist}
Group:      System/System Control
License:    YXPL
Source0:    %{name}-%{version}.tar.bz2
Requires:   
Requires:   systemd
BuildRequires:  pkgconfig(Qt5Core)
BuildRequires:  pkgconfig(Qt5DBus)
BuildRequires:  pkgconfig(systemd)
BuildRequires:  pkgconfig(syberos-application)

%description
The powersaver daemon (%{name}) provides power saving service and closes different apps depending on current power status.


%prep
%setup -q -n %{name}-%{version}/powersaver

%build

%qmake5 

make -j8

%install
rm -rf %{buildroot}
%qmake5_install

install -d %{buildroot}%{_libdir}/systemd/user/pre-user-session.target.wants/
ln -s ../%{name}.service %{buildroot}%{_libdir}/systemd/user/pre-user-session.target.wants/%{name}.service


# system changes time zone by linking /data/systemservice/timemanager/localtime to zones in /usr/share/zoneinfo.
# Initial links are done in the post section
install -d %{buildroot}/data/systemservice/powersaver
chmod 775 %{buildroot}/data/systemservice/powersaver
#touch %{buildroot}/data/systemservice/timemanager/localtime

%post
# Make /etc/localtime a link to /data/systemservice/timemanager/localtime to make system time zone follow time deamon.
# Remove existing link so that copying the UTC file will not overwrite anything during reinstall.
#rm -f /data/systemservice/timemanager/localtime
#ln -sf /usr/share/zoneinfo/Asia/Shanghai /data/systemservice/timemanager/localtime
#ln -sf /data/systemservice/timemanager/localtime /etc/localtime
#chmod 664 /data/systemservice/timemanager/localtime
#chown system:system /data/systemservice/timemanager/localtime

if [ "$1" -ge 1 ]; then
systemctl --user daemon-reload || :
systemctl --user restart %{name}.service || :
fi

%preun
systemctl --user stop %{name}.service

%postun
/sbin/ldconfig
if [ "$1" -eq 0 ]; then
systemctl --user stop {%name}.service || :
systemctl --user daemon-reload || :
getent group time >/dev/null && groupdel system
fi


%files
%defattr(-,dbus,dbus,-)
%config(noreplace) %{_sysconfdir}/dbus-1/system.d/%{name}.conf
#%config(noreplace) %{_prefix}/%{_sysconfdir}/%{name}.rc

# Todo: need get a uid and gid
%defattr(-,timed,timed,-)
%{_bindir}/%{name}
%{_libdir}/systemd/user/%{name}.service
%{_libdir}/systemd/user/pre-user-session.target.wants/%{name}.service
%dir %attr(0775,system,system) /data/systemservice/timemanager
%ghost %attr(664,system,system) /data/systemservice/timemanager/localtime

