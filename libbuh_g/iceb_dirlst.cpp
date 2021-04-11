/*$Id: dirlst.c,v 5.16 2014/04/30 06:12:32 sasa Exp $*/
/*23.05.2018    15.01.1993      Белых А.И.  iceb_dirlst.c
Подпрограмма работы с файлами листингов
распечатка файлов на АЦПУ
редактирование и просмотр в редакторе
удаление
*/
#include	"iceb_libbuh.h"

void iceb_dirlst(const char *tfima,GtkWidget *wpredok) //Трафарет имени файла
{
class iceb_u_spisok mfa; /*Массив с именами файлов*/
class spis_oth oth;
char bros[1024];
if(iceb_dirfajl(tfima,2,&oth.spis_imaf,wpredok) == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного файла !"),wpredok);
  return;
 }

for(int nom=0; nom < oth.spis_imaf.kolih(); nom++)
 {
  sprintf(bros,"%s %d",gettext("Файл"),nom+1);
  oth.spis_naim.plus(bros);
  oth.spis_ms.plus(oth.spis_imaf.ravno(nom));
 }   
iceb_rabfil(&oth,wpredok);
}
