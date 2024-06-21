#serial 1

dnl Macro that provides the --enable-target option
AC_DEFUN([ENABLE_SB_TARGET_OPT],
[
AC_ARG_ENABLE([target],
  [AS_HELP_STRING([--enable-target=@<:@custom|bare|versatile@:>@], [select target])],
  [case $enableval in
     custom|bare|versatile) : ;;
     *) AC_MSG_ERROR([--enable-target: unknown target: "$enableval"]) ;;
  esac],
  [enable_target=versatile]
)
])dnl

