
AC_PREREQ(2.59)
AC_INIT([kerneldb], [0.1], [smprotocol@gmail.com])
AC_CONFIG_SRCDIR([config.h.in])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE([foreign])
AC_CONFIG_MACRO_DIR([m4])

AC_LANG_CPLUSPLUS

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_CPP
AC_PROG_LIBTOOL
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_FUNC_REALLOC
LT_INIT


# c++11
CXXFLAGS="$CXXFLAGS -std=c++11"


KERNELS_PATH="$(eval echo "$sysconfdir")/kerneldb/kernels"
AC_DEFINE_UNQUOTED([KERNELS_PATH], "${KERNELS_PATH}",[Path to kernels])

########################################################################################################################
##  librocksdb
##

AC_ARG_WITH([librocksdb], AC_HELP_STRING([--with-librocksdb=PATH], [path to librocksdb installation]))

if test "$with_librocksdb" == ""; then
	AC_MSG_ERROR([Please specify librocksdb location.  Use --with-librocksdb=PATH])
fi

CPPFLAGS="$CPPFLAGS -I$with_librocksdb/include"
LDFLAGS="$LDFLAGS -L$with_librocksdb -lrocksdb"

AC_CHECK_LIB([rocksdb], [rocksdb_open],[],[AC_MSG_ERROR([Unable to find librocksdb here: $with_librocksdb])])


########################################################################################################################
##  libv8
##

AC_ARG_WITH([libv8], AC_HELP_STRING([--with-libv8=PATH], [path to libv8 installation]))

if test "$with_libv8" == ""; then
	AC_MSG_CHECKING(standard libv8 install locations)
	if test -e /usr/local/include/v8.h; then
		with_libv8="/usr/local"
	elif test -e /opt/local/include/v8.h; then
		with_libv8="/opt/local"
	elif test -e /usr/include/v8.h; then
		with_libv8="/usr"
	else
		AC_MSG_ERROR([Unable to find libv8 in standard install locations.  Use --with-libv8=PATH])
	fi

	AC_MSG_RESULT([$with_libv8])
fi

CPPFLAGS="$CPPFLAGS -I$with_libv8/include"
LDFLAGS="$LDFLAGS -L$with_libv8/lib -lv8"

AC_MSG_CHECKING([for libv8])
AC_COMPILE_IFELSE([AC_LANG_SOURCE[
#include <v8.h>
int main(int argc, char **argv)
{
	v8::Isolate* isolate = v8::Isolate::GetCurrent();
	return 0;
}]], [
		AC_MSG_RESULT([yes])
	], [AC_MSG_ERROR([no])], [AC_MSG_ERROR([unknown])])
	
	
	


# Checks for header files.
AC_CHECK_HEADERS([])


AC_CONFIG_FILES([
					Makefile
					src/Makefile
					src/kerneldb/Makefile
				])



AC_OUTPUT

