AC_INIT([pedalevite], [r1], [http://pedalevite.fr], [pedalevite], [http://pedalevite.fr])
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects no-define])
AM_SILENT_RULES([yes])

AC_PROG_CXX
AC_PROG_CC([$CXX])

AC_CANONICAL_HOST

AS_IF(
        [test "x$pv_version" != "x2"],
        [pv_version=1]
)
AC_DEFINE_UNQUOTED([PV_VERSION], [($pv_version)], [Pedale Vite version. 1 or 2.])

X86="false"
PPC="false"
ARM="false"

AS_CASE(
        [$host_cpu],
        [i?86],   [BITS="32" X86="true"],
        [x86_64], [BITS="64" X86="true"],
        [powerpc*], [PPC="true"],
        [arm*], [BITS="32" ARM="true"],
	[aarch64*], [BITS="64" ARM="true"]
)

AS_CASE(
        [$host_os],
        [cygwin*|mingw*],
        [
         AS_IF(
               [test "x$BITS" = "x32"],
               [
                AC_SUBST([PLUGINLDFLAGS], ["-Wl,--kill-at"])
               ]
         )
        ]
)



AC_LANG_PUSH([C++])
AS_IF([test "x$CXXSTD" = "x"], AX_CHECK_COMPILE_FLAG([-std=c++17], [CXXSTD="c++17"]))
AS_IF([test "x$CXXSTD" = "x"], AX_CHECK_COMPILE_FLAG([-std=c++11], [CXXSTD="c++11"]))
AS_IF([test "x$CXXSTD" = "x"], AC_MSG_ERROR([Minimum requirement: C++11]))
AX_CHECK_COMPILE_FLAG([-Wunused-private-field -Werror], [CXXFLAGS="$CXXFLAGS -Wno-unused-private-field"])
AX_CHECK_COMPILE_FLAG([-Wunused-command-line-argument -Werror], [CXXFLAGS="$CXXFLAGS -Wno-unused-command-line-argument"])
AC_LANG_POP([C++])

# It seems that -latomic is needed only for some versions of GCC < 5.3
AX_CHECK_LINK_FLAG([-latomic], [LDFLAGS="$LDFLAGS -latomic"])

AS_IF(
      [test "x$X86" = "xtrue"],
      [
       MFLAGS="-mfpmath=sse -msse2"

       AS_IF(
             [test "x$BITS" = "x64"],
             [
              MFLAGS="$MFLAGS -mcx16"
             ]
       )
       AC_SUBST([MFLAGS])
      ]
)


AS_IF(
      [test "x$ARM" = "xtrue"],
      [
       rpi_version=`cat /sys/firmware/devicetree/base/model | awk '{print $3}'`
       AC_DEFINE_UNQUOTED([PV_RPI_VER_MAJOR], [$rpi_version], [Raspberry Pi version number (3 or 4).])
       AS_IF(
           [test "x$rpi_version" = "x4"],
           [MFLAGS="-mfloat-abi=hard -mfpu=neon -ftree-vectorize -mtune=cortex-a72 -march=armv8-a+crc+simd"],
           [MFLAGS="-mfloat-abi=hard -mfpu=neon -ftree-vectorize -mtune=cortex-a53"]
       )
       AC_SUBST([MFLAGS])
      ]
)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
