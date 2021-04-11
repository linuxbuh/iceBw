/* $Id: sumdokw.c,v 1.13 2014/07/31 07:08:27 sasa Exp $ */
/*12.07.2015	05.02.1999	Белых А.И.	sumdokw.c
Вычисление данных по документа
*/
#include	"buhg_g.h"

extern short	vtara; /*Код группы возвратная тара*/
extern double	okrcn;
extern double   okrg1;
extern SQL_baza bd;

void            sumdokw(SQL_str row,
double *suma, //Сумма по документа
double *sumabn, //Сумма без НДС*/
double *ves,  //Вес по накладной
int lnds, //Метка НДС*/
double *sumsnds,
float pnds)
{
char		strsql[1024];
SQL_str         row1;
double		kolih;
double		cena;
double		bb;
int		kodm,kgrm;
float		fas;
class iceb_u_str ei("");
short		vtr;
short		mnds; /*1-НДС в том числе*/


fas=kgrm=0;

kodm=atoi(row[4]);
kolih=atof(row[5]);
cena=atof(row[6]);
//  Для вложенного налога
if(lnds == 0 || lnds == 4)
  bb=cena+cena*pnds/100.;
else
  bb=cena;

bb=iceb_u_okrug(bb,okrg1);
bb=bb*kolih;
*sumsnds+=iceb_u_okrug(bb,okrg1);

vtr=atoi(row[8]);
ei.new_plus(row[7]);
mnds=atoi(row[10]);
bb=cena*kolih;
bb=iceb_u_okrug(bb,okrg1);

sprintf(strsql,"select fasv,kodgr from Material where kodm=%d",
kodm);
fas=0.;
kgrm=0;
SQLCURSOR cur1;
if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
 {
  //printw("\nstrsql=%s\n",strsql);
  printf("sumdokw-%s %d !\n",gettext("Не найден код материалла"),kodm);
 }
else
 {
  fas=atof(row1[0]);
  kgrm=atoi(row1[1]);
 }
if(vtr == 1 || (kgrm == vtara && vtara != 0) || (lnds > 0 && lnds < 4) || mnds == 1)
  *sumabn+=bb;
else
  *suma+=bb;

/*Читаем карточку материалла*/
if(atoi(row[3]) != 0)
 {
  fas=0.;
  sprintf(strsql,"select fas from Kart where sklad=%s and nomk=%s",
  row[1],row[3]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    printf("sumdok-");
    printf(gettext("Нет на складе %s карточки %s !"),row[1],row[3]);
    printf("\n");
   }
  else
    fas=atof(row1[0]);

 }

if(fas > 0.00001 )
 {
  *ves=*ves+kolih*fas;
 }
else 
  if(iceb_u_SRAV(ei.ravno(),"Кг",0) == 0 || iceb_u_SRAV(ei.ravno(),"кг",0) == 0 || iceb_u_SRAV(ei.ravno(),"КГ",0) == 0)
   {
    *ves=*ves+kolih;
   }

}
