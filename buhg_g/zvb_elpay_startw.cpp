/*$Id: zvb_ibank2ua_start.c,v 5.16 2014/04/30 06:12:27 sasa Exp $*/
/*25.10.2019	17.10.2019	Белых А.И. 	zvb_elpay_start.c
Начало формирования данных для зачисления на карт-счета для системы нтернет ELPay 
Если вернули 0-шапка сформирована
             1-нет
*/

#include <errno.h>
#include "buhg_g.h"



int zvb_elpay_startw(char *imaf_lst,//имя файла распечатки
char *imaf_csv, //имя файла в электронном виде
char *imaf_dbf,
class iceb_u_str *edrpou, /*ЕДРПОУ*/
const char *kod00,
FILE **ff_csv,  //Файл в электонном виде
FILE **ff_lst, //Файл распечатки
FILE **ff_dbf,
GtkWidget *wpredok)
{

class zvb_menu_lst_r rm;

if(zvb_menu_lstw(&rm,wpredok) != 0)
 return(1);

sprintf(imaf_csv,"elpay.csv");
if((*ff_csv = fopen(imaf_csv,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_csv,"",errno,wpredok);
  return(1);
 }
sprintf(imaf_lst,"elpayua%d.txt",getpid());
if((*ff_lst = fopen(imaf_lst,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_lst,"",errno,wpredok);
  return(1);
 }

sprintf(imaf_dbf,"elpayua.dbf");
if((*ff_dbf = fopen(imaf_dbf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_dbf,"",errno,wpredok);
  return(1);
 }

zvb_start_lstw(&rm,*ff_lst,wpredok);
class iceb_rnl_c rd;
zvb_head_lstw(&rd,NULL,*ff_lst);


class iceb_u_str shet_sp("");
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
/*узнаём реквизиты организации*/
sprintf(strsql,"select kod from Kontragent where kodkon='%s'",kod00);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  edrpou->new_plus(row[0]);
 }


return(0);
}
