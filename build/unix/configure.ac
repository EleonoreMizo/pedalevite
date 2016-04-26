AC_INIT([pedalevite], [r1], [http://ldesoras.free.fr/doc/articles/pedale-vite/pedale-vite.html], [pedalevite], [http://ldesoras.free.fr/doc/articles/pedale-vite/pedale-vite.html])

: ${CXXFLAGS=""}

AM_INIT_AUTOMAKE([foreign no-dist-gzip dist-xz subdir-objects no-define])
AM_SILENT_RULES([yes])

AC_PROG_CXX([g++])
AC_PROG_CC([g++])

AC_CANONICAL_HOST


X86="false"
PPC="false"
ARM="false"

AS_CASE(
        [$host_cpu],
        [i?86],   [BITS="32" X86="true"],
        [x86_64], [BITS="64" X86="true"],
        [powerpc*], [PPC="true"],
        [arm*], [ARM="true"]
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
       MFLAGS="-mfpu=neon"
       AC_SUBST([MFLAGS])
      ]
)


AC_CONFIG_FILES([Makefile])
AC_OUTPUT
