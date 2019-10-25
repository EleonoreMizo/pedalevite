AC_INIT([pedalevite], [r1], [http://pedalevite.fr], [pedalevite], [http://pedalevite.fr])

: ${CXXFLAGS=""}

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects no-define])
AM_SILENT_RULES([yes])

AC_PROG_CXX([g++])
AC_PROG_CC([g++])

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
