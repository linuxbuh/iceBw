/*$Id: zvb_kievrus_endw.c,v 1.3 2013/08/13 06:10:04 sasa Exp $*/
/*19.10.2019	11.10.2011	Белых А.И.	zvb_kievrus_endw.c
Концовки файлов
*/
#include <errno.h>
#include "buhg_g.h"
#include        "dbfhead.h"

void kievrus_h(const char *imaf,long kolz,GtkWidget *wpredok);

void zvb_kievrus_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,double sumai,int kolzap,FILE *ffr,FILE *ff_dbf_tmp,GtkWidget *wpredok)
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
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",ruk.fio.ravno(),glbuh.fio.ravno());

iceb_podpis(ffr,wpredok); 

fclose(ffr);

fclose(ff_dbf_tmp);

iceb_perecod(2,imaf_dbf_tmp,wpredok); /*Перекодировка*/

kievrus_h(imaf_dbf,kolzap,wpredok);
iceb_cat(imaf_dbf,imaf_dbf_tmp,wpredok);

unlink(imaf_dbf_tmp);

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void kievrus_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void kievrus_h(const char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
#define		kolpol 4 //Количество колонок (полей) в dbf файле
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
kievrus_f(&f[shetshik++],"SBK_NUM", 'C',29, 0,&header_len,&rec_len);
kievrus_f(&f[shetshik++],"SBK_SUM", 'N', 12, 2,&header_len,&rec_len);
kievrus_f(&f[shetshik++],"SBK_INN", 'C', 10, 0,&header_len,&rec_len);
kievrus_f(&f[shetshik++],"SBK_FIO", 'C', 50, 0,&header_len,&rec_len);


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
