/*$Id: ostvaw.c,v 1.7 2013/05/17 14:58:22 sasa Exp $*/
/*12.07.2016	17.04.2003	Белых А.И.	ostvaw.c
Получение списка остатков по водителю и автомобилю
Возвращяем общее количество на остатке
*/
#include	"buhg_g.h"

void  ostva_mas(const char *kodtop,double kolih,iceb_u_spisok *KTC,iceb_u_double *OST);

extern short	startgodupl; //Стартовый год для "Учета путевых листов"
extern SQL_baza bd;

/**************************************/
double	ostvaw(const char *data_ost, //Дата остатка
const char *kodvod, //Код водителя
const char *kodavt, //Код автомобиля
int nom_zap, //Порядковый номер документа до которого нужно узнать остаток
class iceb_u_spisok *KTC, //Список Код топлива
class iceb_u_double *OST, //Массив остатков
FILE *fil,
GtkWidget *wpredok)
{
short dos,mos,gos;
iceb_u_rsdat(&dos,&mos,&gos,data_ost,1);
return(ostvaw(dos,mos,gos,kodvod,kodavt,nom_zap,KTC,OST,fil,wpredok));

}

/*************************************/

double	ostvaw(short dos,short mos,short gos, //Дата остатка
const char *kodvod, //Код водителя
const char *kodavt, //Код автомобиля
int nom_zap, //Порядковый номер документа до которого нужно узнать остаток
class iceb_u_spisok *KTC, //Список Код топлива
class iceb_u_double *OST, //Массив остатков
FILE *fil,
GtkWidget *wpredok)
{
short gn=startgodupl;
char  strsql[512];
long	kolstr=0;
double  kolih=0.;
double  kolihi=0.;
SQL_str row;
int	tipz=0;

//class iceb_fopen fil;
if(fil != NULL)
  {
//   fil.start(imaf_prot,"w",wpredok);
   if(fil != NULL)
     fprintf(fil,"%s-Протокол хода расчёта остака топлива\nКод водителя-%s Код автомобиля-%s\n",__FUNCTION__,kodvod,kodavt);
   
  }

if(gn == 0 || gos < gn)
  gn=gos;

//Смотрим сальдо
if(kodvod[0] != '\0')
  sprintf(strsql,"select kmt,sal from Uplsal where god=%d and ka=%s and kv=%s",
  gn,kodavt,kodvod);
else
  sprintf(strsql,"select kmt,sal from Uplsal where god=%d and ka=%s",
  gn,kodavt);

if(fil != NULL)
 fprintf(fil,"%s-смотрим старовое сальдо\n%s\n",__FUNCTION__,strsql);

SQLCURSOR cur;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }
while(cur.read_cursor(&row) != 0)
 {
  
  kolih=atof(row[1]);
  kolih=iceb_u_okrug(kolih,0.001);
  kolihi+=kolih;

  ostva_mas(row[0],kolih,KTC,OST);
  if(fil != NULL)
   fprintf(fil,"%s-%s %s\n",__FUNCTION__,row[0],row[1]);
 }

if(fil != NULL)
 fprintf(fil,"%s-остаток-%.3f\n",__FUNCTION__,kolihi);

//Смотрим движение топлива по документам

if(kodvod[0] != '\0')
  sprintf(strsql,"select tz,kodtp,kolih,datd,nomd from Upldok1 where datd >= '%d-%d-%d' and \
datd < '%d-%d-%d' and ka=%s and kv=%s",
  gn,1,1,gos,mos,dos,kodavt,kodvod);
else
  sprintf(strsql,"select tz,kodtp,kolih,datd,nomd from Upldok1 where datd >= '%d-%d-%d' and \
datd < '%d-%d-%d' and ka=%s",
  gn,1,1,gos,mos,dos,kodavt);

if(fil != NULL)
 fprintf(fil,"\n%s-смотрим движение топлива по документам\n%s\n",__FUNCTION__,strsql);


//printw("\nostva-%s\n",strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(0.);
 }


if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//    printw("2 - %s %s %s\n",row[0],row[1],row[2]);
  tipz=atoi(row[0]);
    
  kolih=atof(row[2]);
  kolih=iceb_u_okrug(kolih,0.001);
  
  if(tipz == 2)
    kolih*=-1;

  kolihi+=kolih;
  ostva_mas(row[1],kolih,KTC,OST);
  if(fil != NULL)
   fprintf(fil,"%s-%s %s %s %s %s\n",__FUNCTION__,iceb_u_datzap(row[3]),row[4],iceb_u_tipz(row[0]),row[1],row[2]);
 }
if(fil != NULL)
 fprintf(fil,"%s-остаток-%.3f\n",__FUNCTION__,kolihi);

if(nom_zap != 0)
 {
  if(kodvod[0] != '\0')
    sprintf(strsql,"select tz,kodtp,kolih,datd,nomd from Upldok1 where datd = '%d-%d-%d' \
and ka=%s and kv=%s and nz < %d",
    gos,mos,dos,kodavt,kodvod,nom_zap);
  else
    sprintf(strsql,"select tz,kodtp,kolih,kap,datd,nomd from Upldok1 where datd = '%d-%d-%d' \
and ka=%s and nz < %d",
    gos,mos,dos,kodavt,nom_zap);

  if(fil != NULL)
   fprintf(fil,"%s-%s\n",__FUNCTION__,strsql);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

  if(kolstr > 0)
  while(cur.read_cursor(&row) != 0)
   {
//    printw("3 - %s %s %s\n",row[0],row[1],row[2]);
    tipz=atoi(row[0]);

    if(kodvod[0] == '\0')
     if(tipz == 1 && atoi(row[3]) != 0)
      continue;
      
    kolih=atof(row[2]);
    kolih=iceb_u_okrug(kolih,0.001);
    
    if(tipz == 2)
      kolih*=-1;

    kolihi+=kolih;

    if(fil != NULL)
     {
      if(kodvod[0] != '\0')
       fprintf(fil,"%s-%s %s %s %s %s\n",__FUNCTION__,iceb_u_datzap(row[3]),row[4],iceb_u_tipz(row[0]),row[1],row[2]);
      else
       fprintf(fil,"%s-%s %s %s %s %s\n",__FUNCTION__,iceb_u_datzap(row[4]),row[5],iceb_u_tipz(row[0]),row[1],row[2]);
  
     }

    ostva_mas(row[1],kolih,KTC,OST);
   }

  if(fil != NULL)
   fprintf(fil,"%s-остаток-%.3f\n",__FUNCTION__,kolihi);

 }



//printw("kolihi=%f\n",kolihi);

  //Смотрим передачи топлива другим водителям
if(kodvod[0] != '\0')
  sprintf(strsql,"select kodtp,kolih,datd,nomd from Upldok1 where datd >= '%d-%d-%d' and \
  datd < '%d-%d-%d' and kap=%s and kodsp=%s and tz=1",
  gn,1,1,gos,mos,dos,kodavt,kodvod);
else
  sprintf(strsql,"select kodtp,kolih,datd,nomd from Upldok1 where datd >= '%d-%d-%d' and \
  datd < '%d-%d-%d' and kap=%s and tz=1",
  gn,1,1,gos,mos,dos,kodavt);

if(fil != NULL)
 fprintf(fil,"\n%s-смотрим передачи топлива другим водителям\n%s\n",__FUNCTION__,strsql);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);


if(kolstr > 0)
while(cur.read_cursor(&row) != 0)
 {
//  printw("4 - %s %s \n",row[0],row[1]);
  
  kolih=atof(row[1]);
  kolih=iceb_u_okrug(kolih,0.001)*-1;
  
  kolihi+=kolih;

  ostva_mas(row[0],kolih,KTC,OST);
  if(fil != NULL)
   fprintf(fil,"%s-%s %s %s -%s\n",__FUNCTION__,iceb_u_datzap(row[2]),row[3],row[0],row[1]);
 }
if(fil != NULL)
 fprintf(fil,"%s-остаток-%.3f\n",__FUNCTION__,kolihi);

if(kodvod[0] != '\0')
  if(nom_zap != 0)
   {
    sprintf(strsql,"select kodtp,kolih from Upldok1 where datd = '%d-%d-%d' \
and kap=%s and kodsp=%s and tz=1 and nz < %d",
    gos,mos,dos,kodavt,kodvod,nom_zap);

    if(fil != NULL)
     fprintf(fil,"\n%s-%s\n",__FUNCTION__,strsql);
    if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
      iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    //printw("strsql=%s\n",strsql);
    //OSTANOV();
    while(cur.read_cursor(&row) != 0)
     {
//      printw("5 - %s %s \n",row[0],row[1]);
      
      kolih=atof(row[1]);
      kolih=iceb_u_okrug(kolih,0.001)*-1;
      
      kolihi+=kolih;

      ostva_mas(row[0],kolih,KTC,OST);
      if(fil != NULL)
       fprintf(fil,"%s-%s %s %s %s\n",__FUNCTION__,iceb_u_datzap(row[2]),row[3],row[0],row[1]);
     }
    if(fil != NULL)
     fprintf(fil,"%s-остаток-%.3f\n",__FUNCTION__,kolihi);
   
   }


kolihi=iceb_u_okrug(kolihi,0.001);  
return(kolihi);
}

/*************************************/
/*Запись в массивы остатков*/
/*******************************/


void  ostva_mas(const char *kodtop,double kolih,
iceb_u_spisok *KTC, //Список Код топлива
iceb_u_double *OST) //Массив остатков
{
if(KTC == NULL)
 return;

int nomer=0;
if((nomer=KTC->find_r(kodtop)) < 0)
    KTC->plus(kodtop);
OST->plus(kolih,nomer);

}
