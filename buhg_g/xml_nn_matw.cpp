/*$Id:$*/
/*06.12.2018	12.10.2006	Белых А.И.	xml_nn_matw.c
Выгруз в файл налоговой накладной в xml формате из подсистемы "Материальный учёт".
Если вернули 0- документ найден
             1- не найден
*/
#include "buhg_g.h"

extern class iceb_rnfxml_data rek_zag_nn;

extern double   okrg1; /*Округление*/
extern double	okrcn;  /*Округление цены*/
extern SQL_baza bd;

int xml_nn_matw(int *pnd, //порядковый номер документа
int tipz, //1-приходный документ 2-расходный
short dd,short md,short gd, //Дата документа
const char *nomdok,                //Номер документа
const char *nom_nal_nak,           //Номер налоговой накладной
short mn,short gn,          //Дата начала поиска
short mk,                   //Месяц конца периода
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
int sklad=0;
short dnp=0,mnp=0,gnp=0; //Дата получения/выписки налоговой накладной
class iceb_u_str kodop("");
class iceb_u_str kod_stavki("");
class iceb_u_str kod_lgoti("");
class iceb_u_str kontr00("00");

//читаем шапку документа
sprintf(strsql,"select sklad,kontr,pn,kodop,datpnn from Dokummat where datd='%04d-%02d-%02d' and nomd='%s' and nomnn='%s' and tip=%d",
gd,md,dd,nomdok,nom_nal_nak,tipz);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  class iceb_u_str repl;
  repl.plus(gettext("Материальный учёт"));
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

sklad=atoi(row[0]);
kontr.new_plus(row[1]);
procent_nds=atof(row[2]);
kodop.new_plus(row[3]);
iceb_u_rsdat(&dnp,&mnp,&gnp,row[4],2);

if(tipz == 2)
 {
  sprintf(strsql,"select kk from Rashod where kod='%s'",kodop.ravno_filtr());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   kontr00.new_plus(row[0]);
 }

class iceb_u_str uslov_prod("");
class iceb_u_str forma_rash("");


class iceb_u_str kod_form_opl("");

int lnds=0;
double sumkor=0.;

char		datop[64];
memset(datop,'\0',sizeof(datop));

int kolstr=0;

sprintf(strsql,"select nomerz,sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d",
gd,nomdok,sklad);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

double suma_nds_v=0.;//Сумма ндс введённая вручную для приходных документов
int nomer=0;
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  nomer=atoi(row[0]);

  if(nomer == 6)
   if(tipz == 1)
    suma_nds_v=atof(row[1]);
  if(nomer == 7)
     uslov_prod.new_plus(row[1]);
  if(nomer == 8)
    kod_form_opl.new_plus(row[1]);
  if(nomer == 9)
   {
    short d,m,g;
    iceb_u_rsdat(&d,&m,&g,row[1],0);
    if(d != 0 && iceb_u_sravmydat(d,m,g,dd,md,gd) < 0)
      sprintf(datop,"%02d%02d%04d",d,m,g);
   }
  if(nomer == 11)
    lnds=atoi(row[1]);

  if(nomer == 13)
   sumkor=atof(row[1]);  
 }
kod_stavki.new_plus(iceb_get_kodstavki(lnds));

if(datop[0] == '\0')
  sprintf(datop,"%02d%02d%04d",dnp,mnp,gnp);
 
if(kod_form_opl.ravno()[0] != '\0')
 {
  //читаем наименование формы оплаты
  sprintf(strsql,"select naik from Foroplat where kod='%s'",kod_form_opl.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   forma_rash.new_plus(row[0]);
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


//читаем записи в документе
sprintf(strsql,"select kodm,kolih,cena,ei,dnaim,voztar,mi,kdstv from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,sklad,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
nomer=0;
class iceb_u_str naim_mat("");
double kolih;
double cena;
double suma;
double isuma_bnds=0.;
double suma_voz_tar=0.;
int kod_grup=0;
int nomer_str=0;
int metka_imp_tov=0;
class iceb_u_str kdstv("");
double spnv=0.;

while(cur.read_cursor(&row) != 0)
 {
  nomer++;


  kod_grup=0;
  sprintf(strsql,"select kodgr,naimat from Material where kodm=%s",row[0]);
  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
   {
    kod_grup=atoi(row1[0]);
    naim_mat.new_plus(row1[1]);
    if(row[4][0] != '\0')
     {
      naim_mat.plus(" ",row[4]);
     }
   }
  else
   naim_mat.new_plus("");


  metka_imp_tov=atoi(row[6]);
  kdstv.new_plus(row[7]);
  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrg1);
  spnv=suma*procent_nds/100.;
  
  if(kod_grup == rek_zag_nn.kod_gr_voz_tar || atoi(row[5]) == 1)
   {
    suma_voz_tar+=suma;
    continue;
   }

  isuma_bnds+=suma;
  
  //вывод строки в файл
  iceb_nn10_str_xml(++nomer_str,kolih,cena,suma,row[3],naim_mat.ravno(),row1[2],kod_stavki.ravno(),kod_lgoti.ravno(),metka_imp_tov,"",kdstv.ravno(),spnv,ff_xml,wpredok);
   

 }


sprintf(strsql,"select ei,kolih,cena,naimu,ku from Dokummat3 where \
datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,sklad,nomdok);

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }
class iceb_u_str kod_usl("");

while(cur.read_cursor(&row) != 0)
 {
  nomer++;
  kolih=atof(row[1]);
  cena=atof(row[2]);
  cena=iceb_u_okrug(cena,okrcn);

  if(kolih > 0)
    suma=cena*kolih;
  else
    suma=cena;

  suma=iceb_u_okrug(suma,okrg1);

  isuma_bnds+=suma;
  spnv=suma*procent_nds/100.;
  
  if(atoi(row[4]) != 0)
   {
    sprintf(strsql,"select naius,ku from Uslugi where kodus=%s",row[4]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     {
      kod_usl.new_plus(row1[1]);
      naim_mat.new_plus(row1[0]);
      if(row[3][0] != '\0')
       {
        naim_mat.plus(" ",row[4]);
       }
     }
    else
     {
      naim_mat.new_plus("");
      kod_usl.new_plus("");
     }
   }
  //вывод строки в файл
  iceb_nn10_str_xml(++nomer_str,kolih,cena,suma,row[3],naim_mat.ravno(),"",kod_stavki.ravno(),kod_lgoti.ravno(),0,kod_usl.ravno(),"",spnv,ff_xml,wpredok);

 }
double suma_dok=0;
double suma_nds=0;

/*НДС*/
if(lnds == 0) //20% НДС
 {
  suma_nds=(isuma_bnds+sumkor)*procent_nds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }

if(lnds == 4) //7% НДС
 {
  suma_nds=(isuma_bnds+sumkor)*procent_nds/100.;
  suma_nds=iceb_u_okrug(suma_nds,okrg1);

 }
if(tipz == 1 && suma_nds_v != 0.)
 suma_nds=suma_nds_v;

/*Загальна сума з ПДВ*/
suma_dok=isuma_bnds+sumkor+suma_nds;

//концовка файла
iceb_nn10_end_xml(kodop.ravno(),"matnast.alx",lnds,suma_dok,suma_nds,suma_voz_tar,ff_xml,wpredok);


fclose(ff_xml);

return(0);

}

