AC_INIT([petratc], [1.0])
AC_CONFIG_SRCDIR([petratc.c])
AC_PROG_CC

#what if i have pdcurses or just curses and not ncurses idk
#this will be useful only when i decide to port it to widnows 
CURSES_LIB=""
AC_CHECK_LIB([ncurses], [initscr], [CURSES_LIB="-lncurses"])
AS_IF([test "x$CURSES_LIB" = "x"],
  [AC_CHECK_LIB([curses], [initscr], [CURSES_LIB="-lcurses"])])
AS_IF([test "x$CURSES_LIB" = "x"],
  [AC_CHECK_LIB([pdcurses], [initscr], [CURSES_LIB="-lpdcurses"])])
AS_IF([test "x$CURSES_LIB" = "x"],
  [AC_MSG_ERROR([no curses library found (tried ncursesw, ncurses, curses, pdcurses)])])

#in mac having -ltinfo in your makefile gives you a linker error
#whereas in linux you need it
AC_CHECK_LIB([tinfo], [setupterm], [CURSES_LIB="$CURSES_LIB -ltinfo"])

AC_SUBST([CURSES_LIB])
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
