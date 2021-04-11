/*$Id: iceb_system.c,v 1.2 2013/08/13 06:10:13 sasa Exp $*/
/*31.08.2012	31.08.2012	Белых А.И.	iceb_system.c
Запуск программы, чтение и вывод на экран её вывод
*/
#include "iceb_libbuh.h"

int iceb_system(const char *komanda,GtkTextBuffer *buffer,GtkWidget *view)
{
char stroka[1024];
FILE *ff;

if((ff=popen(komanda,"r")) == NULL)
 {
  sprintf(stroka,"%s-%s:%s\n",__FUNCTION__,gettext("Ошибка открытия канала"),komanda);
  iceb_printw(stroka,buffer,view);
  return(1);
 }

while(fgets(stroka,sizeof(stroka),ff) != NULL)
 {
  iceb_printw(stroka,buffer,view);
 }

int voz=pclose(ff);

//if(WIFEXITED(voz) != TRUE)
if(voz != 0)
 {
  
//  printw("\n%s-%s:%s\n",__FUNCTION__,gettext("Ошибка при выполнении команды"),komanda);
//  OSTANOV();
 }  
return(voz);

}
