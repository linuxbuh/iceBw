/*$Id:$*/
/*22.03.2017	13.10.2006	Белых А.И.	xml_nn_uosw.c
Выгрузка в xml файл налоговой накладной из "Учёта основных средств"
*/
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;
extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int xml_nn_uosw(int *pnd, //порядковый номер документа
int tipz, //1-приходный документ 2-расходный
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
char strsql[1024];
SQL_str row,row1;
class SQLCURSOR cur,cur1;
double procent_nds=0.;
class iceb_u_str kontr("");
int podr=0;
class iceb_u_str kodop("");
class iceb_u_str kod_stavki("");
class iceb_u_str kod_lgoti("");
class iceb_u_str kontr00("00");

//читаем шапку документа
sprintf(strsql,"select kontr,podr,forop,datpnn,datop,nds,pn,kodop from Uosdok where datd='%04d-%02d-%02d' and nomd='%s' and nomnn='%s' and tipz=%d",
gd,md,dd,nomdok,nom_nal_nak,tipz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {

  class iceb_u_str repl;
  repl.plus(gettext("Учёт основных средств"));
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

podr=atoi(row[1]);
kontr.new_plus(row[0]);
int lnds=atoi(row[5]);
kod_stavki.new_plus(iceb_get_kodstavki(lnds));

class iceb_u_str kod_forop(row[2]);

char datop[64];
memset(datop,'\0',sizeof(datop));
short dop,mop,gop;
iceb_u_rsdat(&dop,&mop,&gop,row[4],2);
if(dop != 0)
 if(iceb_u_sravmydat(dop,mop,gop,dd,md,gd) < 0)
  sprintf(datop,"%02d%02d%d",dop,mop,gop);
if(datop[0] == '\0')
  sprintf(datop,"%02d%02d%d",dd,md,gd);



short dnp,mnp,gnp;
iceb_u_rsdat(&dnp,&mnp,&gnp,row[3],2);


procent_nds=atof(row[6]);
kodop.new_plus(row[7]);

class iceb_u_str forma_rash("");

if(kod_forop.ravno()[0] != '\0')
 {
  //читаем наименование формы оплаты
  sprintf(strsql,"select naik from Foroplat where kod='%s'",kod_forop.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   forma_rash.new_plus(row[0]);
 } 

if(tipz == 2)
 {
  sprintf(strsql,"select kk from Uosras where kod='%s'",kodop.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   kontr00.new_plus(row[0]);
 }

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
iceb_nn10_zag_xml(tipz,nom_nal_nak,dnp,mnp,gnp,dd,md,gd,kontr00.ravno(),kontr.ravno(),vid_dok,ff_xml,wpredok);


int kolstr=0;
//читаем записи в документе
sprintf(strsql,"select innom,bs,bsby,cena from Uosdok1 where datd='%04d-%02d-%02d' and \
podr=%d and nomd='%s'",gd,md,dd,podr,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

int nomer=0;
class iceb_u_str naim_mat("");
double kolih;
double cena;
double suma;
double isuma_bnds=0.;
double bs=0.;
double bsby=0.;
int nomer_str=0;
double spnv=0.;

while(cur.read_cursor(&row) != 0)
 {
  nomer++;

  sprintf(strsql,"select naim from Uosin where innom=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
      naim_mat.new_plus(row1[0]);
  else
      naim_mat.new_plus("");
  

  bs=atof(row[1]);
  bsby=atof(row[2]);
  kolih=1.;

  if(tipz == 2)
    cena=atof(row[3]);
  else
   {
    if(bs != 0.)
     cena=bs;
    else
     cena=bsby;
   }

  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrg1);


  isuma_bnds+=suma;
  spnv=suma*procent_nds/100.;    
  //вывод строки в файл
  iceb_nn10_str_xml(++nomer_str,kolih,cena,suma,row[3],naim_mat.ravno(),row1[0],kod_stavki.ravno(),kod_lgoti.ravno(),0,"","",spnv,ff_xml,wpredok);


}

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

//концовка файла
iceb_nn10_end_xml(kodop.ravno(),"uosnast.alx",lnds,suma_dok,suma_nds,0,ff_xml,wpredok);


fclose(ff_xml);

return(0);


 }
