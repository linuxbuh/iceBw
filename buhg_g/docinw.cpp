/*$Id: docinw.c,v 1.8 2014/07/31 07:08:24 sasa Exp $*/
/*10.07.2015	17.10.2010	Белых А.И.	Белых А.И.	docinw.c
Загрузка в таблицу документации
*/
#include <errno.h>
#include "buhg_g.h"

void docinw(const char *put_na_doc,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
int nomer_str=0;
class iceb_u_str stroka("");
FILE *ff;

GDir *dirp;
if((dirp=g_dir_open(put_na_doc,0,NULL)) == NULL)
 {
  perror(__FUNCTION__);
  class iceb_u_str repl;
  repl.plus(__FUNCTION__);
  repl.ps_plus(gettext("Не могу открыть каталог !"));
  repl.ps_plus(put_na_doc);
  repl.ps_plus(strerror(errno));
  iceb_menu_soob(repl.ravno(),wpredok);
  
 }

if(iceb_sql_zapis("delete from Doc",1,0,wpredok) != 0)
   return;

class iceb_u_str imafil;
class iceb_u_str rashir;
const char *imafil_read;
class iceb_u_str strin("");
int nom=0;
while((imafil_read=g_dir_read_name(dirp)) != NULL)
 {
  if(iceb_u_polen(imafil_read,&rashir,2,'.') != 0)
   continue;

  if(iceb_u_SRAV("txt",rashir.ravno(),0) != 0 && iceb_u_SRAV("TXT",rashir.ravno(),0) != 0)
   continue;

  
  imafil.new_plus(put_na_doc);
  imafil.plus(G_DIR_SEPARATOR_S,imafil_read);

//  sprintf(strsql,"%s-%s\n",gettext("Загрузка файла"),imafil.ravno());
//  iceb_printw(strsql,buffer,view);
  nom++;  
  iceb_printw(".",buffer,view);
  if(nom > 80)
   {
    iceb_printw("\n",buffer,view);
    nom=0;
   }   
  if((ff=fopen(imafil.ravno(),"r")) == NULL)
   {
    iceb_er_op_fil(imafil.ravno(),"",errno,wpredok);
    return;
   }

  while(iceb_u_fgets(&stroka,ff) == 0)
   {

    gchar *strsql=g_strdup_printf("insert into Doc values('%s',%d,'%s')",imafil_read,++nomer_str,stroka.ravno_filtr());

    iceb_sql_zapis(strsql,1,0,wpredok);

    g_free(strsql);
   }
  fclose(ff);

 }
iceb_printw("\n",buffer,view);

g_dir_close(dirp);
}
