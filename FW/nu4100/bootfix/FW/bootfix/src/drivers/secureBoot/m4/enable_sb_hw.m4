#serial 1

dnl Macro that provides the --enable-sb* options
AC_DEFUN([ENABLE_SB_HW],
[
AC_ARG_ENABLE([sbsw],
  [AS_HELP_STRING([--enable-sbsw], [Build SW only Secure Boot (default)])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbsw: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbsw=yes]
)
AC_ARG_ENABLE([sbsimple],
  [AS_HELP_STRING([--enable-sbsimple], [Build SB_SIMPLE SW Secure Boot])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbsimple: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbsimple=no]
)
AC_ARG_ENABLE([sbsm],
  [AS_HELP_STRING([--enable-sbsm], [Build Secure Boot with EIP-130 support])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbsm: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbsm=no]
)
AC_ARG_ENABLE([sbcm],
  [AS_HELP_STRING([--enable-sbcm], [Build Secure Boot with EIP-123 support])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbcm: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbcm=no]
)
AC_ARG_ENABLE([sbcmpk],
  [AS_HELP_STRING([--enable-sbcmpk], [Build Secure Boot with EIP-123+28/150 support])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbcmpk: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbcmpk=no]
)
AC_ARG_ENABLE([sbpe],
  [AS_HELP_STRING([--enable-sbpe], [Build Secure Boot with EIP-93 support])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbpe: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbpe=no]
)
AC_ARG_ENABLE([sbpepk],
  [AS_HELP_STRING([--enable-sbpepk], [Build Secure Boot with EIP-93+28/150 support])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbpepk: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbpepk=no]
)
AC_ARG_ENABLE([sbpk],
  [AS_HELP_STRING([--enable-sbpk], [Build Secure Boot with EIP-28/150 support])],
  [case $enableval in
     yes|no) : ;;
     *) AC_MSG_ERROR([--enable-sbpk: unrecognized ARG: expected yes or no]) ;;
  esac],
  [enable_sbpk=no]
)
])dnl

