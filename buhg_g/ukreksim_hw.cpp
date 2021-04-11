/*$Id: ukreksim_hw.c,v 1.6 2013/05/17 14:58:28 sasa Exp $*/
/*20.05.2016	25.01.2006 	Белых А.И.	ukreksim_hw.c
Формирование заголовка dbf файла для Укрексимбанка
*/
#include <errno.h>
#include    "buhg_g.h"
#include        "dbfhead.h"

void ukreksim_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,int *header_len,int *rec_len);


/*********************************/

void ukreksim_hw(const char *imaf,long kolz)
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
/*************
ukreksim_f(&f[shetshik++],"Currency", 'C', 3, 0,&header_len,&rec_len);
ukreksim_f(&f[shetshik++],"Card_Acct", 'C', 10, 0,&header_len,&rec_len);
ukreksim_f(&f[shetshik++],"Tran_Date", 'D', 8, 0,&header_len,&rec_len);
ukreksim_f(&f[shetshik++],"Amount", 'N', 15, 2,&header_len,&rec_len);
**************/
ukreksim_f(&f[shetshik++],"TRAN_DATE", 'D', 8, 0,&header_len,&rec_len); /*Дата транзакции(не исп)*/
ukreksim_f(&f[shetshik++],"CARD_ACCT", 'C', 10, 0,&header_len,&rec_len); /*Номер контракта*/
ukreksim_f(&f[shetshik++],"AMOUNT", 'N', 15, 2,&header_len,&rec_len);    /*Сумма*/
ukreksim_f(&f[shetshik++],"CURRENCY", 'C', 3, 0,&header_len,&rec_len);   /*Буквенный код валюты UAH*/
ukreksim_f(&f[shetshik++],"IDENTCODE", 'C', 32, 0,&header_len,&rec_len); /*Идентификационный код (не обязательно)*/
ukreksim_f(&f[shetshik++],"DETALS", 'C', 32, 0,&header_len,&rec_len);    /*Коментарий (не обязательно)*/


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
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void ukreksim_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
int *header_len,int *rec_len)
{

strncpy(f->name, fn,sizeof(f->name)-1);
f->type = ft;
f->length = l1;
f->dec_point = l2;
*header_len=*header_len+sizeof(DBASE_FIELD);
*rec_len=*rec_len+l1;
}
