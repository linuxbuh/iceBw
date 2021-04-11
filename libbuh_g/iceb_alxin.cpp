/*$Id: iceb_alxin.c,v 1.4 2014/06/30 06:35:56 sasa Exp $*/
/*13.05.2014	07.09.2010	Белых А.И.	iceb_alxin.c
Загрузка файла alx в базу
*/
#include <errno.h>
#include "iceb_libbuh.h"

void iceb_alxin(const char *imaf,GtkWidget *wpredok)
{
class iceb_u_str stroka("");
gchar *strsql=NULL;
FILE *ff;
class iceb_u_str imafz;
class iceb_u_str strin;
int nom_op=iceb_getuid(wpredok);
time_t vrem=time(NULL);
int nomer_str=0;

int kolb=iceb_u_pole2(imaf,G_DIR_SEPARATOR);
if(kolb == 0)
 imafz.plus(imaf);
else
 iceb_u_polen(imaf,&imafz,kolb,G_DIR_SEPARATOR);  
if((ff=fopen(imaf,"r")) == NULL)
  {
   iceb_er_op_fil(imaf,"",errno,wpredok);
   return;
  }

class iceb_lock_tables kkk("LOCK TABLES Alx WRITE");

//sprintf(strsql,"delete from Alx where fil='%s'",imafz.ravno());
strsql=g_strdup_printf("delete from Alx where fil='%s'",imafz.ravno());
if(iceb_sql_zapis(strsql,1,0,wpredok) != 0)
 {
  g_free(strsql);
  return;
 }
g_free(strsql);

while(iceb_u_fgets(&stroka,ff) == 0)
 {
  strsql=g_strdup_printf("insert into Alx values('%s',%d,%d,%ld,'%s')",imafz.ravno(),++nomer_str,nom_op,vrem,stroka.ravno_filtr());

  iceb_sql_zapis(strsql,1,0,wpredok);
  g_free(strsql);
 }
fclose(ff);

}



