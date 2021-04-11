/*$Id: dok_end_muw.c,v 1.3 2014/07/31 07:08:24 sasa Exp $*/
/*17.07.2014	16.12.2005	Белых А.И.	dok_end_muw.c
Распечатка концовки документа для материального учёта
*/
#include "buhg_g.h"

extern SQL_baza bd;

double dok_end_muw(int tipz,
short dd,short md,short gd,
const char *nomdok,int skl,int dlinna,double itogo,float pnds,
int lnds,
int mnt,
int nnds,//0-накладная 1-накладная с ценой НДС 2-накладная с номером заказа 3-счёт-фактура
FILE *ff,
GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
SQLCURSOR cur;

  /*Читаем сумму корректировки*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gd,nomdok,skl,13);

double sumkor=0.;

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  sumkor=atof(row[0]);
  sumkor=iceb_u_okrug(sumkor,0.01);
 }
 

double bb=0.;

if(sumkor != 0)
 {
  if(sumkor > 0.)
    bb=sumkor*100./itogo;
  
  
  if(sumkor < 0.)
    bb=100*sumkor/(itogo-sumkor*-1);
  bb=iceb_u_okrug(bb,0.1);
  if(sumkor < 0)
    sprintf(strsql,"%s %.1f%% :",gettext("Снижка"),bb*(-1));
  if(sumkor > 0)
    sprintf(strsql,"%s %.1f%% :",gettext("Надбавка"),bb);

  fprintf(ff,"%*s %10.2f/З %s %.2f\n",
  iceb_u_kolbait(dlinna+1,strsql),strsql,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);


  if(sumkor < 0)
    sprintf(strsql,gettext("Итого со снижкой :"));
  if(sumkor > 0)
    sprintf(strsql,gettext("Итого с надбавкой :"));

  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(dlinna+1,strsql),strsql,sumkor+itogo);

 }

bb=0.;

if(lnds == 0 || lnds == 4)
 {
  char bros[512];   
  sprintf(bros,"%s %.2f%% :",gettext("НДС"),pnds);
  bb=(itogo+sumkor)*pnds/100.;
  if(tipz == 1) //Читаем сумму НДС введенную вручную
   {
    sprintf(strsql,"select sodz from Dokummat2 where god=%d and sklad=%d \
and nomd='%s' and nomerz=6",gd,skl,nomdok);
    
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
      bb=atof(row[0]);
   }

  bb=iceb_u_okrug(bb,0.01);

  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(dlinna+1,bros),bros,bb);

 }

double sumt=0.;

if(mnt != 0)
   vztr(dd,md,gd,skl,nomdok,&sumt,ff,nnds,wpredok);
      
double itogo1=(itogo+sumkor)+bb+sumt;
strcpy(strsql,gettext("Итого"));
strcat(strsql,":");

fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(dlinna+1,strsql),strsql,itogo1);
return(itogo1);
}
