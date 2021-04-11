/*$Id: zvb_privatbank_start.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*18.10.2014	17.10.2014	Белых А.И.	zvb_privatbank_startw.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg_g.h"

int zvb_privatbank_v(class iceb_u_str *branch,class iceb_u_str *zpkod,class iceb_u_str *rlkod,GtkWidget *wpredok);


int zvb_privatbank_startw(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
class iceb_u_str *branch,
class iceb_u_str *zpkod,
class iceb_u_str *rlkod,
FILE **ff,  //Файл в электонном виде
FILE **ffr, //Файл распечатки
GtkWidget *wpredok)
{

if(zvb_privatbank_v(branch,zpkod,rlkod,wpredok) != 0)
 return(1);


sprintf(imaf_dbf,"privatbank.dbf");
sprintf(imaf_dbf_tmp,"privatbankdbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf_tmp,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"privatbank%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return(1);
 }

iceb_zagolov(gettext("Ведомость для зачисления на карт-счета"),*ffr,wpredok);

fprintf(*ffr,"\
------------------------------------------------------------------------------------------------------------\n");

fprintf(*ffr,"\
N п/п | Т.н. |      Прізвище Імʼя Побатькові          |        N картрахунку        |Реєстрац.н|  Сума     |\n");
/*********
123456 123456 1234567890123456789012345678901234567890 12345678901234567890123456789 1234567890 1234567890
*********/
fprintf(*ffr,"\
------------------------------------------------------------------------------------------------------------\n");




return(0);
}

