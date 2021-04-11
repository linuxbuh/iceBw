/*$Id:$*/
/*25.10.2019	18.09.2008	Белых А.И.	zvb_ibank2ua_end.c
Концовка формирования данных для зачисления на карт-счета для системы Інтернет ELPay 
*/
#include <errno.h>
#include "buhg_g.h"
#include        "dbfhead.h"

void elpay_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,int *header_len,int *rec_len);
void elpay_h(const char *imaf,long kolz);


void zvb_elpay_endw(const char *imaf_lst,const char *imaf_csv,const char *imaf_dbf,double sumai,int kolzap,FILE *ff_lst,FILE *ff_csv,FILE *ff_dbf,GtkWidget *wpredok)
{

zvb_end_lstw(imaf_lst,sumai,ff_lst,wpredok);


fclose(ff_csv);
iceb_perecod(2,imaf_csv,wpredok);

fputc(26,ff_dbf);

fclose(ff_dbf);

char imaf_dbf_tmp[64];
sprintf(imaf_dbf_tmp,"elpay%d.tmp",getpid());

rename(imaf_dbf,imaf_dbf_tmp);

iceb_perecod(2,imaf_dbf_tmp,wpredok); /*Перекодировка*/


elpay_h(imaf_dbf,kolzap);

iceb_cat(imaf_dbf,imaf_dbf_tmp,wpredok);

unlink(imaf_dbf_tmp);

}
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void elpay_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void elpay_h(const char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 6 //Количество колонок (полей) в dbf файле
DBASE_FIELD f[kolpol];
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,NULL);
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
elpay_f(&f[shetshik++],"INN", 'C',10, 0,&header_len,&rec_len);
elpay_f(&f[shetshik++],"NR", 'C', 29, 0,&header_len,&rec_len);
elpay_f(&f[shetshik++],"SUMA", 'D', 12, 2,&header_len,&rec_len);
elpay_f(&f[shetshik++],"VAL", 'C', 3, 0,&header_len,&rec_len);
elpay_f(&f[shetshik++],"EDRPOU", 'C', 10, 0,&header_len,&rec_len);
elpay_f(&f[shetshik++],"PIB", 'C', 250, 0,&header_len,&rec_len);


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
 {
  printf("\n%s-%s\n",__FUNCTION__,strerror(errno));
 }

for(i=0; i < kolpol; i++) 
 {
  if(write(fd, &f[i], sizeof(DBASE_FIELD)) < 0)
   {
    printf("\n%s-%s\n",__FUNCTION__,strerror(errno));
   }
 }
fputc('\r', ff);

fclose(ff);

}
