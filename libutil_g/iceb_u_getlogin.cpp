/*$Id: iceb_u_getlogin.c,v 1.3 2013/05/17 14:58:42 sasa Exp $*/
/*29.09.2010	05.09.2010	Белых А.И.	iceb_u_getlogin.c
Получение догина оператора
*/
#include <pwd.h>
#include <unistd.h>
#include "iceb_util.h"
const char *iceb_u_getlogin()
{
static class iceb_u_str login("");
struct passwd *ktor=getpwuid(getuid());
login.new_plus(ktor->pw_name);
return(login.ravno());

}