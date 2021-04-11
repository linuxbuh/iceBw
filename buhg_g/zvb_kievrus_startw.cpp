/*$Id: zvb_kievrus_startw.c,v 1.3 2013/08/13 06:10:04 sasa Exp $*/
/*18.10.2014	24.10.2011	Белых А.И.	zvb_kievrus_startw.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg_g.h"
#include        "dbfhead.h"

int zvb_kievrus_startw(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ffr, //Файл распечатки
GtkWidget *wpredok)
{



sprintf(imaf_dbf,"kievrusbank.dbf");
sprintf(imaf_dbf_tmp,"kievrus%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf_tmp,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"kievrus%d.txt",getpid());
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

