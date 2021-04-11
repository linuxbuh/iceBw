/*$Id:$*/
/*06.12.2018	16.10.2006	Белых А.И.	xml_nn_ukrw.c
Выгрузка налоговых накладных для подсистемы "Учёт командировочных расходов".
*/
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int xml_nn_ukrw(int *pnd, //порядковый номер документа
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,
int period_type,
const char *vid_dok,
class iceb_u_str *imafil_xml,
GtkWidget *wpredok)
{
int tipz=1; //1-приходный документ 2-расходный

char strsql[512];
SQL_str row;
class SQLCURSOR cur,cur1;
double procent_nds=0.;
class iceb_u_str kontr("");

//может быть в документе несколько записей с одинаковым номером налоговой накладной - используем цену для однозначного определения
//Читаем запись в документе
sprintf(strsql,"select kodr,kolih,cena,ediz,snds,kdrnn from Ukrdok1 where datd='%04d-%02d-%02d' and \
ndrnn='%s' and nomd='%s'",
gd,md,dd,nom_nal_nak,nomdok);

//читаем шапку документа
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {

  class iceb_u_str repl;
  repl.plus(gettext("Учёт командировочных расходов"));
  repl.ps_plus(gettext("Ненайден документ"));
  repl.plus("!");
  

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%s",
  gettext("Дата"),
  dd,md,gd,
  gettext("Номер"),
  nomdok,
  gettext("Налоговый номер"),
  nom_nal_nak);

  repl.ps_plus(strsql);
  
  iceb_menu_soob(&repl,wpredok);
  
  return(1);
 }

class iceb_u_str ei(row[3]);

kontr.new_plus(row[5]);
double suma_nds_v=atof(row[4]);
int lnds=0;
if(suma_nds_v == 0.)
 lnds=3;
 
class iceb_u_str eiz(row[3]);

double kolih=atof(row[1]);
double cena=atof(row[2]);
double suma=cena;
double isuma_bnds=suma;
double spnv=0.;

char datop[64];
memset(datop,'\0',sizeof(datop));
sprintf(datop,"%02d%02d%04d",dd,md,gd);



short dnp=dd,mnp=md,gnp=gd;


procent_nds=iceb_pnds(dd,md,gd,wpredok);


//читаем наименование
class iceb_u_str naim_mat("");
sprintf(strsql,"select naim from Ukrkras where kod=%s",row[0]);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_mat.new_plus(row[0]);


FILE *ff_xml;
char imaf_xml[512];
//char imaf[64]; 

*pnd+=1;

//открываем файл
if(iceb_open_fil_nn_xml(imaf_xml,*pnd,tipz,mn,gn,&rek_zag_nn,&ff_xml,wpredok) != 0)
 return(1);

imafil_xml->new_plus(imaf_xml);

//Заголовок файла
iceb_rnn_sap_xml(mn,gn,mk,iceb_getkoddok(1,mn,gn,1),*pnd,period_type,&rek_zag_nn,ff_xml);

//Заголовок документа
iceb_nn10_zag_xml(tipz,nom_nal_nak,dnp,mnp,gnp,dd,md,gd,"00",kontr.ravno(),vid_dok,ff_xml,wpredok);






class iceb_u_str kod_stavki(iceb_get_kodstavki(lnds));
class iceb_u_str kod_lgoti("");

//вывод строки в файл
iceb_nn10_str_xml(1,kolih,cena,suma,ei.ravno(),naim_mat.ravno(),"",kod_stavki.ravno(),kod_lgoti.ravno(),0,"","",spnv,ff_xml,wpredok);

double suma_dok=0;
double suma_nds=0;

/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(isuma_bnds)*procent_nds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(isuma_bnds)*procent_nds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }


/*Загальна сума з ПДВ*/
suma_dok=isuma_bnds+suma_nds;
spnv=suma_nds;

//концовка файла
iceb_nn10_end_xml("","",lnds,suma_dok,suma_nds,0,ff_xml,wpredok);


fclose(ff_xml);

return(0);
}
