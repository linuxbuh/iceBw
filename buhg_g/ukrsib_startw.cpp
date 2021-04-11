/*$Id: ukrsib_startw.c,v 1.9 2013/09/26 09:46:56 sasa Exp $*/
/*13.07.20015	29.01.2006	Белых А.И. 	ukrsib_startw.c
Начало формирования данных для зачисления на карт-счета Укрсиббанка
*/

#include        <errno.h>
#include    "buhg_g.h"



int ukrsib_startw(char *imafr,char *imaftmp,
class iceb_u_str *operatorr,
FILE **fftmp,FILE **ffr,GtkWidget *wpredok)
{

if(iceb_menu_vvod1(gettext("Введите фамилию оператора в банке"),operatorr,20,"",wpredok) != 0)
 return(1);


time_t vrem;
struct tm *bf;
time(&vrem);
bf=localtime(&vrem);

sprintf(imaftmp,"ukrsib%d.tmp",getpid());
if((*fftmp = fopen(imaftmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaftmp,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"ukrsib%d.lst",getpid());
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
