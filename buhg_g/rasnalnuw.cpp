/*$Id:*/ 
/*07.12.2018	22.01.1994 Белых А.И.  rasnalnuw.c
Распечатка налоговых накладных для услуг
*/
#include	"buhg_g.h"

int rasnalnuw9(short dd,short md,short gd,int podr,const char *nomdok,const char *imaf,short lnds,GtkWidget *wpredok);

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int rasnalnuw(short dd,short md,short gd,int podr,
const char *nomdok,const char *imaf,
short lnds, //Тип накладной
GtkWidget *wpredok)
{
/******************



if(iceb_u_sravmydat(dd,md,gd,1,1,2015) >= 0  && iceb_u_sravmydat(dd,md,gd,1,4,2016) < 0) 
   return(rasnalnuw7(dd,md,gd,podr,nomdok,imaf,lnds,wpredok));
if(iceb_u_sravmydat(dd,md,gd,1,4,2016) >= 0 ) 
***************/
//   return(rasnalnuw8(dd,md,gd,podr,nomdok,imaf,lnds,wpredok));
   return(rasnalnuw9(dd,md,gd,podr,nomdok,imaf,lnds,wpredok));

return(1);
}

/**********************************************************************/

int rasnalnuw9(short dd,short md,short gd,int podr,
const char *nomdok,const char *imaf,
short lnds, //Тип накладной
GtkWidget *wpredok)
{
SQL_str         row,row1;
int		kolstr;
char		strsql[1024];
FILE            *f1;
short           i;
double          itogo;
double          bb;
double		sumkor;
class iceb_u_str kpos("");
int		tipz=0;
class iceb_u_str nomnn(""); /*Номер налоговой накладной*/
class iceb_u_str naim("");
double		cena,kolih;
class iceb_u_str ei("");
short 		metka;
int		kodzap;
short		dnn,mnn,gnn; //Дата налоговой накладной
class iceb_u_str kodop("");
float		procent=0.;
class iceb_u_str naimnal("");

float pnds=0.;

SQLCURSOR curr;		
SQLCURSOR cur;		
class iceb_u_str kodkontr00("00");


/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tp,sumkor,datop,nomnn,forop,uslpr,osnov,datpnn,kodop,pn,k00 \
from Usldokum where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s %s!",__FUNCTION__,gettext("Не найден документ"));
  repl.plus(strsql);
  
  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  dd,md,gd,
  gettext("Документ"),
  nomdok,
  gettext("Подразделение"),
  podr);
  
  repl.ps_plus(strsql);
  
  iceb_menu_soob(&repl,wpredok);
  return(1);
 }
pnds=atof(row[10]);
sumkor=0.;


kpos.new_plus(row[0]);
tipz=atoi(row[1]);
sumkor=atof(row[2]);

iceb_u_rsdat(&dnn,&mnn,&gnn,row[8],2);

nomnn.new_plus(row[4]);
kodop.new_plus(row[9]);

kodkontr00.new_plus(row[11]);
if(tipz == 2)
 if(iceb_u_SRAV(kodkontr00.ravno(),"00",0) == 0)
  kodkontr00.new_plus(iceb_getk00(1,kodop.ravno(),wpredok));
//  iceb_poldan(kodop.ravno(),&kodkontr00,"uslnast.alx",wpredok);

if(kodkontr00.getdlinna() <= 1)
 kodkontr00.new_plus("00");

usldopnalw(tipz,kodop.ravno(),&procent,&naimnal);




//печатаем заголовок налоговой накладной
if(iceb_nn9_start(imaf,dd,md,gd,dnn,mnn,gnn,nomnn.ravno(),kpos.ravno(),tipz,kodkontr00.ravno(),&f1,wpredok) != 0)
 return(1);

iceb_insfil("nn9_hap.alx",f1,wpredok);


sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
gd,md,dd,podr,nomdok,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

class iceb_u_str kod_ei_usl;
iceb_poldan("Код единицы измерения услуги",&kod_ei_usl,"uslnast.alx",wpredok);

char razdel[8];
strcpy(razdel," I");
itogo=0.;
i=0;
int nomer_str=0;
class iceb_u_str kod_usl("");
while(cur.read_cursor(&row) != 0)
 {
  i++;

  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);

  /*Читаем наименование материалла*/

  if(metka == 0)
   sprintf(strsql,"select naimat,ku from Material where kodm=%d",kodzap);

  if(metka == 1)
   sprintf(strsql,"select naius,ku from Uslugi where kodus=%d",kodzap);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    naim.new_plus("");
    if(metka == 0)
     sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodzap);
    if(metka == 1)
     sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodzap);
    iceb_menu_soob(strsql,wpredok);
    kod_usl.new_plus("");
   }
  else
   {
    kod_usl.new_plus(row1[1]);
    naim.new_plus(row1[0]);
    if(row[5][0] != '\0')
     {
      naim.plus(" ",row[5]);
     }
   }
  ei.new_plus(row[4]);

  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);

  if(i == 2)
   {
    memset(razdel,'\0',sizeof(razdel));
   }  

  /*печатаем строку*/
  if(iceb_u_proverka(kod_ei_usl.ravno(),ei.ravno(),0,1) == 0)    
   iceb_nn9_str(++nomer_str,1,naim.ravno(),ei.ravno(),kolih,cena,bb,"",0,kod_usl.ravno(),"",f1,wpredok);
  else
   iceb_nn9_str(++nomer_str,0,naim.ravno(),ei.ravno(),kolih,cena,bb,"",0,kod_usl.ravno(),"",f1,wpredok);
  itogo+=bb;


 }
fprintf(f1,"ICEB_LST_END\n");

//Печатаем концовку налоговой накладной
iceb_nn9_end(tipz,lnds,"uslnast.alx",kodop.ravno(),kpos.ravno(),f1,wpredok);

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
iceb_nn9_start_s(imaf,lnds,suma_dok,suma_nds,0,wpredok);

iceb_ustpeh(imaf,3,&kolstr,wpredok);

iceb_rnl(imaf,kolstr,"",&iceb_nn9_hap,wpredok);

return(0);
}
