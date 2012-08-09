AC_DEFUN([RCHIP_VERBOSITY],
[
	AC_ARG_ENABLE(debug,AS_HELP_STRING(--enable-debug={0-5},prints verbose output for debugging))
	case "x$enable_debug" in
		x)	AC_DEFINE(VERBOSE,3,"Output Verbosity Level");;
		x0)	AC_DEFINE(VERBOSE,0,"Output Verbosity Level");;
		x1)	AC_DEFINE(VERBOSE,1,"Output Verbosity Level");;
		x2)	AC_DEFINE(VERBOSE,2,"Output Verbosity Level");;
		x3)	AC_DEFINE(VERBOSE,3,"Output Verbosity Level");;
		x4)	AC_DEFINE(VERBOSE,4,"Output Verbosity Level");;
		x5)	AC_DEFINE(VERBOSE,5,"Output Verbosity Level");;
		*)	AC_MSG_ERROR([unexpected value $enable_debug for --enable-debug configure option]);;
	esac
])

