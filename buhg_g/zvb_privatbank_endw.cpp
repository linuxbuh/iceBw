/*$Id: zvb_privatbank_end.c,v 5.1 2014/08/31 06:18:19 sasa Exp $*/
/*20.05.2016	17.10.2014	Белых А.И.	zvb_privatbank_endw.c
Концовки файлов
Описание формата файла dbf:

BRANCH     CHAR     3     карточный бранч             обязательно      (в банке должны вам его сообщить)
ZPКOD      CHAR     3     код зарплатного проэкта     обязательно      (в банке дожны вам его сообщить)
CARD_NO    CHAR     16    номер карты                 не обязательно
LSTBL      CHAR     10    Табельный номер сотрудника  обязательно
FAM        CHAR     50    фамилия                     не обязательно
NAME       CHAR     50    имя                         не обязательно
OT         CHAR     50    отчество                    не обязательно
RLSUM      NUM      15.2  сумма                       обязательно
INN        CHAR     20    инн сотрудника              не обязательно
RLKOD      CHAR     30    назначение зачисления       не обязательно

*/
#include <errno.h>
#include "buhg_g.h"
#include "dbfhead.h"

void privatbank_h(const char *imaf,long kolz,GtkWidget *wpredok);

void zvb_privatbank_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp,GtkWidget *wpredok)
{
class iceb_fioruk_rk ruk;
class iceb_fioruk_rk glbuh;

iceb_fioruk(1,&ruk,wpredok);
iceb_fioruk(2,&glbuh,wpredok);

fputc(26, ff_dbf_tmp);


fprintf(ffr,"\
---------------------------------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(80,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
%-*s _____________________ %s\n\n\
%-*s _____________________ %s\n",
iceb_u_kolbait(20,gettext("Руководитель")),
gettext("Руководитель"),
ruk.fio.ravno(),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),
gettext("Главный бухгалтер"),
glbuh.fio.ravno());

iceb_podpis(ffr,wpredok); 

fclose(ffr);

fclose(ff_dbf_tmp);

iceb_perecod(2,imaf_dbf_tmp,wpredok); /*Перекодировка*/

privatbank_h(imaf_dbf,kolzap,wpredok);
iceb_cat(imaf_dbf,imaf_dbf_tmp,wpredok);

unlink(imaf_dbf_tmp);

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void privatbank_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn,sizeof(f->name)-1);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}

/***********************************************/
void privatbank_h(const char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 10 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));
memset(&f, '\0', sizeof(f));

h.version = 3;

time(&tmm);
bf=localtime(&tmm);

h.l_update[0] = bf->tm_year;       /* yymmdd for last update*/
h.l_update[1] = bf->tm_mon+1;       /* yymmdd for last update*/
h.l_update[2] = bf->tm_mday;       /* yymmdd for last update*/

h.count = kolz;              /* number of records in file*/

header_len = sizeof(h);
rec_len = 0;
int shetshik=0;
privatbank_f(&f[shetshik++],"BRANCH", 'C', 3, 0,&header_len,&rec_len);//1
privatbank_f(&f[shetshik++],"ZPKOD", 'C', 3, 0,&header_len,&rec_len);//2
privatbank_f(&f[shetshik++],"CARD_NO", 'C', 16, 0,&header_len,&rec_len);//3
privatbank_f(&f[shetshik++],"LSTBL", 'C', 10, 0,&header_len,&rec_len);//4
privatbank_f(&f[shetshik++],"FAM",'C',50,0,&header_len,&rec_len);//5
privatbank_f(&f[shetshik++],"NAME",'C',50,0,&header_len,&rec_len);//6
privatbank_f(&f[shetshik++],"OT",'C',50,0,&header_len,&rec_len);//7
privatbank_f(&f[shetshik++],"RLSUM",'N',15,2,&header_len,&rec_len);//8
privatbank_f(&f[shetshik++],"INN",'C',20,0,&header_len,&rec_len);//9
privatbank_f(&f[shetshik++],"RLCOD",'C',30,0,&header_len,&rec_len);//10


h.header = header_len + 1;/* length of the header
                           * includes the \r at end
                           */
h.lrecl= rec_len + 1;     /* length of a record
                           * includes the delete
                           * byte
                          */
/*
 printw("h.header=%d h.lrecl=%d\n",h.header,h.lrecl);
*/


fd = fileno(ff);

if(write(fd, &h, sizeof(h)) < 0)
  printf("\n%s-%s\n",__FUNCTION__,strerror(errno));

for(i=0; i < kolpol; i++) 
 {
  if(write(fd, &f[i], sizeof(DBASE_FIELD)) < 0)
   printf("\n%s-%s\n",__FUNCTION__,strerror(errno));
 }
fputc('\r', ff);

fclose(ff);

}
