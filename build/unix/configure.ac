AC_INIT([pedalevite], [r1], [http://pedalevite.fr], [pedalevite], [http://pedalevite.fr])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects no-define])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_PROG_CC

AC_CANONICAL_HOST

# Default pv_version is 2
AS_IF(
    [test "x$pv_version" != "x1"],
    [pv_version=2]
)
AC_DEFINE_UNQUOTED([PV_VERSION], [($pv_version)], [Pedale Vite version. 1 or 2.])

AC_ARG_ENABLE([debug], AS_HELP_STRING([--enable-debug], [Compilation options required for debugging. [default=no]]))



X86="false"
PPC="false"
ARM="false"
WIN="false"
UNX="false"
MAC="false"

AS_CASE(
    [$host_cpu],
    [i?86],     [BITS="32" X86="true"],
    [x86_64],   [BITS="64" X86="true"],
    [powerpc*], [          PPC="true"],
    [arm*],     [BITS="32" ARM="true"],
    [aarch64*], [BITS="64" ARM="true"]
)

AS_CASE(
    [$host_os],
    [cygwin*|mingw*],                [WIN="true"],
    [darwin*],                       [MAC="true"],
    [*linux*|gnu*|dragonfly*|*bsd*], [UNX="true"]
)



AC_LANG_PUSH([C++])
AS_IF([test "x$CXXSTD" = "x"], AX_CHECK_COMPILE_FLAG([-std=c++17], [CXXSTD="c++17"]))
AS_IF([test "x$CXXSTD" = "x"], AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXSTD="c++11"]))
AS_IF([test "x$CXXSTD" = "x"], AC_MSG_ERROR([Minimum requirement: C++11]))
AX_CHECK_COMPILE_FLAG([-Wunused-private-field]        , [CXXFLAGS="$CXXFLAGS -Wno-unused-private-field"]        , , [-Werror])
AX_CHECK_COMPILE_FLAG([-Wunused-command-line-argument], [CXXFLAGS="$CXXFLAGS -Wno-unused-command-line-argument"], , [-Werror])
AC_LANG_POP([C++])

AX_PTHREAD
AS_IF(
    [test "x$UNX" = "xtrue"],
    [
        AC_CHECK_LIB([jack], [jack_client_open])
        AC_CHECK_LIB([asound], [snd_pcm_open])
        LIBS="$LIBS `pkg-config --cflags --libs jack` -lasound"
        AS_IF(
            [test "x$ARM" = "xtrue"],
            [
                AC_CHECK_LIB([wiringPi], [wiringPiSetupPhys])
                LIBS="$LIBS -lwiringPi -lbcm_host"
                LDFLAGS="$LDFLAGS -L/opt/vc/lib"
                CXXFLAGS="$CXXFLAGS -I/opt/vc/include"
            ]
        )
    ]
)

# It seems that -latomic is needed only for some versions of GCC < 5.3
AX_CHECK_LINK_FLAG([-latomic], [LIBS="$LIBS -latomic"])

AS_IF(
    [test "x$enable_debug" = "xyes"],
    [DEBUGCFLAGS="-O0 -g3 -ggdb"],
    [DEBUGCFLAGS="-O3 -g3 -DNDEBUG"]
)

AS_IF(
    [test "x$WIN" = "xtrue"],
    [
        LIBS="$LIBS -lws2_32 -lgdi32 -lcomdlg32 -lole32 -loleaut32 -luuid"
        AS_IF(
            [test "x$BITS" = "x32"],
            [
                PLUGINLDFLAGS="-Wl,--kill-at"
                STACKREALIGN="-mstackrealign"
            ]
        )
    ]
)

AS_IF(
    [test "x$X86" = "xtrue"],
    [
        MFLAGS="$MFLAGS -mfpmath=sse -msse2 -Wno-ignored-attributes"
        # We need this to use CMPXCHG16B for 2x64-bit CAS (compare and swap)
        AS_IF([test "x$BITS" = "x64"], [MFLAGS="$MFLAGS -mcx16"])
    ]
)


AS_IF(
    [test "x$ARM" = "xtrue"],
    [
        rpi_version=`cat /sys/firmware/devicetree/base/model | awk '{print $3}'`
        AS_IF([test "x$rpi_version" = "x"], AC_MSG_ERROR([[Not on a Raspberry Pi, or unknown model.]]))

        AC_DEFINE_UNQUOTED([PV_RPI_VER_MAJOR], [$rpi_version], [Raspberry Pi version number (3 or 4).])
        MFLAGS="$MFLAGS -mfloat-abi=hard -ftree-vectorize"
        AS_IF(
            [test "x$rpi_version" = "x4"],
            [
                MFLAGS="$MFLAGS -mfpu=neon-fp-armv8 -mtune=cortex-a72"
                # Clang doesn't support the +crc+simd arch extensions
		AX_CHECK_COMPILE_FLAG(
                    [-march=armv8-a+crc+simd],
                    [CXXFLAGS="$CXXFLAGS -march=armv8-a+crc+simd"],
                    [CXXFLAGS="$CXXFLAGS -march=armv8-a"]
                )
            ],
            [MFLAGS="$MFLAGS -mfpu=neon -mtune=cortex-a53"]
        )
        # GCC 7 emits some warnings about ABI changes when using std::vector
        AX_CHECK_COMPILE_FLAG([-Wpsabi], [MFLAGS="$MFLAGS -Wno-psabi"], , [-Werror])
    ]
)



AC_SUBST([CXXSTD])
AC_SUBST([LDFLAGS])
AC_SUBST([MFLAGS])
AC_SUBST([DEBUGCFLAGS])
AC_SUBST([PLUGINLDFLAGS])
AC_SUBST([STACKREALIGN])

AM_CONDITIONAL([X86], [test "x$X86" = "xtrue"])
AM_CONDITIONAL([ARM], [test "x$ARM" = "xtrue"])
AM_CONDITIONAL([UNX], [test "x$UNX" = "xtrue"])
AM_CONDITIONAL([WIN], [test "x$WIN" = "xtrue"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
