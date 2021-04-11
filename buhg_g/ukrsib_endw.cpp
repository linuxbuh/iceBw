/*$Id: ukrsib_endw.c,v 1.8 2013/05/17 14:58:28 sasa Exp $*/
/*20.05.2016	29.01.2006	Белых А.И.	ukrsib_endw.c
Концовка формирования данных для зачисления на карт-счета для Укрсиббанка
*/
#include        <errno.h>
#include "buhg_g.h"
#include        "dbfhead.h"

void		ukrsib_h(const char *imaf,long kolz);
void ukrsib_f(DBASE_FIELD *f,char *fn,char  ft,int l1,int l2,int *header_len,int *rec_len);


void ukrsib_endw(char *imafdbf,const char *imaftmp,double sumai,
int nomer,const char *rukov,const char *glavbuh,FILE *ffr,FILE *fftmp,
GtkWidget *wpredok)
{


fputc(26, fftmp);
fprintf(ffr,"\
-----------------------------------------------------------------------\n");
fprintf(ffr,"%*s:%10.2f\n",iceb_u_kolbait(29,"Разом"),"Разом",sumai);


fprintf(ffr,"\n\
Керівник           _____________________ %s\n\n\
Головний бухгалтер _____________________ %s\n",rukov,glavbuh);
iceb_podpis(ffr,wpredok); 
fclose(fftmp);
fclose(ffr);

sprintf(imafdbf,"ukrsib.dbf");
ukrsib_h(imafdbf,nomer);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaftmp,NULL);
unlink(imaftmp);

}

/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void ukrsib_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{
memset(f->name,'\0',sizeof(f->name));
strncpy(f->name, fn,sizeof(f->name)-1);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}

/***********************************************/
void		ukrsib_h(const char *imaf,long kolz)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
//#define		kolpol 9 //Количество колонок (полей) в dbf файле
#define		kolpol 7 //Количество колонок (полей) в dbf файле
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
ukrsib_f(&f[shetshik++],"TRAN_DATE", 'D', 8, 0,&header_len,&rec_len);
ukrsib_f(&f[shetshik++],"SLIP_NR", 'C', 7, 0,&header_len,&rec_len);
ukrsib_f(&f[shetshik++],"TRAN_TYPE", 'C', 2, 0,&header_len,&rec_len);
ukrsib_f(&f[shetshik++],"CARD_ACCT", 'C', 14, 0,&header_len,&rec_len);
ukrsib_f(&f[shetshik++],"AMOUNT", 'N',15, 2,&header_len,&rec_len);
ukrsib_f(&f[shetshik++],"CURRENCY", 'C', 3, 0,&header_len,&rec_len);
ukrsib_f(&f[shetshik++],"OPERATOR", 'C', 10, 0, &header_len,&rec_len);
//ukrsib_f(&f[shetshik++],"START_OV", 'D', 8, 0,&header_len,&rec_len);
//ukrsib_f(&f[shetshik++],"FIN_OV", 'D', 8, 0,&header_len,&rec_len);


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
