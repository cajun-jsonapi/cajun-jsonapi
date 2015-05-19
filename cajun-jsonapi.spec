
Name:           cajun-jsonapi
Version:        2.0.3
Release:        1%{?dist}
Summary:        A cross-platform C++ header library for JSON
License:        BSD-3-Clause
Group:          Development/Libraries/C and C++
Url:            https://github.com/cajun-jsonapi/cajun-jsonapi
Source0:        %{name}-%{version}.tar.gz
BuildRequires:  gcc-c++
BuildArch:      noarch
BuildRoot:      %{_tmppath}/%{name}-%{version}-build

%description
CAJUN is a C++ API for the JSON data interchange format with an emphasis
on an intuitive, concise interface. The library provides JSON types
and operations that mimic standard C++ as closely as possible in concept
and design.

%package devel
Summary:        Header files for cajun
Group:          Development/Libraries/C and C++

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
install -d -m755 %{buildroot}%{_includedir}/cajun/json
install -p -m644 json/* %{buildroot}%{_includedir}/cajun/json

%check
make %{?_smp_mflags}

%clean
rm -rf %{buildroot}

%files devel
%defattr(-, root, root)
%doc Readme.txt ReleaseNotes.txt
%{_includedir}/cajun/

%changelog
* Thu Sep 26 2013 Daniel Pocock <daniel@pocock.com.au> - 2.0.3-1
- Initial spec file
