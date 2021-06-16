# cajun only ships headers, so no debuginfo package is needed
BuildArch:      noarch

Summary: A cross-platform C++ header library for JSON
Name: cajun-jsonapi
Version: 2.1.1
Release: 14%{?dist}
URL: https://github.com/cajun-jsonapi/cajun-jsonapi
Source0: https://github.com/cajun-jsonapi/cajun-jsonapi/archive/%{version}.tar.gz
License: BSD

BuildRequires:  gcc-c++
BuildRequires: make
%description
CAJUN is a C++ API for the JSON data interchange format with an emphasis
on an intuitive, concise interface. The library provides JSON types
and operations that mimic standard C++ as closely as possible in concept
and design.

%package devel
Summary: Header files for cajun

%description devel
Header files you can use to develop applications with cajun.

CAJUN is a C++ API for the JSON data interchange format with an emphasis
on an intuitive, concise interface. The library provides JSON types
and operations that mimic standard C++ as closely as possible in concept
and design.

%prep
%setup -q

%build

%install
install -d -m755 $RPM_BUILD_ROOT/%{_includedir}/cajun/json
install -p -m644 include/cajun/json/* $RPM_BUILD_ROOT/%{_includedir}/cajun/json

%check
make %{?_smp_mflags}

%files devel
%doc Readme.md ReleaseNotes.txt
%dir %{_includedir}/cajun
%dir %{_includedir}/cajun/json
%{_includedir}/cajun/json/*

%changelog
* Wed Jun 16 2021 Daniel Pocock <daniel@pocock.pro> - 2.1.1-14
- Updated for next release

* Mon Jun 14 2021 Daniel Pocock <daniel@pocock.pro> - 2.1.0-14
- Updated for next release

* Thu Sep 26 2013 Daniel Pocock <daniel@pocock.pro> - 2.0.3-1
- Initial spec file
