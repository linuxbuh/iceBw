/*$Id: admin_alterw.c,v 1.2 2013/08/13 06:09:30 sasa Exp $*/
/*19.02.2012	19.02.2012	Белых А.И.	admin_alter.c
Разметка базы данных для бухгалтерской системы iceB
*/

#include "buhg_g.h"

void admin_alter_r(const char *imabaz,const char *fromnsi,const char *fromdoc,GtkWidget *wpredok);
int admin_alter_m(class iceb_u_str *imabaz,class iceb_u_str *fromnsi,class iceb_u_str *fromdoc,GtkWidget *wpredok);

void admin_alter(GtkWidget *wpredok)
{
class iceb_u_str imabaz("");
class iceb_u_str fromnsi("");
class iceb_u_str fromdoc("");

if(admin_alter_m(&imabaz,&fromnsi,&fromdoc,wpredok) != 0)
 return;
 
admin_alter_r(imabaz.ravno(),fromnsi.ravno(),fromdoc.ravno(),wpredok);


}
