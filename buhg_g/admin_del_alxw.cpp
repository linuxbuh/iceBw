/*$Id: admin_del_alxw.c,v 1.5 2013/05/17 14:58:18 sasa Exp $*/
/*10.07.2015	12.03.2010	Белых А.И.	admin_del_alx.c
Удаление настроечных файлов при удалении базы данных
*/
#include        <errno.h>
#include "buhg_g.h"


int admin_del_alx(const char *put_na_kat,GtkWidget *wpredok)
{


GDir *dirp;
if((dirp=g_dir_open(put_na_kat,0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(put_na_kat);
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(&repl,wpredok);
  
  return(1);
 }

const char *imafil_read;
class iceb_u_str imafil_del;

while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  imafil_del.new_plus(put_na_kat);
  imafil_del.plus(G_DIR_SEPARATOR_S,imafil_read);
  
  unlink(imafil_del.ravno());  

 }
g_dir_close(dirp);

if(rmdir(put_na_kat) != 0)
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу удалить каталог!"));
  repl.ps_plus(put_na_kat);
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(&repl,wpredok);
  
  return(1);
 }

return(0);
}

