AC_DEFUN([RCHIP_CHECK_MYSQL],
[
	AC_ARG_ENABLE(mysql,AC_HELP_STRING([--disable-mysql], 
                [Do not build with mysql support]),
                [
                        if test "x$enableval" = "xno"; then
                                enable_mysql=no
                        elif test "x$enableval" = "xyes"; then
                                enable_mysql=yes
                        fi
                ], enable_mysql=yes
	)
	AC_ARG_WITH([mysql-server],AC_HELP_STRING([--with-mysql-server=],[server MySQL is installed on]),MYSQLS="$withval",MYSQLS="192.168.1.3")
	AC_ARG_WITH([mysql-username],AC_HELP_STRING([--with-mysql-username=],[MySQL Server Username]),MYSQLU="$withval",MYSQLU="nTesla")
	AC_ARG_WITH([mysql-password],AC_HELP_STRING([--with-mysql-password=],[MySQL Server Password]),MYSQLP="$withval",MYSQLP="deathray")
	AC_ARG_WITH([mysql-database],AC_HELP_STRING([--with-mysql-database=],[MySQL Server Database]),MYSQLD="$withval",MYSQLD="madSci")

	MYSQLCONFIG="/usr/bin/mysql_config"
	if test "x$enable_mysql" = "xno"; then
       		AC_DEFINE(_NOSQL,,"Turn OFF SQL Functionality")
	else
       		MYSQL_CFLAGS=`$MYSQLCONFIG --cflags`
       		MYSQL_INCLUDES=`$MYSQLCONFIG --include`
       		MYSQL_LIBS=`$MYSQLCONFIG --libs_r`
       		AC_DEFINE(_SQL,,"Turn ON SQL Functionality")
	fi
	AC_SUBST(MYSQL_CFLAGS)
	AC_SUBST(MYSQL_INCLUDES)
	AC_SUBST(MYSQL_LIBS)

	AC_MSG_CHECKING(for MySQL CFLAGS)
	AC_MSG_RESULT($MYSQL_CFLAGS)
	AC_MSG_CHECKING(for MySQL includes)
	AC_MSG_RESULT($MYSQL_INCLUDES)
	AC_MSG_CHECKING(for MySQL libraries)
	AC_MSG_RESULT($MYSQL_LIBS)
])

