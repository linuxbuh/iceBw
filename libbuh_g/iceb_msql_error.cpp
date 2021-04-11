/* $Id: iceb_msql_error.c,v 1.12 2013/08/13 06:10:12 sasa Exp $ */
/*11.11.2008	06.03.2001	Белых А.И.	iceb_msql_error.c
Меню вывода сообщения об ощибке базы данных
Две подпрограммы одна выводит на экран другая в файл
*/
#include        "iceb_libbuh.h"

void	iceb_msql_error(SQL_baza *bd,const char *soobsh,const char *zapros,GtkWidget *wpredok)
{
char	strsql[1024];
int		i=0;
const short     DLINNA=100;

memset(strsql,'\0',sizeof(strsql));
//sprintf(strsql,"%s %d",soobsh,sql_nerror(bd));

iceb_u_str SP;

if(soobsh[0] != '\0')
  SP.plus_ps(soobsh);

memset(strsql,'\0',sizeof(strsql));
sprintf(strsql,"%d %s",sql_nerror(bd),sql_error(bd));
SP.plus_ps(strsql);

sprintf(strsql,"%.*s",DLINNA,zapros);
SP.plus(strsql);

for(i=1; i< 20;i++)
 {
  if((int)strlen(zapros) > DLINNA*i)
   {
    sprintf(strsql,"%.*s",DLINNA,&zapros[DLINNA*i]);
    SP.ps_plus(strsql);
   }
  else
   break;
 }

iceb_menu_soob(&SP,wpredok);

}
