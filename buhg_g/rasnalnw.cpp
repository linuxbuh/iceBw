/* $Id:$ */
/*07.12.2017    22.01.1994      Белых А.И.      rasnalnw.c
Распечатка налоговых накладных
*/
#include	"buhg_g.h"
#define		KOLSTRLIST 105

int rasnalnw9(short dd,short md,short gd,int skl,const char *nomdok,const char *imaf,short lnds,GtkWidget *wpredok);

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza bd;
extern short srtnk;

int rasnalnw(short dd,short md,short gd,int skl,const char *nomdok,
const char *imaf,short lnds,  //Тип документа
GtkWidget *wpredok)
{
/******************



if(iceb_u_sravmydat(dd,md,gd,1,1,2015) >= 0 && iceb_u_sravmydat(dd,md,gd,1,4,2016) < 0)
  return(rasnalnw7(dd,md,gd,skl,nomdok,imaf,lnds,wpredok));

if(iceb_u_sravmydat(dd,md,gd,1,4,2016) >= 0)
***************/
//  return(rasnalnw8(dd,md,gd,skl,nomdok,imaf,lnds,wpredok));
  return(rasnalnw9(dd,md,gd,skl,nomdok,imaf,lnds,wpredok));

return(1);
 
}

/*****************************************************/


int rasnalnw9(short dd,short md,short gd,int skl,const char *nomdok,
const char *imaf,short lnds,  //Тип документа
GtkWidget *wpredok)
{
SQL_str         row,row1,rowtmp;
int		kolstr;
char		strsql[1024];
FILE            *f1=NULL;
short           i;
double          itogo;
double          bb;
double		sumt; /*Сумма по возвратной таре*/
int		vt=0;
double		sumkor;
class iceb_u_str bros("");
class iceb_u_str kpos("");
int		tipz;
class iceb_u_str nomnn(""); /*Номер налоговой накладной*/
class iceb_u_str naim("");
short		kgrm;
double		cena,kolih;
short		vtar;
class iceb_u_str ei("");
short		metz;
short		dnn,mnn,gnn;
class iceb_u_str kodop("");
class iceb_u_str imaf_nast("matnast.alx");


sumt=0.;
vt=0;
iceb_poldan("Код группы возвратная тара",&vt,imaf_nast.ravno(),wpredok);

class iceb_u_str kodkontr00("00");

SQLCURSOR cur,cur1,curtmp;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gd,md,dd,skl,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден документ"));
  repl.plus(" !!!");

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер документа"),
  nomdok,
  gettext("Склад"),
  skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);  
  return(1);
 }
float pnds=atof(row[13]);

kpos.new_plus(row[3]);
tipz=atoi(row[0]);
nomnn.new_plus(row[5]);
kodop.new_plus(row[6]);
iceb_u_rsdat(&dnn,&mnn,&gnn,row[12],2);

kodkontr00.new_plus(row[15]);

if(tipz == 2)
 if(iceb_u_SRAV(kodkontr00.ravno(),"00",0) == 0)
  kodkontr00.new_plus(iceb_getk00(0,kodop.ravno(),wpredok));

if(kodkontr00.getdlinna() <= 1)
 kodkontr00.new_plus("00");


sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,skl);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

sumkor=0.;


if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  metz=atoi(row[0]);
  if(metz == 13)
   {
    sumkor=iceb_u_atof(row[1]);
    sumkor=iceb_u_okrug(sumkor,0.01);
   }
 }


class iceb_u_str kod_vid_deqt("");
if(lnds == 4)
 kod_vid_deqt.new_plus("7");
 
//Печатаем шапку документа
if(iceb_nn9_start(imaf,dd,md,gd,dnn,mnn,gnn,nomnn.ravno(),kpos.ravno(),tipz,kodkontr00.ravno(),&f1,wpredok) != 0)
 return(1);
iceb_insfil("nn9_hap.alx",f1,wpredok);

class iceb_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dd,md,gd,skl,nomdok,imatmptab,&tabtmp,NULL,wpredok) != 0)
 return(1);


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

/*Готовим отсортированный файл*/

itogo=0.;
i=0;
class iceb_u_str kodtov("");
int nomer_str=0;
int metka_imp_tov=0;
class iceb_u_str kdstv("");

while(curtmp.read_cursor(&rowtmp) != 0)
 {

  kgrm=atoi(rowtmp[0]);
  naim.new_plus(rowtmp[1]);
  cena=atof(rowtmp[4]);
  cena=iceb_u_okrug(cena,okrcn);
  ei.new_plus(rowtmp[6]);
  vtar=atoi(rowtmp[7]);
  kolih=atof(rowtmp[9]);
  kodtov.new_plus(rowtmp[12]);
  metka_imp_tov=atoi(rowtmp[13]);
  kdstv.new_plus(rowtmp[14]);

  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  if(kgrm == vt || vtar == 1)
   {
    sumt+=bb;
    continue;
   }

  i++;


  iceb_nn9_str(++nomer_str,0,naim.ravno(),ei.ravno(),kolih,cena,bb,kodtov.ravno(),metka_imp_tov,"",kdstv.ravno(),f1,wpredok);
  itogo+=bb;


 }

/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
class iceb_u_str naiusl("");
class iceb_u_str kod_usl("");
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;

  bb=iceb_u_okrug(bb,okrg1);

  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius,ku from Uslugi where kodus=%s",row[10]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     {
      naiusl.new_plus(row1[0]);
      kod_usl.new_plus(row1[1]);
     }
    else
     {
      kod_usl.new_plus("");
      naiusl.new_plus("");
     }
   }  
  if(naiusl.ravno()[0] == '\0')
   naiusl.new_plus(row[7]);  
  else
   {
    naiusl.plus(" ",row[7]);
   }
  iceb_nn9_str(++nomer_str,0,naiusl.ravno(),ei.ravno(),kolih,cena,bb,"",0,kod_usl.ravno(),"",f1,wpredok);
  itogo+=bb;
 }


fprintf(f1,"ICEB_LST_END\n");
//Печатаем концовку налоговой накладной
iceb_nn9_end(tipz,lnds,"matnast.alx",kodop.ravno(),kpos.ravno(),f1,wpredok);

double suma_dok=0;
double suma_nds=0;

/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(itogo+sumkor)*pnds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(itogo+sumkor)*pnds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }


/*Загальна сума з ПДВ*/
suma_dok=itogo+sumkor+suma_nds;
/*записываем в шапку суммы*/
iceb_nn9_start_s(imaf,lnds,suma_dok,suma_nds,sumt,wpredok);


iceb_ustpeh(imaf,3,&kolstr,wpredok);
iceb_rnl(imaf,kolstr,"",&iceb_nn9_hap,wpredok);
return(0);
}
