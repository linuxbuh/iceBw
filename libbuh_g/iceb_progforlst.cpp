/*$Id: iceb_progforlst.c,v 1.6 2013/05/17 14:58:36 sasa Exp $*/
/*02.12.2014	20.03.2010	Белых А.И.	iceb_t_progforlst.c
Чтение и выбор настройки для выполнения программы
Возвращаем 0- программа выполнена
           1- нет
*/
#include <errno.h>
#include "iceb_libbuh.h"


int iceb_progforlst(const char *imaf,GtkWidget *wpredok)
{
class iceb_u_spisok str_menu;
class iceb_u_spisok prog;

class iceb_u_str strok("");
class iceb_u_str bros("");

class iceb_u_str imaf_nast;

imaf_nast.plus(g_get_home_dir());
imaf_nast.plus(G_DIR_SEPARATOR_S,".iceB");
imaf_nast.plus(G_DIR_SEPARATOR_S,"progforlst.alx");

FILE *ff;

if((ff = fopen(imaf_nast.ravno(),"r")) == NULL)
 {
  iceb_er_op_fil(imaf_nast.ravno(),"",errno,wpredok);
  return(1);
 }

while(iceb_u_fgets(&strok,ff) == 0)
 {
  if(strok.ravno()[0] == '#')
   continue;
  if(iceb_u_polen(strok.ravno(),&bros,1,'|') != 0)
   continue;
  if(bros.ravno()[0] == '\0')
   continue;

  str_menu.plus(bros.ravno());

  iceb_u_polen(strok.ravno(),&bros,2,'|');
  prog.plus(bros.ravno());
 }
fclose(ff);

if(str_menu.kolih() == 0)
 {
  iceb_menu_soob(gettext("Не найдено настроек !"),wpredok);
  return(1);
 }

int voz=0;
if(str_menu.kolih() > 1)
 {
  class iceb_u_spisok naim_kol;
  naim_kol.plus(gettext("Наименование операции"));
  iceb_u_str naim_menu;
  naim_menu.plus(gettext("Выбор базы данных"));
  if((voz=iceb_l_spisokm(&str_menu,&prog,&naim_kol,&naim_menu,0,wpredok)) < 0)
    return(1);

 }

//sprintf(bros,"%s %s",prog.ravno(voz),imaf);
bros.new_plus(prog.ravno(voz));
bros.plus(" ",imaf);

int kodv=system(bros.ravno());

if(kodv != 0)
 {
  char str[1024];
  sprintf(str,"%s %d!\n%s",gettext("Ошибка выполнения программы"),kodv,bros.ravno());
  iceb_menu_soob(str,wpredok);
 } 
return(kodv);
}



