/* $Id: prpuosw.c,v 1.9 2013/08/13 06:09:49 sasa Exp $ */
/*15.05.2017    07.04.1997      Белых А.И.      prpuosw.c
Программа просмотра проводок и определения не сделанных
В учете основных средств
*/
#include        <math.h>
#include        "buhg_g.h"

extern double   okrg1; /*Округление*/

extern struct M2 LL;
extern SQL_baza bd;

void prpuosw(short d,short m,short g, //Дата документа
const char *ndk, //Номер документа
int podd, //Код подразделения
class iceb_u_spisok *spsh,
class iceb_u_double *sn,
class iceb_u_double *sp,
short mr, /*0-Проверить выполнены ли проводки 1-составить список проводок которые нужно сделать 2-распечатку на экран */
class iceb_u_str *shet_suma,
GtkWidget *wpredok)
{
short           mpi=0; /*0 - удалить отметку >0 записать отметку*/
class iceb_u_str sh1("");
double          bb;
short           mop=0; /*0- нет проводок >0 -есть*/
int             i;
int            in;
double		pbs;
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
short		tipz;

if(mr == 1)
 {
  sprintf(strsql,"select innom,bs,bsby from Uosdok1 where \
datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);
  class SQLCURSOR cur;
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }
//  ksh=0;
  while(cur.read_cursor(&row) != 0)
   {

    in=atoi(row[0]);
    pbs=atof(row[2]);
    if(pbs == 0.)
      pbs=atof(row[1]);
    
    class poiinpdw_data rekin;
     
    /*Читаем переменные данные*/
    poiinpdw(in,m,g,&rekin,wpredok);



    int nomer=0;
    if((nomer=spsh->find(rekin.shetu.ravno())) < 0)
     spsh->plus(rekin.shetu.ravno());
    sn->plus(pbs,nomer);
    sp->plus(0.,nomer);

   }
  
  bb=0.;
  shet_suma->new_plus(gettext("Подтвержденные суммы"));
  shet_suma->plus(":");

  for(i=0; i < spsh->kolih() ; i++)
   {
    sprintf(strsql,"%-*s %14s",iceb_u_kolbait(8,spsh->ravno(i)),spsh->ravno(i),iceb_u_prnbr(sn->ravno(i)));
    shet_suma->ps_plus(strsql);
    bb+=sn->ravno(i);
   }

  sprintf(strsql,"%-*s %14s",iceb_u_kolbait(8,gettext("Итого")),gettext("Итого"),iceb_u_prnbr(bb));
  shet_suma->ps_plus(strsql);

  return;
 }

/******************************************/
  /*Ищем проводки*/
if( mr == 0 )
 {
  class SQLCURSOR cur;  
  
  /*Читаем шапку документа*/
  sprintf(strsql,"select tipz from Uosdok where \
datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);

  if(iceb_sql_readkey(strsql,&row1,&cur,wpredok) != 1)
   {
    sprintf(strsql,"prpuosw-%s %s %d.%d.%d",
    gettext("Не найден документ "),ndk,d,m,g);
    iceb_menu_soob(strsql,wpredok);
    return;
   }
  else
   {
    tipz=atoi(row1[0]);
   }  

  mop=mpi=0;
  sprintf(strsql,"select sh,shk,deb,kre from Prov \
where kto='%s' and pod=%d and nomd='%s' and datd='%d-%d-%d'",
  ICEB_MP_UOS,podd,ndk,g,m,d);
  if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
    return;
   }

  if(kolstr == 0)
   {
    goto vp1;
   }

  while(cur.read_cursor(&row) != 0)
   {
    if(fabs(atof(row[3])) > 0.009)
     continue;

    mop++;

    if(tipz == 1 )
      sh1.new_plus(row[0]);

    if(tipz == 2 )
      sh1.new_plus(row[1]);


    for(i=0; i<spsh->kolih() ;i++)
     {
      if(iceb_u_SRAV(spsh->ravno(i),sh1.ravno(),0) == 0)
       {
        sp->plus(atof(row[2]),i);
        break;
       }
     }
   }
  
  /***********************/
  class iceb_u_spisok repl;
  
  if( mop > 0 )
   {
    for(i=0; i < spsh->kolih() ; i++)
     {
      if(fabs(sp->ravno(i) - sn->ravno(i)) > 0.009)
       {
        /************
        sprintf(strsql,gettext("По счету %s не сделаны все проводки ! %.14g != %.14g %d/%d"),
        spsh->ravno(i),sp->ravno(i),sn->ravno(i),i,spsh->kolih());
        **************/
        sprintf(strsql,"%s %s %s! %.14g != %.14g %d/%d",
        gettext("По счёту"),
        spsh->ravno(i),
        gettext("не сделаны все проводки"),
        sp->ravno(i),sn->ravno(i),i,spsh->kolih());
        mpi++;
        repl.plus(strsql);
       }
     }
   }

  if(mpi > 0)
   {
    iceb_menu_soob(&repl,wpredok);
   }
 }

vp1:;

if(mop == 0)
  mpi++;

/*
printw("\n***mpi-%d mop-%d\n",mpi,mop);
OSTANOV();
  */

if(mpi == 0 )
 {
  /*Проводки выполнены*/
  sprintf(strsql,"update Uosdok \
set prov=0 where datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);
 }
else
 {
  /*Делаем отметку о несделанных проводках*/
  sprintf(strsql,"update Uosdok \
set prov=1 where datd='%d-%d-%d' and nomd='%s'",g,m,d,ndk);

 }

if(sql_zap(&bd,strsql) != 0)
 if(sql_nerror(&bd) != ER_DBACCESS_DENIED_ERROR)
  {
   iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  }

}
