/* $Id: bsizw.c,v 1.11 2013/09/26 09:46:46 sasa Exp $ */
/*14.04.2020    20.06.1996      Белых А.И.      bsizw.c
Получение балансовой стоимости и износа на текущюю дату
Если вернули 0-нашли запись
	     1-не нашли
*/
#include        "buhg_g.h"

extern short	startgoduos; /*Стартовый год*/
extern SQL_baza bd;

int bsizw(int in, //Инвентарный номер
int ceh, //Цех/участок
short dp,short mp,short gp, //Дата
class bsizw_data *bal_st,
FILE *ff,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row;
SQLCURSOR curr;
char		strsql[1024];
double		bss,izz,bssby,izzby;
short		d,m,g;
short		tip;
class iceb_u_str startos(gettext("ОСТ"));
char metka_zap[8];

d=dp;
m=mp;
g=gp;

if(startgoduos != gp || mp != 1 || dp != 1)
  iceb_u_dpm(&d,&m,&g,2); 

bal_st->clear();
if(ceh != 0)
 sprintf(strsql,"select datd,tipz,bs,iz,kodop,bsby,izby,nomd from Uosdok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and innom=%d \
and podr=%d and podt=1 order by datd asc",startgoduos,1,1,g,m,d,in,ceh);
else
 sprintf(strsql,"select datd,tipz,bs,iz,kodop,bsby,izby,nomd from Uosdok1 where \
datd >= '%04d-%02d-%02d' and datd <= '%04d-%02d-%02d' and innom=%d \
and podt=1 order by datd asc",startgoduos,1,1,g,m,d,in);

//printw("\n%s\n",strsql);

class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }


//printw("kolstr=%d\n",kolstr);
//OSTANOV();


if(kolstr == 0)
  return(1);

if(ff != NULL)
 {
  fprintf(ff,"%s-Список документов.\n%s:%d %s:%d  %s:%d.%d.%d\n",__FUNCTION__,
  gettext("Инвентарный номер"),
  in,
  gettext("Подразделение"),
  ceh,
  gettext("Дата"),
  dp,mp,gp);

  fprintf(ff,"\
----------------------------------------------------------------------------\n");
  fprintf(ff,"\
 |  Дата    |Номер док.|Код оп.|Бал. н.у. |Износ н.у.|Бал. бух.у|Износ б.у.|\n");
  fprintf(ff,"\
----------------------------------------------------------------------------\n");

 }


while(cur.read_cursor(&row) != 0)
 {
  tip=atoi(row[1]);
  iceb_u_rsdat(&d,&m,&g,row[0],2);

  bss=atof(row[2]);
  izz=atof(row[3]);  
  bssby=atof(row[5]);
  izzby=atof(row[6]);  

  if(tip == 2)
   {
    bss*=(-1.);
    izz*=(-1.);
    bssby*=(-1.);
    izzby*=(-1.);
   }

  if(ff != NULL)
   {
    if(tip == 1)
     strcpy(metka_zap,"+");
    else
     strcpy(metka_zap,"-");
     
    fprintf(ff,"%s %02d.%02d.%d %-10s %-7s %10.2f %10.2f %10.2f %10.2f\n",
    metka_zap,d,m,g,row[7],row[4],bss,izz,bssby,izzby);
   }

  if(tip == 1 && startgoduos == g && iceb_u_SRAV(row[4],startos.ravno(),0) == 0)
   {
    bal_st->sbs=atof(row[2]);
    bal_st->siz=atof(row[3]);
    bal_st->sbsby=atof(row[5]);
    bal_st->sizby=atof(row[6]);
    continue;
   }   


  bal_st->bs+=bss;
  bal_st->iz+=izz;
  bal_st->bsby+=bssby;
  bal_st->izby+=izzby;

 }

if(ff != NULL)
 {
  fprintf(ff,"\
----------------------------------------------------------------------------\n");
 
  fprintf(ff,"%31s:%10.2f %10.2f %10.2f %10.2f\n",
  gettext("Итого"),bal_st->sbs+bal_st->bs,bal_st->siz+bal_st->iz,bal_st->sbsby+bal_st->bsby,bal_st->sizby+bal_st->izby);
 }

/*Узнаем амортизацию для налогового учёта*/
if(ceh != 0)
  sprintf(strsql,"select da,suma from Uosamor where innom=%d and \
da >= '%04d-01-01' and da < '%04d-%02d-01' and \
podr=%d and suma <> 0. order by da asc",in,startgoduos,gp,mp,ceh);
else
  sprintf(strsql,"select da,suma from Uosamor where innom=%d and \
da >= '%04d-01-01' and da < '%04d-%02d-01' and \
suma <> 0. order by da asc",in,startgoduos,gp,mp);
if(ff != NULL)
 fprintf(ff,"\n%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(ff != NULL)
 {
  fprintf(ff,"Амортизация для налогового учёта.\n");

  fprintf(ff,"\
-------------------\n");

  fprintf(ff,"\
   Дата   |Сумма\n");

  fprintf(ff,"\
-------------------\n");
 
 }
if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(ff != NULL)
   fprintf(ff,"%s %10s\n",iceb_u_datzap(row[0]),row[1]);

  bal_st->iz1+=atof(row[1]);

 }


if(ff != NULL)
 {
  fprintf(ff,"\
-------------------\n");
  fprintf(ff,"%7s %10.2f\n",gettext("Итого"),bal_st->iz1);
 }

/*Узнаем амортизацию для бухгалтерского учёта*/
if(ceh != 0)
  sprintf(strsql,"select da,suma from Uosamor1 where innom=%d and \
da >= '%04d-01-01' and da < '%04d-%02d-01' and \
podr=%d and suma <> 0. order by da asc",in,startgoduos,gp,mp,ceh);
else
  sprintf(strsql,"select da,suma from Uosamor1 where innom=%d and \
da >= '%04d-01-01' and da < '%04d-%02d-01' and \
suma <> 0. order by da asc",in,startgoduos,gp,mp);

if(ff != NULL)
 fprintf(ff,"\n%s\n",strsql);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
 }
//if(in == 2843)
// printw("%s\n",strsql);
if(ff != NULL)
 {
  fprintf(ff,"Амортизация для бухгалтерского учёта.\n");

  fprintf(ff,"\
-------------------\n");

  fprintf(ff,"\
   Дата   |Сумма\n");

  fprintf(ff,"\
-------------------\n");
 }

if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
  if(ff != NULL)
   fprintf(ff,"%s %10s\n",iceb_u_datzap(row[0]),row[1]);

// if(in == 2843)
//   printw("%s %s %s\n",row[1],row[0],row[2]);

  bal_st->iz1by+=atof(row[1]);

 }

if(ff != NULL)
 {
  fprintf(ff,"\
-------------------\n");
  fprintf(ff,"%7s %10.2f\n",gettext("Итого"),bal_st->iz1by);
 }

if(ff != NULL)
 {
  fprintf(ff,"%s  sbs=%.2f bs-%.2f siz=%.2f iz-%.2f iz1=%.2f\n\
sbsby=%.2f bsby-%.2f sizby=%.2f izby-%.2f iz1by=%.2f\n",__FUNCTION__,
  bal_st->sbs,bal_st->bs,bal_st->siz,bal_st->iz,bal_st->iz1,bal_st->sbsby,bal_st->bsby,bal_st->sizby,bal_st->izby,bal_st->iz1by);
 }
/*********
printw("bsiz  sbs=%.2f bs-%.2f siz=%.2f iz-%.2f iz1=%.2f\n\
sbsby=%.2f bsby-%.2f sizby=%.2f izby-%.2f iz1by=%.2f\n",
bal_st->sbs,bal_st->bs,bal_st->siz,bal_st->iz,bal_st->iz1,bal_st->sbsby,bal_st->bsby,bal_st->sizby,bal_st->izby,bal_st->iz1by);
OSTANOV();
*************/
bal_st->kbsnu=bal_st->sbs+bal_st->bs;
bal_st->kiznu=bal_st->siz+bal_st->iz+bal_st->iz1;

bal_st->kbsbu=bal_st->sbsby+bal_st->bsby;
bal_st->kizbu=bal_st->sizby+bal_st->izby+bal_st->iz1by;

bal_st->osnu=bal_st->kbsnu-bal_st->kiznu;
bal_st->osbu=bal_st->kbsbu-bal_st->kizbu;

return(0);

}
