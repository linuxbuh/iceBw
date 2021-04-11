/*$Id: iceb_alxzag.c,v 1.7 2013/08/13 06:10:09 sasa Exp $*/
/*05.12.2014	12.09.2010	Белых А.И.	iceb_alxzag.c
Загрузка настроечных файлов в базу
*/
#include <errno.h>
#include "iceb_libbuh.h"


int iceb_alxzag(const char *put_alx,
int metka, /*0-загрузить если в базе нет 1-всегда грузить*/
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok) 
{
char strsql[1024];
class iceb_u_str allput(put_alx);

sprintf(strsql,"%s\n",gettext("Загрузка настроечных файлов в базу"));
iceb_printw(strsql,buffer,view);

GDir *dirp;
if((dirp=g_dir_open(allput.ravno(),0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(allput.ravno());
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(repl.ravno(),wpredok);
  
  return(1);
 }
class iceb_u_str imafil;
class iceb_u_str rashir;
const char *imafil_read;

while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(iceb_u_polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(iceb_u_SRAV("alx",rashir.ravno(),0) != 0)
   continue;

  /*Проверяем есть ли там этот файл*/
  if(metka == 0)
   {
    sprintf(strsql,"select fil from Alx where fil='%s' limit 1",imafil_read);
    if(iceb_sql_readkey(strsql,wpredok) > 0)
     continue;
   }
  if(iceb_u_SRAV("icebstart",imafil_read,1) == 0)
   continue;
  
  imafil.new_plus(allput.ravno());
  imafil.plus(G_DIR_SEPARATOR_S,imafil_read);

  sprintf(strsql,"%s-%s\n",gettext("Загрузка файла"),imafil.ravno());
  iceb_printw(strsql,buffer,view);

  iceb_alxin(imafil.ravno(),wpredok);

 }
g_dir_close(dirp);
return(0);
}
