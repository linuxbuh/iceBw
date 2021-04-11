/*$Id: iceb_prosf.c,v 1.15 2013/05/17 14:58:36 sasa Exp $*/
/*19.06.2019	24.08.2006	Белых А.И.	iceb_prosf.c
Запуск программы просмотра файла
*/
#include <errno.h>
#include "iceb_libbuh.h"

void iceb_prosf(const char *imaf,GtkWidget *wpredok)
{

/*Проверяем возможность открытия файла*/
FILE *ff;

if((ff=fopen(imaf,"r")) == NULL)
 {
  iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
  return;
 }
fclose(ff);




char prog_pros[512];
memset(prog_pros,'\0',sizeof(prog_pros));

if(iceb_poldan("Просмотрщик файлов для графического интерфейса",prog_pros,"nastsys.alx",wpredok) != 0)
 {
  class iceb_u_str repl;
  repl.plus("iceb_prosf-В файле ");
  repl.plus("nastsys.alx");
  repl.ps_plus("Не найдена настройка \"Просмотрщик файлов для графического интерфейса\" !!!");
  return;
 }
char comanda[1024];
if(strchr(prog_pros,'"') != NULL) /*значит символ есть в строке*/
  sprintf(comanda,"%s %s\"",prog_pros,imaf);
else
  sprintf(comanda,"%s %s",prog_pros,imaf);

iceb_refresh();


class iceb_clock kkk(wpredok);

if(system(comanda) != 0)
 {
  char bros[2048];
  sprintf(bros,"%s-%s %s\n",__FUNCTION__,gettext("Не выполнена команда"),comanda);
  iceb_menu_soob(bros,wpredok);  
 }


}
