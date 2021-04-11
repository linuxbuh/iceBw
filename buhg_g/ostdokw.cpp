/* $Id: ostdokw.c,v 1.11 2013/05/17 14:58:22 sasa Exp $ */
/*12.07.2015    21.09.1994      Белых А.И.      ostdokw.c
Получение остатка по записям в документах привязанных
к карточкам
		       С начала года         Период
		       включая период
		       кол.        цена      кол.   цена
Остаток начальный   0  000         000       000    000
Приход              1  000         000       000    000
Расход              2  000         000       000    000
Остаток             3  000         000       000    000

*/
#include	"buhg_g.h"

int        ostdokw_ras(short dn,short mn,short gn,short dk,short mk,short gk,const char *skk,const char *nkk,class ostatok *data);

extern double   okrg1; /*Округление*/
extern short	startgod; /*Стартовый год просмотров*/
extern SQL_baza	bd;

/**************************************/

int        ostdokw(short dn,short mn,short gn,short dk,short mk,
short gk,int skk,int nkk,class ostatok *data)
 {
  char sklad[64];
  char nomkar[64];
  sprintf(sklad,"%d",skk);
  sprintf(nomkar,"%d",nkk);
  return(ostdokw_ras(dn,mn,gn,dk,mk,gk,sklad,nomkar,data));
 
 }
 
/**************************************/

int        ostdokw(short dn,short mn,short gn,short dk,short mk,
short gk,const char *skk,const char *nkk,class ostatok *data)
 {
  return(ostdokw_ras(dn,mn,gn,dk,mk,gk,skk,nkk,data));
 }

/**************************************/

int        ostdokw_ras(short dn,short mn,short gn,short dk,short mk,
short gk,const char *skk,const char *nkk,class ostatok *data)
{
extern double	okrcn;
double		cena;
int             i;
double          prf;
double          cnn;
short           dp,mp,gp;
short           d,m,g;
double          bb;
double          kol,koll;
short           tz; /*Тип записи*/
SQL_str         row;
char		strsql[512];
int		kolstr=0;
double		kolih=0.;
class iceb_u_str nomd("");
short		godn;
SQLCURSOR cur;

/*
printf("\n%d.%d.%d %d.%d.%d %s %s\n",dn,mn,gn,dk,mk,gk,skk,nkk);
*/

data->clear();

godn=startgod;
if(godn == 0)
 godn=gn;
if(gn < startgod)
  godn=gn; 
 
/*Читаем реквизиты карточки материалла*/
sprintf(strsql,"select cena from Kart where sklad=%s and nomk=%s",skk,nkk);
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  printf("%s-Не найдена карточка материалла %s на складе %s !\n",__FUNCTION__,nkk,skk);
  return(1);
 }
cena=atof(row[0]);
cena=iceb_u_okrug(cena,okrcn);



dp=dn;
mp=mn;
gp=gn;

iceb_u_dpm(&dp,&mp,&gp,2);


cnn=prf=0.;

sprintf(strsql,"select datd,kolih,nomd,tipz from Dokummat1 where \
sklad=%s and nomkar=%s and datd >= '%d-01-01' and \
datd <= '%d-%02d-%02d' order by datd asc",
skk,nkk,godn,gk,mk,dk);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
  return(0);

kol=koll=0.;
while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&d,&m,&g,row[0],2);    
  kolih=atof(row[1]);
  nomd.new_plus(row[2]);
  
  if(iceb_u_sravmydat(d,m,g,dk,mk,gk) > 0)
   {
     break;
   }

  kol=kolih;

  
  tz=atoi(row[3]);
 
  if(tz == 1 && iceb_u_SRAV(nomd.ravno(),"000",0) == 0 && godn != g)
     continue;
  if(tz == 1 && iceb_u_SRAV(nomd.ravno(),"000",0) == 0 && godn == g)
   {
    data->ostg[0]+=kol;
    bb=kol*cena;
    data->ostgc[0]=iceb_u_okrug(bb,okrg1);
    continue;
   }

  data->ostg[tz]+=kol;
  bb=kol*cena;
  data->ostgc[tz]+=iceb_u_okrug(bb,okrg1);

  /*Период*/
  if(iceb_u_sravmydat(d,m,g,dk,mk,gk) <= 0)
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) >= 0)
   {
    data->ostm[tz]+=kol;
    bb=kol*cena;
    data->ostmc[tz]+=iceb_u_okrug(bb,okrg1);
   }

  /*До периода*/
  if(iceb_u_sravmydat(d,m,g,dn,mn,gn) < 0)
   {
    if(tz == 2)
       kol*=(-1);
    prf+=kol;
    bb=kol*cena;
    cnn+=iceb_u_okrug(bb,okrg1);
   }

 }
data->ostg[3]=data->ostg[0]+data->ostg[1]-data->ostg[2];
data->ostm[0]=data->ostg[0]+prf;
data->ostm[3]=data->ostm[0]+data->ostm[1]-data->ostm[2];

data->ostgc[3]=data->ostgc[0]+data->ostgc[1]-data->ostgc[2];
data->ostmc[0]=data->ostgc[0]+cnn;
data->ostmc[3]=data->ostmc[0]+data->ostmc[1]-data->ostmc[2];
/*Убираем погрешность вычисления*/
for(i=0; i<4 ; i++)
 {
  if(data->ostg[i] < 0.00000000001 && data->ostg[i] > -0.00000000001)
     data->ostg[i]=0.;
  if(data->ostm[i] < 0.00000000001 && data->ostm[i] > -0.00000000001)
    data->ostm[i]=0.;
 }

return(0);
}
