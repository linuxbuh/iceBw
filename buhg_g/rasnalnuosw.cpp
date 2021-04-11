/*$Id:$ */
/*07.12.2018    22.01.1994      Белых А.И.      rasnalnuosw.c
Распечатка налоговых накладных для Учета основных средств
*/
#include	"buhg_g.h"
int rasnalnuosw9(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok);

extern double          okrcn;  /*Округление цены*/
extern double okrg1;
extern SQL_baza bd;

int rasnalnuosw(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok)
{
/********



if(iceb_u_sravmydat(data_dok,"1.1.2015") >= 0 && iceb_u_sravmydat(data_dok,"1.4.2016") < 0)
 return(rasnalnuosw7(data_dok,podr,nomdok,imaf,wpredok));

if(iceb_u_sravmydat(data_dok,"1.4.2016") >= 0 )
***********************/
// return(rasnalnuosw8(data_dok,podr,nomdok,imaf,wpredok));
 return(rasnalnuosw9(data_dok,podr,nomdok,imaf,wpredok));

return(1);
}

/***********************************************************************/
int rasnalnuosw9(const char *data_dok,int podr,const char *nomdok,char *imaf,GtkWidget *wpredok)
{
short dd,md,gd;
SQL_str         row,row1;
SQLCURSOR curr;
int		kolstr;
char		strsql[1024];
FILE            *f1;
short           i;
double          itogo;
class iceb_u_str kpos("");
short		tipz;
class iceb_u_str nomnn(""); /*Номер налоговой накладной*/
class iceb_u_str naim("");
double		cena;
class iceb_u_str ei("");
short		dnn,mnn,gnn; //Дата налоговой накладной
class iceb_u_str kodop("");
float pnds=0.;
short lnds=0;

iceb_u_rsdat(&dd,&md,&gd,data_dok,1);

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,tipz,osnov,forop,nomnn,datpnn,kodop,nds,pn \
from Uosdok where datd='%04d-%02d-%02d' and podr=%d and nomd='%s'",
gd,md,dd,podr,nomdok);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"Не найден документ! Дата:%d.%d.%d Документ:%s Подразделение:%d",
  dd,md,gd,nomdok,podr);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

kpos.new_plus(row[0]);
tipz=atoi(row[1]);
nomnn.new_plus(row[4]);
iceb_u_rsdat(&dnn,&mnn,&gnn,row[5],2);
kodop.new_plus(row[6]);
lnds=atoi(row[7]);
pnds=atof(row[8]);


class iceb_u_str kodkontr00("00");
if(tipz == 2)
  kodkontr00.new_plus(iceb_getk00(2,kodop.ravno(),wpredok));

if(kodkontr00.getdlinna() <= 1)
 kodkontr00.new_plus("00");
 
//Печатаем шапку документа
if(iceb_nn9_start(imaf,dd,md,gd,dnn,mnn,gnn,nomnn.ravno(),kpos.ravno(),tipz,kodkontr00.ravno(),&f1,wpredok) != 0)
 return(1);
iceb_insfil("nn9_hap.alx",f1,wpredok);


sprintf(strsql,"select innom,cena from Uosdok1 \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tipz=%d \
order by innom asc",gd,md,dd,podr,nomdok,tipz);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }


itogo=0.;
i=0;
ei.new_plus(gettext("Шт"));
int nomer_str=0;
while(cur.read_cursor(&row) != 0)
 {
  i++;

  cena=atof(row[1]);
  cena=iceb_u_okrug(cena,okrcn);

  //Читаем наименование
  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(sql_readkey(&bd,strsql,&row1,&curr) > 0)
    naim.new_plus(row1[0]);
  else
    naim.new_plus("");

  /*печатаем строку*/
  iceb_nn9_str(++nomer_str,0,naim.ravno(),ei.ravno(),1,cena,cena,"",0,"","",f1,wpredok);
  itogo+=cena;


 }
fprintf(f1,"ICEB_LST_END\n");

//Печатаем концовку налоговой накладной
iceb_nn9_end(tipz,lnds,"uosnast.alx",kodop.ravno(),kpos.ravno(),f1,wpredok);

double suma_dok=0;
double suma_nds=0;

/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(itogo)*pnds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(itogo)*pnds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }


/*Загальна сума з ПДВ*/
suma_dok=itogo+suma_nds;
/*записываем в шапку суммы*/
iceb_nn9_start_s(imaf,lnds,suma_dok,suma_nds,0,wpredok);

iceb_ustpeh(imaf,3,&kolstr,wpredok);

iceb_rnl(imaf,kolstr,"",&iceb_nn9_hap,wpredok);

return(0);
}

