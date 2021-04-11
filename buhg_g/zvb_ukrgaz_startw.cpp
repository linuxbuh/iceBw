/*$Id: zvb_ukrgaz_startw.c,v 1.7 2013/09/26 09:47:01 sasa Exp $*/
/*26.10.2009	15.10.2009	Белых А.И.	zvb_ukrgaz_startw.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg_g.h"



int zvb_ukrgaz_startw(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ffr, //Файл распечатки
GtkWidget *wpredok)
{



sprintf(imaf_dbf,"ukrgazbank.dbf");
sprintf(imaf_dbf_tmp,"ukrgazdbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf_tmp,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"ukrgaz%d.txt",getpid());
if((*ffr = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return(1);
 }

fprintf(*ffr,"%s\n\n",iceb_get_pnk("00",0,wpredok));
fprintf(*ffr,"Відомість для зарахування на карт-рахунки\n");

fprintf(*ffr,"\
--------------------------------------------------------------------------------------------\n");

fprintf(*ffr,"\
N п/п |Таб.н.|      Прізвище Ім'я Побатькові          |   ІНН    |N картрахунку |  Сумма    |\n");
/*********
123456 123456 1234567890123456789012345678901234567890 1234567890 12345678901234 1234567890 
*********/
fprintf(*ffr,"\
--------------------------------------------------------------------------------------------\n");





return(0);
}

