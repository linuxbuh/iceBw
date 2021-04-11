/*$Id: zvb_corp2_startw.c,v 1.1 2014/08/31 06:19:23 sasa Exp $*/
/*20.11.2019	19.08.2014	Белых А.И.	zvb_corp2_startw.c
Меню для начала 
*/

#include        <errno.h>
#include    "buhg_g.h"



int zvb_corp2_startw(char *imafr,//имя файла распечатки
char *imaf_dbf, 
char *imaf_dbf_tmp, //имя файла в электронном виде
FILE **ff,  //Файл в электонном виде
FILE **ff_lst, //Файл распечатки
GtkWidget *wpredok)
{

sprintf(imaf_dbf,"corp2.dbf");
sprintf(imaf_dbf_tmp,"corp2dbf%d.tmp",getpid());

if((*ff = fopen(imaf_dbf_tmp,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf_tmp,"",errno,wpredok);
  return(1);
 }
sprintf(imafr,"corp2%d.txt",getpid());
if((*ff_lst = fopen(imafr,"w")) == NULL)
 {
  iceb_er_op_fil(imafr,"",errno,wpredok);
  return(1);
 }

fprintf(*ff_lst,"%s\n\n",iceb_get_pnk("00",0,wpredok));
fprintf(*ff_lst,"%s\n",gettext("Ведомость для зачисления на карт-счета"));

class iceb_rnl_c rd;
zvb_head_lstw(&rd,NULL,*ff_lst);


#if 0
##################################3
fprintf(*ff_lst,"\
------------------------------------------------------------------------------------------------------------\n");

fprintf(*ff_lst,"\
N п/п | Т.н. |      Призвище Ім'я Побатькові          |  Номер картрахунку          |Реєст. кар|  Сума     |\n");
/*********
123456 1234567890123456789012345678901234567890 12345678901234 1234567890 1234567890 1234567890
*********/
fprintf(*ff_lst,"\
------------------------------------------------------------------------------------------------------------\n");
#####################################
#endif





return(0);
}

