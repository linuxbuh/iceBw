/*$Id: zvb_corp2_endw.c,v 1.1 2014/08/31 06:19:22 sasa Exp $*/
/*20.11.2019	19.08.2014	Белых А.И.	zvb_corp2_endw.c
Концовки файлов

"Державний ощадний банк України"
тел. 55-45-21 Алексей
Ім’я поля 	Тип		Довжина	Найменування реквізиту
NSC		Char		29	рахунок отримувача
SUMMA		Numeric		20.2	сума платежу (в грн)
FIO		Character	38	найменування отримувача
ID_KOD		Numeric		14	ідентифікатор код отримувача
*/
#include <errno.h>
#include "buhg_g.h"
#include "dbfhead.h"

void corp2_h(const char *imaf,long kolz,GtkWidget *wpredok);

void zvb_corp2_endw(const char *imaf_dbf,const char *imaf_dbf_tmp,const char *imaf_lst,double sumai,int kolzap,FILE *ff_lst,FILE *ff_dbf_tmp,GtkWidget *wpredok)
{
class iceb_fioruk_rk ruk;
class iceb_fioruk_rk glbuh;

iceb_fioruk(1,&ruk,wpredok);
iceb_fioruk(2,&glbuh,wpredok);

zvb_end_lstw(imaf_lst,sumai,ff_lst,wpredok);



fputc(26, ff_dbf_tmp);

#if 0
######################################################3
fprintf(ff_lst,"\
------------------------------------------------------------------------------------------------------------\n");
fprintf(ff_lst,"%*s:%10.2f\n",iceb_u_kolbait(95,"Разом"),"Разом",sumai);


fprintf(ff_lst,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",ruk.fio.ravno(),glbuh.fio.ravno());

iceb_podpis(ff_lst,wpredok); 

fclose(ff_lst);
######################################################
#endif



fclose(ff_dbf_tmp);

iceb_perecod(2,imaf_dbf_tmp,wpredok); /*Перекодировка*/

corp2_h(imaf_dbf,kolzap,wpredok);
iceb_cat(imaf_dbf,imaf_dbf_tmp,wpredok);

unlink(imaf_dbf_tmp);

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void corp2_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void corp2_h(const char *imaf,long kolz,GtkWidget *wpredok)
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
corp2_f(&f[shetshik++],"NSC", 'C', 29, 0,&header_len,&rec_len);
corp2_f(&f[shetshik++],"SUMMA", 'N', 20, 2,&header_len,&rec_len);
corp2_f(&f[shetshik++],"FIO", 'C', 38, 0,&header_len,&rec_len);
corp2_f(&f[shetshik++],"ID_KOD", 'N', 14, 0,&header_len,&rec_len);


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
