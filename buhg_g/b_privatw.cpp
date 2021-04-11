/*$Id: b_privatw.c,v 1.9 2013/08/13 06:09:31 sasa Exp $*/
/*20.05.2016	23.03.2007	Белых А.И.	b_privatw.c
Экспорт платёжек для Приват Банка
В базе данных должна быть создана таблица с именем JBKL_SND
Колонки таблицы

TIP        Numeric        1        0        тип документа =0 - дебетовый информационный, платежное  требование, иначе - реальный кредитовый. платежное поручение    
N_D        Numeric        10       0        Номер документа
DATE       Date           8                 Клиентская дата
SUMMA      Numeric        15       2        Сумма документа
NAME_A     Character      40                Наименование счета плательщика (Наименование организации плательщика)
COUNT_A    Numeric        16       0        Счет плательщика
MFO_A      Numeric        9        0        Не используется
KSCH_A     Numeric        16       0        Не используется
BANK_A     Character      45                Не используется
NAME_B     Character      40                Наименование счета получателя (Наименование организации получателя)
MFO_B      Numeric        9        0        МФО банка получателя
COUNT_B    Numeric        16       0        Счет получателя
KSCH_B     Numeric        16       0        Наименование банка получателя (это не наименование)
BANK_B     Character      45                Не используется
N_P        Character      160               Назначение платежа
VAL        Character      4                 Валюта документа
PACKET     Character      1                 Не используется
USER       Character      20                Не используется
K_S        Character      6                 Не используется
OKPO_A     Numeric        10       0        Код ОКПО плательщика
OKPO_B     Numeric        10       0        Код ОКПО получателя
SKP        Character      2                 Не используется
SOURCE     Character      12                Не используется
OI         Character      3                 Не используется
                        
*/
#include <errno.h>
#include        "buhg_g.h"
#include        "dok4w.h"
#include        "dbfhead.h"

extern struct REC rec;
extern SQL_baza bd;

void b_privat_h(char *imaf,long kolz,GtkWidget *wpredok);

extern class REC rec;

int b_privatw(const char *tabl,GtkWidget *wpredok) /*выгрузка в dbf файл*/
{
char		imaf1[64];
FILE		*ff1;
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;

int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа для передачи в банк!"),wpredok);
  return(1);
 }







sprintf(imaf1,"plat_b.txt");
if((ff1 = fopen(imaf1,"w")) == NULL)
 {
  iceb_er_op_fil(imaf1,__FUNCTION__,errno,wpredok);
  return(1);
 }

int nomstr=0;

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  nomstr++;  

  fprintf(ff1," 1\
%*.*s\
%04d%02d%02d\
%15.2f\
%-*.*s\
%-16.16s\
%-9.9s\
%16s\
%45s\
%-*.*s\
%-9.9s\
%-16.16s\
%16s\
%45s\
%-*.*s\
%-4s\
%1s\
%20s\
%6s\
%-10s\
%-10s\
%2s\
%12s\
%3s",
  iceb_u_kolbait(10,rec.nomdk.ravno()),
  iceb_u_kolbait(10,rec.nomdk.ravno()),
  rec.nomdk.ravno(),
  g,m,d,
  rec.sumd,
  iceb_u_kolbait(40,rec.naior.ravno()),
  iceb_u_kolbait(40,rec.naior.ravno()),
  rec.naior.ravno(),
  rec.nsh.ravno(),      
  rec.mfo.ravno(),
  "0",
  "",
  iceb_u_kolbait(40,rec.naior1.ravno()),
  iceb_u_kolbait(40,rec.naior1.ravno()),
  rec.naior1.ravno(),
  rec.mfo1.ravno(),
  rec.nsh1.ravno(),
  "0",
  "",
  iceb_u_kolbait(160,koment.ravno()),
  iceb_u_kolbait(160,koment.ravno()),
  koment.ravno(),
  "UAH",
  "",
  "",
  "",
  rec.kod.ravno(),
  rec.kod1.ravno(),
  "",
  "",
  "");

 }

fputc(26, ff1);

fclose(ff1);

iceb_perecod(2,imaf1,wpredok);

char imafdbf[64];

sprintf(imafdbf,"plat.txt");
b_privat_h(imafdbf,nomstr,wpredok);

/*Сливаем два файла*/
iceb_cat(imafdbf,imaf1,wpredok);
unlink(imaf1);
return(0);

}
#if 0
###############################################################333
int b_privatw(const char *tabl,GtkWidget *wpredok)
{
class iceb_u_str koment("");
char		strsql[512];
short		d,m,g;

int kolstr=0;
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select datd,nomd from %s where vidpl='1'",tabl);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа для передачи в банк!"),wpredok);
  return(1);
 }


//Удаляем все записи в таблице
iceb_sql_zapis("DELETE FROM JBKL_SND",0,0,wpredok);
                        
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  if(readpdokw(tabl,g,row[1],wpredok) != 0)
     continue;

  /*Читаем комментарий*/
  readkomw(tabl,rec.dd,rec.md,rec.gd,rec.nomdk.ravno(),&koment,wpredok);

  sprintf(strsql,"insert into JBKL_SND (TIP,N_D,DATE,SUMMA,NAME_A,COUNT_A,NAME_B,MFO_B,COUNT_B,\
N_P,VAL,OKPO_A,OKPO_B) values (%d,'%s','%d-%d-%d',%.2f,'%s',%s,'%s',%s,%s,'%s','%s','%s','%s')",
  1,row[1],g,m,d,rec.sumd,rec.naior.ravno(),rec.nsh.ravno(),rec.naior1.ravno(),rec.mfo1.ravno(),rec.nsh1.ravno(),
  koment.ravno(),"UAH",rec.kod.ravno(),rec.kod1.ravno());
  
  iceb_sql_zapis(strsql,1,0,wpredok);
    
 }

return(0);
}
                        
############################################33
#endif
/*******************************************/
                        
/******************************************************/
/******************************/
/*Создаем заголовок файла dbf*/
/******************************/
void b_bpivat_f(DBASE_FIELD *f,const char *fn,char  ft,int l1,int l2,
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
void b_privat_h(char *imaf,long kolz,GtkWidget *wpredok)
{
time_t		tmm;
struct  tm      *bf;
FILE		*ff;
DBASE_HEAD  h;
int fd;
int i;
int header_len, rec_len;
#define kolpol  24
DBASE_FIELD f[kolpol];
memset(&f, '\0', sizeof(f));

if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,__FUNCTION__,errno,wpredok);
  return;
 }

memset(&h,'\0',sizeof(h));

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



b_bpivat_f(&f[shetshik++],"TIP", 'N', 1, 0,&header_len,&rec_len);//1
b_bpivat_f(&f[shetshik++],"N_D", 'N', 10, 0,&header_len,&rec_len);//2
b_bpivat_f(&f[shetshik++],"DATE", 'D', 8,0,&header_len,&rec_len);//3
b_bpivat_f(&f[shetshik++],"SUMMA", 'N', 15, 2,&header_len,&rec_len);//4
b_bpivat_f(&f[shetshik++],"NAME_A", 'C',40, 0,&header_len,&rec_len);//5
b_bpivat_f(&f[shetshik++],"COUNT_A", 'N',16, 0,&header_len,&rec_len);//6
b_bpivat_f(&f[shetshik++],"MFO_A", 'N', 9, 0,&header_len,&rec_len);//7
b_bpivat_f(&f[shetshik++],"KSCH_A", 'N', 16, 0,&header_len,&rec_len);//8
b_bpivat_f(&f[shetshik++],"BANK_A", 'C', 45, 4,&header_len,&rec_len);//9
b_bpivat_f(&f[shetshik++],"NAME_B", 'C', 40, 0, &header_len,&rec_len);//10
b_bpivat_f(&f[shetshik++],"MFO_B", 'N', 9, 0,&header_len,&rec_len);//11
b_bpivat_f(&f[shetshik++],"COUNT_B", 'N',16, 0,&header_len,&rec_len);//12
b_bpivat_f(&f[shetshik++],"KSCH_B", 'N', 16, 0,&header_len,&rec_len);//13
b_bpivat_f(&f[shetshik++],"BANK_B", 'C', 45, 0,&header_len,&rec_len);//14
b_bpivat_f(&f[shetshik++],"N_P", 'C',160, 0, &header_len,&rec_len);//15
b_bpivat_f(&f[shetshik++],"VAL", 'C', 4, 0, &header_len,&rec_len);//16
b_bpivat_f(&f[shetshik++],"PACKET", 'C', 1, 0, &header_len,&rec_len);//17
b_bpivat_f(&f[shetshik++],"USER", 'C', 20, 0, &header_len,&rec_len);//18
b_bpivat_f(&f[shetshik++],"K_S", 'C', 6, 0, &header_len,&rec_len);//19
b_bpivat_f(&f[shetshik++],"OKPO_A", 'C', 10, 0, &header_len,&rec_len);//20
b_bpivat_f(&f[shetshik++],"OKPO_B", 'C', 10, 0, &header_len,&rec_len);//21
b_bpivat_f(&f[shetshik++],"SKP", 'C', 2, 0, &header_len,&rec_len);//22
b_bpivat_f(&f[shetshik++],"SOURSE", 'C', 12, 0, &header_len,&rec_len);//23
b_bpivat_f(&f[shetshik++],"OI", 'C', 3, 0, &header_len,&rec_len);//24


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
