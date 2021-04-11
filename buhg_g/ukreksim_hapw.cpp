/*$Id: ukreksim_hapw.c,v 1.8 2013/09/26 09:46:56 sasa Exp $*/
/*13.07.2015	25.01.2006	Белых А.И.	ukreksim_hapw.c
Начало формирования распечатки и файла для записи на дискету для Укрэксимбанка
*/
#include <errno.h>
#include "buhg_g.h"


int ukreksim_hapw(char *imafr,char *imaftmp,FILE **fftmp,FILE **ffr,GtkWidget *wpredok)
{

time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

sprintf(imaftmp,"ukreksim%d.tmp",getpid());
if((*fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"ukreksim%d.lst",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return(1);
 }

fprintf(*ffr,"%s\n\n",iceb_get_pnk("00",0,wpredok));

fprintf(*ffr,"\
          Відомість для зарахування на карт-рахунки N_____\n\
                від %d.%d.%dр.\n\n",bf->tm_mday,bf->tm_mon+1,bf->tm_year+1900);

fprintf(*ffr,"\
-----------------------------------------------------------------------\n");
fprintf(*ffr,"\
 Н/п|Таб.номер| Карт-счёт |  Сумма    | Прізвище,і'мя, по батькові\n");
fprintf(*ffr,"\
-----------------------------------------------------------------------\n");
return(0);
}
