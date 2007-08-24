Summary: DVD-Video creation software
Name: kmediafactory
Version: 0.2.1
Release: 1
Copyright: GPL
Group: Applications/Multimedia
Prefix: %{_prefix}
URL: http://susku.pyhaselka.fi/damu/software/kmediafactory/index.html
Source0: http://susku.pyhaselka.fi/damu/software/kmediafactory/kmediafactory-0.2.1.tar.bz2
BuildRequires: dvdauthor >= 0.6.11, ImageMagick-c++-devel, mjpegtools
BuildRequires: pkgconfig
BuildRoot: %{_tmppath}/%{name}-root

%description
KMediaFactory is easy to use template based dvd authoring tool. You can quickly create DVD menus for home videos and TV recordings in three simple steps.

%prep
%setup -q

%build
%configure --enable-shared
make %{?_smp_mflags}

%install
rm -rf $RPM_BUILD_ROOT
%makeinstall

%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root)
%doc AUTHORS TODO ChangeLog NEWS README
%{_bindir}/*
%{_includedir}/kmediafactory/*.h
%{_includedir}/*.h
%{_libdir}/kde3/*
%{_libdir}/*
%{_datadir}/applnk/Utilities/*
%{_datadir}/apps/kmediafactory/*
%{_datadir}/apps/kmediafactory_output/*
%{_datadir}/apps/kmediafactory_template/*
%{_datadir}/apps/kmediafactory_video/*
%{_datadir}/doc/HTML/en/kmediafactory/*
%{_datadir}/doc/HTML/en/kmediafactory/index.cache.bz2
%{_datadir}/doc/HTML/en/kmediafactory/index.docbook
%{_datadir}/doc/HTML/en/kmediafactory/index.docbook.orig
%{_datadir}/icons/crystalsvg/128x128/apps/kmediafactory.png
%{_datadir}/icons/crystalsvg/128x128/mimetypes/kmediafactory_project.png
%{_datadir}/icons/crystalsvg/16x16/apps/kmediafactory.png
%{_datadir}/icons/crystalsvg/16x16/mimetypes/kmediafactory_project.png
%{_datadir}/icons/crystalsvg/22x22/apps/kmediafactory.png
%{_datadir}/icons/crystalsvg/22x22/mimetypes/kmediafactory_project.png
%{_datadir}/icons/crystalsvg/32x32/actions/add_video.png
%{_datadir}/icons/crystalsvg/32x32/apps/kmediafactory.png
%{_datadir}/icons/crystalsvg/32x32/mimetypes/kmediafactory_project.png
%{_datadir}/icons/crystalsvg/48x48/apps/kmediafactory.png
%{_datadir}/icons/crystalsvg/48x48/mimetypes/kmediafactory_project.png
%{_datadir}/icons/crystalsvg/64x64/apps/kmediafactory.png
%{_datadir}/icons/crystalsvg/64x64/mimetypes/kmediafactory_project.png
%{_datadir}/icons/crystalsvg/scalable/apps/kmediafactory.svgz
%{_datadir}/icons/crystalsvg/scalable/mimetypes/kmediafactory_project.svgz
%{_datadir}/mimelnk/application/x-kmediafactory.desktop
%{_datadir}/services/kmediafactory_output.desktop
%{_datadir}/services/kmediafactory_template.desktop
%{_datadir}/services/kmediafactory_video.desktop
%{_datadir}/servicetypes/kmediafactoryplugin.desktop




%changelog
* Sun Apr 17 2005 John Rimell <john@rimell.cc>
- Create.
~
