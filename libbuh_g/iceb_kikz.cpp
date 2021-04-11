/*$Id: iceb_kikz.c,v 1.6 2013/05/17 14:58:34 sasa Exp $*/
/*08.01.2010	08.05.2004	Белых А.И.	iceb_kikz.c
Формирование символьной строки кто и когда сделал запись
*/
#include        "iceb_libbuh.h"


const char *iceb_kikz(char *kto,char *vremzap,GtkWidget *wpredok)
{
class iceb_u_str fio("");
char strsql[112];
SQL_str row;
class SQLCURSOR cur;

if(kto != NULL)
 {
  sprintf(strsql,"select fio from icebuser where un=%d",atoi(kto));
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   fio.new_plus(row[0]);
 }
else 
 fio.new_plus("NULL");
 
static char stroka[512];

memset(stroka,'\0',sizeof(stroka));
strcpy(stroka,gettext("Записал"));
strcat(stroka,":");
strcat(stroka,fio.ravno());
strcat(stroka," ");
strcat(stroka,gettext("Дата и время записи"));
strcat(stroka,":");
strcat(stroka,iceb_u_vremzap(vremzap));

return(stroka);

}
/*******************************/

const char *iceb_kikz(int kto,time_t vremzap,GtkWidget *wpredok)
{
char kto1[64];
char vremzap1[64];
sprintf(kto1,"%d",kto);
sprintf(vremzap1,"%ld",vremzap);


return(iceb_kikz(kto1,vremzap1,wpredok));

}
