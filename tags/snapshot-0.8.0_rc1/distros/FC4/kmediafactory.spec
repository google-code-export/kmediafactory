Name:           kmediafactory
Version:        0.4.0
Release:        1%{?dist}
Summary:        KMediaFactory is a template based DVD authoring tool

Group:          User Interface/Desktops
License:        GPL
URL:            http://susku.pyhaselka.fi/damu/software/kmediafactory/
Source0:        http://susku.pyhaselka.fi/damu/software/kmediafactory/%{name}-%{version}.tar.bz2
BuildRoot:      %{_tmppath}/%{name}-%{version}-%{release}-root-%(%{__id_u} -n)

BuildRequires:  xorg-x11-devel qt-devel kdelibs-devel libdv-devel libjpeg-devel libdvdread-devel
BuildRequires:  xine-lib-devel ImageMagick-devel ImageMagick-c++-devel lcms-devel libtiff-devel bzip2-devel 
Requires:       dvdauthor >= 0.6.11
Requires:	mjpegtools

%description
It is a easy to use template based dvd authoring tool. 
You can quickly create DVD menus for home videos and TV 
recordings in three simple steps.

%package devel
Summary: Development files for kmediafactory
Group: Development/Libraries
Requires: %{name} = %{version}-%{release}

%description devel
Development files for kmediafactory


%prep
%setup -q

%build
%configure --disable-rpath
make %{?_smp_mflags}


%install
rm -rf $RPM_BUILD_ROOT
make install DESTDIR=$RPM_BUILD_ROOT


%clean
rm -rf $RPM_BUILD_ROOT

%post -p /sbin/ldconfig

%postun -p /sbin/ldconfig

%files
%defattr(-,root,root,-)
%doc
%{_bindir}/kmediafactory
%{_libdir}/kde3/kmediafactory_dvimport.la
%{_libdir}/kde3/kmediafactory_dvimport.so
%{_libdir}/kde3/kmediafactory_output.la
%{_libdir}/kde3/kmediafactory_output.so
%{_libdir}/kde3/kmediafactory_template.la
%{_libdir}/kde3/kmediafactory_template.so
%{_libdir}/kde3/kmediafactory_video.la
%{_libdir}/kde3/kmediafactory_video.so
%{_libdir}/kde3/plugins/designer/kmfwidgets.la
%{_libdir}/kde3/plugins/designer/kmfwidgets.so
%{_libdir}/libkmediafactoryinterfaces.la
%{_libdir}/libkmediafactoryinterfaces.so.0
%{_libdir}/libkmediafactoryinterfaces.so.0.0.0
%{_datadir}/applnk/Utilities/kmediafactory.desktop
%{_datadir}/applnk/Utilities/kmediafactory.kcfg
%{_datadir}/apps/kmediafactory/kmediafactoryui.rc
%{_datadir}/apps/kmediafactory_dvimport/kmediafactory_dvimportui.rc
%{_datadir}/apps/kmediafactory_output/kmediafactory_outputui.rc
%{_datadir}/apps/kmediafactory_template/kmediafactory_templateui.rc
%{_datadir}/apps/kmediafactory_template/preview_1.kmft
%{_datadir}/apps/kmediafactory_template/preview_3.kmft
%{_datadir}/apps/kmediafactory_template/preview_6.kmft
%{_datadir}/apps/kmediafactory_template/simple.kmft
%{_datadir}/apps/kmediafactory_template/templateplugin.kcfg
%{_datadir}/apps/kmediafactory_template/travel.kmft
%{_datadir}/apps/kmediafactory_video/kmediafactory_videoui.rc
%{_datadir}/apps/kmfwidgets/pics/kmfimageview.png
%{_datadir}/apps/kmfwidgets/pics/kmflanguagecombobox.png
%{_datadir}/apps/kmfwidgets/pics/kmflanguagelistbox.png
%{_datadir}/doc/HTML/en/kmediafactory/common
%{_datadir}/doc/HTML/en/kmediafactory/index.cache.bz2
%{_datadir}/doc/HTML/en/kmediafactory/index.docbook
%{_datadir}/doc/HTML/en/kmediafactory/index.docbook.orig
%{_datadir}/icons/crystalsvg/*/apps/kmediafactory.png
%{_datadir}/icons/crystalsvg/*/mimetypes/kmediafactory_project.png
%{_datadir}/icons/crystalsvg/32x32/actions/add_video.png
%{_datadir}/icons/crystalsvg/scalable/apps/kmediafactory.svgz
%{_datadir}/icons/crystalsvg/scalable/mimetypes/kmediafactory_project.svgz
%{_datadir}/mimelnk/application/x-kmediafactory.desktop
%{_datadir}/services/dvimportplugin.kcfg
%{_datadir}/services/kmediafactory_dvimport.desktop
%{_datadir}/services/kmediafactory_output.desktop
%{_datadir}/services/kmediafactory_template.desktop
%{_datadir}/services/kmediafactory_video.desktop
%{_datadir}/services/videoplugin.kcfg
%{_datadir}/servicetypes/kmediafactoryplugin.desktop


%files devel
%defattr(-,root,root,-)
%{_includedir}/kmediafactory/kmfimageview.h
%{_includedir}/kmediafactory/kmflanguagewidgets.h
%{_includedir}/kmediafactory/kmfobject.h
%{_includedir}/kmediafactory/plugin.h
%{_includedir}/kmediafactory/plugin/videoobject.h
%{_includedir}/kmediafactory/projectinterface.h
%{_includedir}/kmediafactory/qdvdinfo.h
%{_includedir}/kmediafactory/uiinterface.h
%{_includedir}/koStore.h
%{_includedir}/koStoreDevice.h
%{_libdir}/libkmediafactoryinterfaces.so

%changelog
* Tue Aug 16 2005 Sebastian Vahl <svahl@web.de> - 0.4.0-1
- Initial Release
