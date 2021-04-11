/* $Id: rasdokw.c,v 1.44 2013/11/05 10:51:25 sasa Exp $ */
/*21.01.2020    15.04.1997      Белых А.И.      rasdokw.c
Распечатка накладных, счета, Акта приемки
*/
#define		KLST1 50
#define		KLST2 67
#include <math.h>
#include        <errno.h>
#include	"buhg_g.h"

void 		sapnak(short,FILE*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short	vtara; /*Код группы возвратная тара*/
extern short    obzap; /*0-не объединять записи 1-обединять*/
extern nl_catd	fils; /*Указатель на базу сообщений*/
extern SQL_baza  bd;
class iceb_u_spisok sp_fio_klad; /*выбранные фамилии кладовщиков по складам*/
class iceb_u_int sp_kod_sklad; /*список кодов складов для фамилий кладовщиков*/


void rasdokw(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
const char *imaf,   // имя файла
//short as,  //1-есть акцизный сбор 2-нет*/
short lnds,  //Льготы по НДС*
short vr,  //0-накладная 1-счет 2-акт приемки 3-акт списания*/
FILE *f1,double ves, //Вес
GtkWidget *wpredok)
{
class iceb_u_str nmo(""),nmo1("");  /*Наименование организации*/
class iceb_u_str gor(""),gor1(""); /*Адрес*/
class iceb_u_str pkod(""),pkod1(""); /*Почтовый код*/
class iceb_u_str nmb(""),nmb1("");  /*Наименование банка*/
class iceb_u_str mfo(""),mfo1("");  /*МФО*/
class iceb_u_str nsh(""),nsh1("");  /*Номер счета*/
class iceb_u_str inn(""),inn1("");  /*Индивидуальный налоговый номер*/
class iceb_u_str grb(""),grb1("");  /*Город банка*/
class iceb_u_str npnds(""),npnds1(""); /*Номер плательщика НДС*/
class iceb_u_str tel(""),tel1("");    /*Номер телефона*/
class iceb_u_str gor_kontr("");
class iceb_u_str gor_kontr1("");
class iceb_u_str regnom(""); /*Регистрационный номер частного предпринимателя*/
char            bros1[1024];
int             i,i1;
class iceb_u_str nomn("");  /*Номенклатурный номер*/
double          itogo;
double          itogo1;
double          bb=0.,bb1=0.;
short           d,m,g;
short           dd,md,gd;
short           nnds=0; //0-накладная 1-накладная с ценой НДС 2-накладная с номером заказа 
short		mnt; /*Метка наличия тары*/
double		sumt; /*Сумма по таре*/
class iceb_u_str kdtr(""); /*Коды тары*/
short		mtpr; /*Метка первого и второго прохода*/
short		mtar=0; /*Метка наличия oбычной тары в накладной*/
short		mppu; /*Метка первого прохода услуг*/
double		mest=0.;
class iceb_u_str naimpr("");
int		skl1;
short		kor,ots;
short           mvnpp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
int             kodm,kodmz,nk;
double		kol,sum;
double		kolt;
double		kratn,kratz,cenan,cenaz;
class iceb_u_str ein(""),eiz("");
class iceb_u_str naim(""),naimz("");
short		klst,klli;
//double		sumkor;
SQL_str         row,row1,rowtmp;
char		strsql[2048];
long		kolstr;
class iceb_u_str kpos("");
class iceb_u_str kop(""); /*Код операции*/
int		tipz=0;
class iceb_u_str naiskl("");
class iceb_u_str nomnn(""); /*Номер налоговой накладной*/
class iceb_u_str nn1(""); /*Номер встречного документа*/
char		bros[1024];
char		str[1024];
short		vtar; /*Метка возвратной тары*/
short		kgrm; /*код группы материала*/
class iceb_u_str shu(""); /*счет учета материала*/
double		kolih,cena;
class iceb_u_str fmoll(""); //Фамилия материально-ответственного
class iceb_u_str nomz("");
class iceb_u_str nomzz("");
short		metkadov=0; //0- доверенность печатается только если реквизит введён 1- всегда
char		mesqc[64];
class iceb_u_str kodkontr00("00");
short           metka_close=0;
SQLCURSOR cur,cur1,curtmp;
double itogo_mest=0.;
mest=mnt=nnds=0;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s-%s",__FUNCTION__,gettext("Не найден документ !")),
  repl.plus(strsql);
  repl.ps_plus(strsql);
  
  sprintf(strsql,"%s %d.%d.%d %s %s %s %d",
  gettext("Дата"),dg,mg,gg,
  gettext("Документ"),nomdok,
  gettext("Слад"),skl);
  
  repl.ps_plus(strsql);
  iceb_menu_soob(&repl,wpredok);
  
  return;
 }
float pnds=atof(row[13]);
dd=dg;
md=mg;
gd=gg;
kpos.new_plus(row[3]);
kop.new_plus(row[6]);
tipz=atoi(row[0]);
nomnn.new_plus(row[5]);
nn1.new_plus(row[11]);
kodkontr00.new_plus(row[15]);
/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Prihod");
if(tipz == 2)
 strcpy(bros,"Rashod");

mvnpp=0;

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код операции"));
  repl.plus(" ");
  repl.plus(kop.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {

  mvnpp=atoi(row[1]);
  naimpr.new_plus(row[0]);
  
 }

kor=0;
memset(bros,'\0',sizeof(bros)); 
iceb_poldan("Корректор межстрочного растояния",bros,"matnast.alx",wpredok);
kor=(short)iceb_u_atof(bros);

iceb_poldan("Печатать строку для ввода доверенности",bros,"matnast.alx",wpredok);
if(iceb_u_SRAV(bros,"Вкл",1) == 0)
  metkadov=1;

class iceb_u_str nadpis("");
iceb_poldan("Надпись на расходной накладной",&nadpis,"matnast.alx",wpredok);
  
ots=0;
memset(bros,'\0',sizeof(bros)); 
iceb_poldan("Отступ от левого края",bros,"matnast.alx",wpredok);
ots=(short)iceb_u_atof(bros);

nnds=0;
memset(bros,'\0',sizeof(bros));
iceb_poldan("Код операции торговли",bros,"matnast.alx",wpredok);
if(iceb_u_proverka(bros,kop.ravno(),0,1) == 0)
 nnds=1;
else
 {
  if(tipz == 2)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_poldan("Форма документов с номером заказа",bros,"matnast.alx",wpredok);
    if(iceb_u_SRAV(bros,"Вкл",1) == 0)
     nnds=2;
   }
 }

iceb_poldan("Код тары",&kdtr,"matnast.alx",wpredok);


if(tipz == 2)
 {
  if(iceb_u_SRAV("00",kodkontr00.ravno(),0) == 0)
    kodkontr00.new_plus(iceb_getk00(0,kop.ravno(),wpredok));
 }

/*Читаем реквизиты организации свои */


sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(sql_readkey(&bd,bros,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",
  gettext("Не найден код контрагента"),kodkontr00.ravno());
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  if(row[16][0] != '\0')
   nmo.new_plus(row[16]);
  else
   if(iceb_u_polen(row[1],&nmo,1,'(') != 0)
     nmo.new_plus(row[1]);
  gor.new_plus(row[3]);
  pkod.new_plus(row[5]);
  nmb.new_plus(row[2]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
  inn.new_plus(row[8]);
  grb.new_plus(row[4]);
  npnds.new_plus(row[9]);
  tel.new_plus(row[10]);
  gor_kontr.new_plus(row[17]);
 } 


/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(kpos.ravno_atof() != 0. || kpos.ravno()[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kpos.ravno());
  if(sql_readkey(&bd,bros,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найден код контрагента"),kpos.ravno());
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    if(row[16][0] != '\0')
     nmo1.new_plus(row[16]);
    else
      nmo1.new_plus(row[1]);
    gor1.new_plus(row[3]);
    pkod1.new_plus(row[5]);
    nmb1.new_plus(row[2]);
    mfo1.new_plus(row[6]);
    nsh1.new_plus(row[7]);
    inn1.new_plus(row[8]);
    grb1.new_plus(row[4]);
    npnds1.new_plus(row[9]);
    tel1.new_plus(row[10]);
    regnom.new_plus(row[15]);
    gor_kontr1.new_plus(row[17]);
    
   }
 }

class iceb_tmptab tabtmp;
const char *imatmptab={__FUNCTION__};

if(sortdokmuw(dg,mg,gg,skl,nomdok,imatmptab,&tabtmp,NULL,wpredok) != 0)
 return;


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

//sortnakw(dg,mg,gg,skl,nomdok,wpredok);


if(f1 == NULL)
 {
  printf("%s-открываем файл\n",__FUNCTION__);
  if((f1 = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return;
   }
  metka_close=1;
 }

if(ots != 0)
 fprintf(f1,"\x1b\x6C%c",ots); /*Установка левого поля*/
fprintf(f1,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал*/
fprintf(f1,"\x12"); /*Отмена ужатого режима*/
fprintf(f1,"\x1B\x4D"); /*12-знаков*/
if(vr == 1)
  fprintf(f1,"\x1B\x45"); /*Включение режима выделенной печати*/


if(vr == 1)
 {
  if(nadpis.ravno()[0] != '\0' && tipz == 2 && mvnpp == 0)
   fprintf(f1,"%s\n",nadpis.ravno());
  if(tipz == 2)
    fprintf(f1,gettext("\
    Поставщик                             Плательщик\n"));
  if(tipz == 1)
    fprintf(f1,gettext("\
    Плательщик                            Поставщик\n"));

  fprintf(f1,"%-*.*s N%s %-*.*s\n",
  iceb_u_kolbait(40,nmo.ravno()),iceb_u_kolbait(40,nmo.ravno()),nmo.ravno(),
  kpos.ravno(),
  iceb_u_kolbait(40,nmo1.ravno()),iceb_u_kolbait(40,nmo1.ravno()),nmo1.ravno());

  for(int nom=40; nom < iceb_u_strlen(nmo.ravno()) || nom < iceb_u_strlen(nmo1.ravno()); nom+=40)  
   {
    if(nom < iceb_u_strlen(nmo.ravno()))
     {  
      fprintf(f1,"%-*.*s ",
      iceb_u_kolbait(40,iceb_u_adrsimv(nom,nmo.ravno())),
      iceb_u_kolbait(40,iceb_u_adrsimv(nom,nmo.ravno())),
      iceb_u_adrsimv(nom,nmo.ravno()));
     }
    else
      fprintf(f1,"%40s ","");
      
    if(nom < iceb_u_strlen(nmo1.ravno()))
     {
      fprintf(f1,"%-*.*s",
      iceb_u_kolbait(40,iceb_u_adrsimv(nom,nmo1.ravno())),
      iceb_u_kolbait(40,iceb_u_adrsimv(nom,nmo1.ravno())),
      iceb_u_adrsimv(nom,nmo1.ravno()));
     }
    fprintf(f1,"\n");
   }   
   
 }
if(vr == 0 || vr == 2)
 {

  if(nadpis.ravno()[0] != '\0' && tipz == 2 && mvnpp == 0)
   fprintf(f1,"%s\n",nadpis.ravno());
  if(tipz == 1)
   fprintf(f1,gettext("\
    Кому                                 От кого\n"));
  if(tipz == 2)
   fprintf(f1,gettext("\
     Поставщик                           Плательщик\n"));

  fprintf(f1,"%-*.*s ",iceb_u_kolbait(40,nmo.ravno()),iceb_u_kolbait(40,nmo.ravno()),nmo.ravno());
  fprintf(f1,"\x1B\x45N%s %-*.*s\x1B\x46\n",kpos.ravno(),iceb_u_kolbait(40,nmo1.ravno()),iceb_u_kolbait(40,nmo1.ravno()),nmo1.ravno());

  for(int nom=40; nom < iceb_u_strlen(nmo.ravno()) || nom < iceb_u_strlen(nmo1.ravno()); nom+=40)
   {
    if(iceb_u_strlen(nmo.ravno()) > nom)
     fprintf(f1,"%-*.*s ",
     iceb_u_kolbait(40,iceb_u_adrsimv(nom,nmo.ravno())),
     iceb_u_kolbait(40,iceb_u_adrsimv(nom,nmo.ravno())),
     iceb_u_adrsimv(nom,nmo.ravno()));
    else
     fprintf(f1,"%40s ","");
     
    if(iceb_u_strlen(nmo1.ravno()) > nom)
      fprintf(f1,"\x1B\x45%-.*s\x1B\x46",
      iceb_u_kolbait(40,iceb_u_adrsimv(nom,nmo1.ravno())),
      iceb_u_adrsimv(nom,nmo1.ravno()));

    fprintf(f1,"\n");
   }

 }
if(vr == 3)
 {
  fprintf(f1,"%s\n",nmo.ravno());
 }

memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));
sprintf(bros,"%s %s",gettext("Адрес"),gor.ravno());
sprintf(bros1,"%s %s",gettext("Адрес"),gor1.ravno());

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 {
  fprintf(f1,"%-*.*s %-*.*s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,iceb_u_kolbait(40,bros1),iceb_u_kolbait(40,bros1),bros1);

  if(iceb_u_strlen(bros) > 40 || iceb_u_strlen(bros1) > 40)
   fprintf(f1,"%-*.*s %-*.*s\n",
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros)),
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros)),
   iceb_u_adrsimv(40,bros),
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros1)),
   iceb_u_kolbait(40,iceb_u_adrsimv(40,bros1)),
   iceb_u_adrsimv(40,bros1));
  
 }
sprintf(bros,"%s %s",gettext("Код ЕГРПОУ"),pkod.ravno());
sprintf(bros1,"%s %s",gettext("Код ЕГРПОУ"),pkod1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);
/*****************************
sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh.ravno(),
gettext("МФО"),mfo.ravno());

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1.ravno(),
gettext("МФО"),mfo1.ravno());
********************/
sprintf(bros,"%s %s",gettext("Р/С"),nsh.ravno());

sprintf(bros1,"%s %s",gettext("Р/С"),nsh1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",gettext("в"),nmb.ravno());
sprintf(bros1,"%s %s",gettext("в"),nmb1.ravno());

if(grb.getdlinna() > 1)
 {
  sprintf(strsql," %s",grb.ravno());
  strcat(bros,strsql);
 }

if(grb1.getdlinna() > 1)
 {
  sprintf(strsql," %s",grb1.ravno());
  strcat(bros1,strsql);
 }
 
//sprintf(bros,"%s %s %s %s",gettext("в"),nmb.ravno(),gettext("гор."),grb.ravno());
//sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1.ravno(),gettext("гор."),grb1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
 {
  fprintf(f1,"%-*.*s %s\n",iceb_u_kolbait(40,bros),iceb_u_kolbait(40,bros),bros,bros1);
  if(iceb_u_strlen(bros) > 40)
   fprintf(f1,"%s\n",iceb_u_adrsimv(40,bros));
 }  
sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn.ravno());

sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1.ravno());

if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
   fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds.ravno());
sprintf(bros1,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
   fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

sprintf(bros,"%s %s",gettext("Телефон"),tel.ravno());
sprintf(bros1,"%s %s",gettext("Телефон"),tel1.ravno());
if(vr == 3)
  fprintf(f1,"%s\n",bros);
else  
   fprintf(f1,"%-*s %s\n",iceb_u_kolbait(40,bros),bros,bros1);

if(nnds == 2)
  fprintf(f1,"\
──────────────────────────────────────────────────────────────────────────────────\n");

fprintf(f1,"\x1B\x50"); /*10-знаков*/



/*Читаем наименование склада*/
static class iceb_u_spisok sp_fio_klad; /*выбранные фамилии кладовщиков по складам*/
static class iceb_u_int sp_kod_sklad; /*список кодов складов для фамилий кладовщиков*/
class iceb_u_spisok fio_klad;
class iceb_u_str fio_klad_vibr("");

sprintf(strsql,"select naik,fmol,np from Sklad where kod=%d",skl);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  repl.plus(gettext("Не найден код склада"));
  repl.plus(" ");
  repl.plus((int)skl);
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
 }
else
 {
  naiskl.new_plus(row[0]);
  fmoll.new_plus(row[1]);
  if(row[2][0] != '\0')
   gor_kontr.new_plus(row[2]);

  int nom_skl=0;
  if((nom_skl=sp_kod_sklad.find(skl)) < 0)
   {
    iceb_u_strspisok(row[1],'/',&fio_klad); /*составляем список фамилий кладовщиков*/

    if(fio_klad.kolih() == 1)
     {
      fio_klad_vibr.new_plus(fio_klad.ravno(0));
     }
    if(fio_klad.kolih() > 1)
     {
      int vozv=0;
      if((vozv=iceb_menu_mv(gettext("Выбор"),gettext("Выберите фамилию кладовщика"),&fio_klad,0,wpredok)) >= 0)
        fio_klad_vibr.new_plus(fio_klad.ravno(vozv));
       
     }

    sp_kod_sklad.plus(skl);
    sp_fio_klad.plus(fio_klad_vibr.ravno());
   
   }
  else
    fio_klad_vibr.new_plus(sp_fio_klad.ravno(nom_skl));
 }
if(vr == 0 || vr == 2 || vr == 3)
  fprintf(f1,"\n%s",naiskl.ravno());

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(kpos.ravno_atof() == 0. && iceb_u_pole(kpos.ravno(),bros,2,'-') == 0 && kpos.ravno()[0] == '0')
   {
    iceb_u_pole(kpos.ravno(),bros,2,'-');
    skl1=(int)iceb_u_atof(bros);
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
    skl1);
    naiskl.new_plus("");
    class iceb_u_str fmol("");
    if(sql_readkey(&bd,strsql,&row,&cur) != 1)
     {
      iceb_u_str repl;
      repl.plus(gettext("Не найден код склада"));
      repl.plus(" ");
      repl.plus((int)skl1);
      repl.plus(" !");
      iceb_menu_soob(&repl,wpredok);  
     }
    else
     {
      naiskl.new_plus(row[0]);
      fmol.new_plus(row[1]);
     }
    if(tipz == 1)
       fprintf(f1,"\n\
%s: %s %d <= %s %d %s\n\
%22s %s <= %s\n",
       naimpr.ravno(),gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl.ravno()," ",fmoll.ravno(),fmol.ravno());
    if(tipz == 2)
       fprintf(f1,"\n\
%s: %s %d => %s %d %s\n\
%22s %s => %s\n",
       naimpr.ravno(),gettext("Склад"),skl,
       gettext("Склад"),skl1,naiskl.ravno()," ",fmoll.ravno(),fmol.ravno());
   }
  else
    fprintf(f1,"\n%s",naimpr.ravno());
 }
else
 { 
  bros[0]='\0';

  if(iceb_poldan("Наименование операции в накладной",bros,"matnast.alx",wpredok) == 0)
   if(iceb_u_SRAV(bros,"Вкл",1) == 0)
    fprintf(f1,"\n%s",naimpr.ravno());
   
 }

if(nnds != 2)
  fprintf(f1,"\n");

sprintf(bros,"N%s",nomdok);  
if(tipz == 2 && iceb_poldan("Перенос символа N",bros,"matnast.alx",wpredok) == 0)
 {
  if(iceb_u_SRAV(bros,"Вкл",1) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_pole(nomdok,bros,1,'-');
    iceb_u_pole(nomdok,bros1,2,'-');
    strcat(bros,"-N");  
    strcat(bros,bros1);  
    
   }       
  else
    sprintf(bros,"N%s",nomdok);  

 } 
memset(mesqc,'\0',sizeof(mesqc));
iceb_mesc(md,1,mesqc);

if(tipz == 1)
 fprintf(f1,"\n%s\n",gor_kontr1.ravno());
if(tipz == 2) 
 fprintf(f1,"\n%s\n",gor_kontr.ravno());

if(vr == 0)
 {
  if(tipz == 1)
    fprintf(f1,"\
                     %s  %s\n",
    gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros);
  if(tipz == 2)
    fprintf(f1,"\
                     %s  %s\n",
    gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros);
 }

if(vr == 1)
 {
  if(nnds == 2)
    fprintf(f1,"\
                 %s %s %s %d %s %d%s\n",
    gettext("С ч е т"),bros,gettext("от"),
    dd,mesqc,gd,gettext("г."));
  else
    fprintf(f1,"\
                     %s %s\n",gettext("Счёт"),bros);
  }

if(vr == 2)
  fprintf(f1,"\
                     %s  %s\n",
  gettext("АКТ ПРИЁМКИ"),bros);
if(vr == 3)
  fprintf(f1,"\
                     %s  %s\n",
  gettext("АКТ СПИСАНИЯ"),bros);


if(nnds != 2 || vr != 1)
  fprintf(f1,"\
                      %d %s %d%s\n",dd,mesqc,gd,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(f1,gettext("Доверенность"));
  fprintf(f1," N");
  iceb_u_pole(row[0],bros,1,'#');
  fprintf(f1," %s",bros);
  iceb_u_pole(row[0],bros,2,'#');
  fprintf(f1," %s %s",gettext("от"),bros);
 }
else
 if(metkadov == 1 && vr == 0)
  fprintf(f1,"%s N_____________________________________________",gettext("Доверенность"));

fprintf(f1,"\n");


/*Читаем через кого*/
class iceb_u_str sherez_kogo("");
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(f1,"%s: %s\n",gettext("Через кого"),row[0]);
  sherez_kogo.new_plus(row[0]);
 }


if(tipz == 2)
 {
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,3);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    fprintf(f1,"%s: %s\n",gettext("Основание"),row[0]);

  if(regnom.ravno()[0] != '\0')
    fprintf(f1,"%s: %s\n",gettext("Регистрационный номер"),regnom.ravno());
  
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,9);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    iceb_u_rsdat(&d,&m,&g,row[0],0);
//    if(d == 0 || SRAV1(g,m,d,gd,md,dd) <= 0)
    if(d == 0 || iceb_u_sravmydat(d,m,g,dd,md,gd) >= 0)
     {
      if(vr == 0)
       fprintf(f1,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
       gettext("Конечная дата расчёта"),row[0]);
      if(vr == 1)
       fprintf(f1,"\x1B\x45%s %s\x1B\x45\x1b\x46\n",
       gettext("Счёт действителен до"),row[0]);

     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)

  if(nomnn.ravno()[0] != '\0')
    fprintf(f1,"%s: %s\n",
    gettext("Номер налоговой накладной"),nomnn.ravno());
    
 }

if(nn1.ravno()[0] != '\0' && tipz == 1)
 fprintf(f1,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1.ravno());
if(nn1.ravno()[0] != '\0' && tipz == 2 && mvnpp > 0 )
 fprintf(f1,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1.ravno());

if(vr == 1)
 {
  fprintf(f1,"\x1B\x46"); /*Выключение режима выделенной печати*/
  fprintf(f1,"\x1B\x47"); /*Включение режима двойного удара*/
 }

if(nnds == 1)
  fprintf(f1,"\x1B\x4D"); //12-знаков
if(nnds == 0)
  fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/
if(nnds == 2)
  fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/

klst=0;
klli=1;

sapnak(nnds,f1);

itogo=itogo1=0.;
mtpr=0;

naz:;
mnt=0;
i=0;
kol=kolt=sum=0.;
kratn=cenan=kratz=cenaz=kodmz=0;

while(curtmp.read_cursor(&rowtmp) != 0)
 {

  kgrm=atoi(rowtmp[0]);
  naim.new_plus(rowtmp[1]);
  kodm=atoi(rowtmp[2]);
  nk=atoi(rowtmp[3]);
  cenan=atof(rowtmp[4]);
  cenan=iceb_u_okrug(cenan,okrcn);
  kratn=atoi(rowtmp[5]);
  ein.new_plus(rowtmp[6]);
  vtar=atoi(rowtmp[7]);
  shu.new_plus(rowtmp[8]);
  kolih=atof(rowtmp[9]);
  nomz.new_plus(rowtmp[10]);
    
  if(vtar == 1)
   {
    mnt++;
    continue;
   } 

  mest=0;

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 

  memset(bros,'\0',sizeof(bros));
  sprintf(bros,"%d",kodm);
  if(kdtr.ravno()[0] != '\0' && mtpr == 0 )
    if(iceb_u_pole1(kdtr.ravno(),bros,',',0,&i1) == 0 || iceb_u_SRAV(kdtr.ravno(),bros,0) == 0)
     {  
      mtar++;
      continue;
     }

  if(kdtr.ravno()[0] != '\0' && mtpr == 1)
    if(iceb_u_pole1(kdtr.ravno(),bros,',',0,&i1) != 0)
     if(iceb_u_SRAV(kdtr.ravno(),bros,0) != 0)
       continue;
   
  if(obzap == 1)
   {
    if((kodmz != 0 && kodmz != kodm) || (kratz != 0 && kratz != kratn) ||
    (cenaz != 0. && cenaz != cenan) || (eiz.ravno()[0] != '\0' && iceb_u_SRAV(eiz.ravno(),ein.ravno(),0) != 0)\
     || (naimz.ravno()[0] != '\0' && iceb_u_SRAV(naimz.ravno(),naim.ravno(),0) != 0))
     {
      if(nnds == 0)
       { 
        i++;
        klst++;
        fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
        i,
        iceb_u_kolbait(18,nomn.ravno()),nomn.ravno(),
        iceb_u_kolbait(46,naimz.ravno()),iceb_u_kolbait(46,naimz.ravno()),naimz.ravno(),
        iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
        kol,iceb_prcn(cenaz));
        fprintf(f1," %10s",iceb_prcn(sum));
        rasdokkr(kol,kolt,kratz,f1);

        if(iceb_u_strlen(naimz.ravno()) > 46)
         {
          klst++;
          fprintf(f1,"%3s %18s %s\n"," "," ",iceb_u_adrsimv(46,naimz.ravno()));
         }
       }
      if(nnds == 1)
       {
        i++;
        klst++;

        fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7.2f %11s",
        i,
        iceb_u_kolbait(33,naimz.ravno()),iceb_u_kolbait(33,naimz.ravno()),naimz.ravno(),
        iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
        kol,bb1,iceb_prcn(cenaz));

        fprintf(f1," %8s",iceb_prcn(sum));
        rasdokkr(kol,kolt,kratz,f1);

        if(iceb_u_strlen(naimz.ravno()) > 33)
         {
          klst++;
          fprintf(f1,"%3s %s\n"," ",iceb_u_adrsimv(33,naimz.ravno()));
         }
       }
      if(nnds == 2)
       { 
        i++;
        klst++;
        fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
        i,
        iceb_u_kolbait(20,nomzz.ravno()),nomzz.ravno(),
        iceb_u_kolbait(46,naimz.ravno()),iceb_u_kolbait(46,naimz.ravno()),naimz.ravno(),
        iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
        kol," ",iceb_prcn(cenaz));
        fprintf(f1," %10s\n",iceb_prcn(sum));

        if(iceb_u_strlen(naimz.ravno()) > 46)
         {
          klst++;
          fprintf(f1,"%3s %20s %s\n"," "," ",iceb_u_adrsimv(46,naimz.ravno()));
         }
       }
     }
    if(kodmz != kodm || kratz != kratn || cenaz != cenan || 
     iceb_u_SRAV(eiz.ravno(),ein.ravno(),0) != 0 || iceb_u_SRAV(naimz.ravno(),naim.ravno(),0) != 0)
     {
      kol=kolt=sum=0.;
      kodmz=kodm;
      cenaz=cenan;
      kratz=kratn;
      eiz.new_plus(ein.ravno());
      naimz.new_plus(naim.ravno());

     }
   }
  
  sprintf(strsql,"%d.%s.%d.%d",skl,shu.ravno(),kodm,nk);

  if(obzap == 1 && kol != 0.)
     sprintf(strsql,"%d.%s.%d.***",skl,shu.ravno(),kodm);
  nomn.new_plus(strsql);
  nomzz.new_plus(nomz.ravno());
  bb=cenan*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  bb1=cenan+(cenan*pnds/100.);
  bb1=iceb_u_okrug(bb1,okrg1);

  mest=0;
  if(mtpr == 0 && kratn != 0.)
   {
    mest=kolih/kratn;
    mest=iceb_u_okrug(mest,0.1);
    itogo_mest+=mest;
   }
  if(obzap == 0)
   {
    naimz.new_plus(naim.ravno());
    if(nnds == 1)
     {
      i++;
      klst++;
      fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7.2f %11s",
      i,
      iceb_u_kolbait(33,naim.ravno()),iceb_u_kolbait(33,naim.ravno()),naim.ravno(),
      iceb_u_kolbait(4,ein.ravno()),iceb_u_kolbait(4,ein.ravno()),ein.ravno(),
      kolih,bb1,iceb_prcn(cenan));
      fprintf(f1," %8s",iceb_prcn(bb));
      rasdokkr(kol,mest,kratn,f1);

      if(iceb_u_strlen(naim.ravno()) > 33)
       {
        klst++;
        fprintf(f1,"%3s %s\n"," ",iceb_u_adrsimv(33,naim.ravno()));
       }
     }

    if(nnds == 0)
     { 
      i++;
      klst++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
      i,
      iceb_u_kolbait(18,nomn.ravno()),nomn.ravno(),
      iceb_u_kolbait(46,naim.ravno()),iceb_u_kolbait(46,naim.ravno()),naim.ravno(),
      iceb_u_kolbait(4,ein.ravno()),iceb_u_kolbait(4,ein.ravno()),ein.ravno(),
      kolih,iceb_prcn(cenan));
      fprintf(f1," %10s",iceb_prcn(bb));
      rasdokkr(kol,mest,kratn,f1);

      if(iceb_u_strlen(naim.ravno()) > 46)
       {
        klst++;
        fprintf(f1,"%3s %18s %s\n"," "," ",iceb_u_adrsimv(46,naim.ravno()));
       }
     }

    if(nnds == 2)
     { 
      i++;
      klst++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
      i,
      iceb_u_kolbait(20,nomz.ravno()),nomz.ravno(),
      iceb_u_kolbait(46,naim.ravno()),iceb_u_kolbait(46,naim.ravno()),naim.ravno(),
      iceb_u_kolbait(4,ein.ravno()),iceb_u_kolbait(4,ein.ravno()),ein.ravno(),
      kolih," ",iceb_prcn(cenan));
      fprintf(f1," %10s\n",iceb_prcn(bb));

      if(iceb_u_strlen(naim.ravno()) > 46)
       {
        klst++;
        fprintf(f1,"%3s %20s %s\n"," "," ",iceb_u_adrsimv(46,naim.ravno()));
       }
     }
   }

  kol+=kolih;
  kolt+=mest;
  sum+=bb;

  itogo+=bb;

  if((klli == 1 && klst >= KLST1 ) || (klli != 1 && klst >= KLST2))
   {
    fprintf(f1,"\f");
    sapnak(nnds,f1);
    klst=0;
    klli++;
   }

 }

if(obzap == 1)
 {
  i++;
  klst++;
  if(nnds == 1)
   {
    fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7.2f %11s",
    i,
    iceb_u_kolbait(33,naimz.ravno()),iceb_u_kolbait(33,naimz.ravno()),naimz.ravno(),
    iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
    kol,bb1,iceb_prcn(cenaz));
    fprintf(f1," %8s",iceb_prcn(sum));
    rasdokkr(kol,kolt,kratz,f1);

    if(iceb_u_strlen(naimz.ravno()) > 33)
     {
      klst++;
      fprintf(f1,"%3s %s\n"," ",iceb_u_adrsimv(33,naimz.ravno()));
     }
   }        
  if(nnds == 0)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    i,
    iceb_u_kolbait(18,nomn.ravno()),nomn.ravno(),
    iceb_u_kolbait(46,naimz.ravno()),iceb_u_kolbait(46,naimz.ravno()),naimz.ravno(),
    iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
    kol,iceb_prcn(cenaz));
    fprintf(f1," %10s",iceb_prcn(sum));
    rasdokkr(kol,kolt,kratz,f1);
    if(iceb_u_strlen(naimz.ravno()) > 46)
     {
      klst++;
      fprintf(f1,"%3s %18s %s\n"," "," ",iceb_u_adrsimv(46,naimz.ravno()));
     }
   }
  if(nnds == 2)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
    i,
    iceb_u_kolbait(20,nomz.ravno()),nomz.ravno(),
    iceb_u_kolbait(46,naimz.ravno()),iceb_u_kolbait(46,naimz.ravno()),naimz.ravno(),
    iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
    kol," ",iceb_prcn(cenaz));
    fprintf(f1," %10s\n",iceb_prcn(sum));

    if(iceb_u_strlen(naimz.ravno()) > 46)
     {
      klst++;
      fprintf(f1,"%3s %20s %s\n"," "," ",iceb_u_adrsimv(46,naimz.ravno()));
     }
   }
 }

mtpr++;
/*
printw("mtpr=%d kdtr=%s mtar=%d\n",mtpr,kdtr.ravno(),mtar);
OSTANOV();
*/
if(mtpr == 1 && mtar != 0) /*Распечатываем отдельно тару*/
 {
  klst++;

  if(nnds == 1)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  if(nnds == 0)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  if(nnds == 2)
    fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - %s - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n",
    gettext("Т а р а"));

  curtmp.poz_cursor(0);
  goto naz;
 }

/*Распечатываем услуги*/
sprintf(strsql,"select * from Dokummat3 where \
datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",gd,md,dd,skl,nomdok);
/*printw("\nstrsql=%s\n",strsql);*/
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

class iceb_u_str naiusl("");

mppu=0;/*метка первого прохода услуг*/
if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);
  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    printf("strsql=%s\n",strsql);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
       naiusl.new_plus(row1[0]);
   }  

  if(naiusl.ravno()[0] == '\0')
   naiusl.new_plus(row[7]);  
  else
   {
    naiusl.plus(" ",row[7]);
   }
  
  nomn.new_plus(gettext("Услуги"));

  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;
  
  bb=iceb_u_okrug(bb,okrg1);
  bb1=cena+(cena*pnds/100.);
  bb1=iceb_u_okrug(bb1,okrg1);

  if(nnds == 1)
   {
/*
    if(mppu == 0)
     fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - П о с л у г и - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
*/
    fprintf(f1,"%3d %-33.33s %-4.4s %6.7g %7.2f %11s",
    ++i,naiusl.ravno(),row[3],kolih,bb1,iceb_prcn(cena));
    fprintf(f1," %8s\n",iceb_prcn(bb));
   }
  if(nnds == 0)
   { 
/*
    if(mppu == 0)
      fprintf(f1,"\
- - - - - - - - - - - - - - - - - - - - П о с л у г и - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - \n");
*/
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    ++i,
    iceb_u_kolbait(18,nomn.ravno()),nomn.ravno(),
    iceb_u_kolbait(46,naiusl.ravno()),iceb_u_kolbait(46,naiusl.ravno()),naiusl.ravno(),
    iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
    kolih,iceb_prcn(cena));
    fprintf(f1," %10s\n",iceb_prcn(bb));
   }
  if(nnds == 2)
   { 
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %5s %11s",
    ++i,
    iceb_u_kolbait(10,nomn.ravno()),nomn.ravno(),
    iceb_u_kolbait(46,naiusl.ravno()),iceb_u_kolbait(46,naiusl.ravno()),naiusl.ravno(),
    iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
    kolih," ",iceb_prcn(cena));
    fprintf(f1," %10s\n",iceb_prcn(bb));
   }
  
  mppu++;
  
  itogo+=bb;

 }
int dlinna=0; 
 
if(nnds == 1)
 {
  dlinna=66;
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");
  fprintf(f1,"\
%*s: %10s\n",iceb_u_kolbait(66,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));
  }

if(nnds == 0)
 {
  dlinna=96;
  memset(strsql,'\0',sizeof(strsql));
  if(itogo_mest > 0.)
   sprintf(strsql,"%.f",itogo_mest);
   
  fprintf(f1,"\
------------------------------------------------------------------------------------------------------------------------\n\
%*s: %10s %-4s\n",iceb_u_kolbait(96,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo),strsql);
  }
if(nnds == 2)
 {
  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n\
%*s: %10s\n",iceb_u_kolbait(104,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));
  dlinna=104;
 }
itogo1=itogo;

if(mvnpp == 0) /*Внешнее перемещение*/
 {
  itogo1=dok_end_muw(tipz,dd,md,gd,nomdok,skl,dlinna,itogo,pnds,lnds,mnt,nnds,f1,wpredok);
 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     vztr(dd,md,gd,skl,nomdok,&sumt,f1,nnds,wpredok);
  itogo1+=sumt;
 }

memset(str,'\0',sizeof(str));
iceb_u_preobr(itogo1,str,0);


fprintf(f1,"%s:%s\n",gettext("Сумма прописью"),str);
if(lnds != 0 && mvnpp == 0)
 fprintf(f1,"%s\n",gettext("Без НДС"));
 
fprintf(f1,"\x1B\x50"); /*10-знаков*/
fprintf(f1,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&rukov,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);


if(vr == 0)
 {
  if(mvnpp == 0)
   {
    iceb_insfil("matnakend_dp.alx",f1,wpredok);
   }
  fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/
  if(tipz == 1)
   {
    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
    iceb_u_kolbait(20,rukov.famil_inic.ravno()),rukov.famil_inic.ravno(),
    iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
    glavbuh.famil_inic.ravno());

    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_u_kolbait(20,gettext("Отпустил")),gettext("Отпустил"),
    20,"",
    iceb_u_kolbait(20,gettext("Принял")),gettext("Принял"),
    fio_klad_vibr.ravno());
   }    

  if(tipz == 2)
   {
    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
    iceb_u_kolbait(20,rukov.famil_inic.ravno()),rukov.famil_inic.ravno(),
    iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
    glavbuh.famil_inic.ravno());

    fprintf(f1,"\n%*s________________%-*s %*s________________%s\n",
    iceb_u_kolbait(20,gettext("Отпустил")),gettext("Отпустил"),
    iceb_u_kolbait(20,fio_klad_vibr.ravno()),fio_klad_vibr.ravno(),
    iceb_u_kolbait(20,gettext("Принял")),gettext("Принял"),
    sherez_kogo.ravno());
   }    

  if(mvnpp == 0)
   {
    iceb_insfil("matnakend.alx",f1,wpredok);
   }
 }
 
fprintf(f1,"\x12"); /*Отмена ужатого режима печати*/
if(vr == 1)
 {
/**********
  fprintf(f1,"\n%s__________________  %s_________________\n",
  gettext("Руководитель"),
  gettext("Главный бухгалтер"));
**************/
  fprintf(f1,"\n%*s________________%s\n\n%*s________________%s\n",
  iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
  rukov.famil_inic.ravno(),
  iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
  glavbuh.famil_inic.ravno());
  

  iceb_insfil("matshetend.alx",f1,wpredok);

  fprintf(f1,"\x1B\x48"); /*Выключение режима двойного удара*/
 }

if(vr == 2)
 {
  fprintf(f1,gettext("\n\
Отпустил _____________________        Принял__________________________\n\n\
Комиссия _____________________\n\
         _____________________\n\
         _____________________\n"));
 }
if(vr == 3)
 {
  fprintf(f1,gettext("\n\
Руководитель _____________________\n\n\
    Комиссия _____________________\n\
             _____________________\n\
             _____________________\n"));
 }
 
if(mvnpp == 0 && tipz == 2 && vr != 3) /*Внешнее перемещение*/
 {
  if(iceb_nalndoh(wpredok) == 0)
    fprintf(f1,"%s\n",gettext("Предприятие является плательщиком налога на доход на общих основаниях"));
//  fprintf(f1,"\n");
  if(ves != 0)
    fprintf(f1,"%s: %.2f %s\n",
    gettext("Вес"),ves,gettext("Кг."));

  fprintf(f1,"\x0E"); /*Включение режима удвоенной ширины на  одну строку*/
  fprintf(f1,gettext("Благодарим за сотрудничество !"));
  fprintf(f1,"\x14"); /*Выключение режима удвоенной ширины*/
 }


iceb_podpis(f1,wpredok);
//fprintf(f1,"\n\n");
if(ots != 0)
 fprintf(f1,"\x1b\x6C%c",1); /*Установка левого поля*/
if(metka_close == 1)
 fclose(f1);

}

/*****************/
/*Распечатка тары*/
/*****************/

void vztr(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,FILE *f1,short nnds,GtkWidget *wpredok)
{
double		itogo,bb;
class iceb_u_str nomn("");
SQL_str         row,row1;
char		strsql[1024];
long		kolstr;
double		kolih,cena;
int		nk,kgrm;
class iceb_u_str naim("");
class iceb_u_str shu("");
short		vtr;
int		i;

sprintf(strsql,"select * from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gd,md,dd,skl,nomdok);
SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  
if(kolstr == 0)
  return;

SQLCURSOR cur1;
itogo=0.;
i=0;
while(cur.read_cursor(&row) != 0)
 {
/*
  printw("%s %s %s %s %s %s %s %s %s %s %s\n",
  row[0],row[1],row[2],row[3],row[4],row[5],row[6],row[7],row[8],row[9],
  row[10]);
*/
  
  /*Читаем наименование материала*/
  naim.new_plus("");
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",row[4]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден код материала"));
    repl.plus(" ");
    repl.plus(row[4]);
    repl.plus(" !");
    iceb_menu_soob(&repl,wpredok);
   }
  else
   {
    naim.new_plus(row1[0]);
    if(row[16][0] != '\0')
     {
      naim.plus(" ",row[16]);
     }
   }
  kgrm=atoi(row1[1]);
  vtr=atoi(row[8]);

  if(vtr == 0. && (vtara != 0 && kgrm != vtara))
    continue;
  if(vtr == 0 && vtara == 0 )
    continue;

  nk=atoi(row[3]);
  kolih=atof(row[5]);
  cena=atof(row[6]);
  cena=iceb_u_okrug(cena,okrcn);

/*
  printw("mk2.kodm=%d %.2f vt=%d %d\n",mk2.kodm,mk2.nds,vt,mk8.kgrm);
  refresh();  
*/
  shu.new_plus("**");
  
  if(nk != 0)
   {
    sprintf(strsql,"select shetu from Kart where sklad=%s and nomk=%d",
    row[1],nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     { 
      shu.new_plus(row1[0]);
     }
   }

  sprintf(strsql,"%s.%s.%s.%d",row[1],shu.ravno(),row[4],nk);
  nomn.new_plus(strsql);

  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  i++;
  if(i == 1)
   fprintf(f1,"%s:\n",gettext("Оборотная тара"));
  if(nnds == 1)
   {
    fprintf(f1,"%3d %-*.*s %-*.*s %6.7g %7s %11s",
    i,
    iceb_u_kolbait(33,naim.ravno()),iceb_u_kolbait(33,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(4,row[7]),iceb_u_kolbait(4,row[7]),row[7],
    kolih," ",iceb_prcn(cena));
    fprintf(f1," %8s\n",iceb_prcn(bb));
   }
  if(nnds == 0)
   { 
  
    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    i,
    iceb_u_kolbait(18,nomn.ravno()),nomn.ravno(),
    iceb_u_kolbait(46,naim.ravno()),iceb_u_kolbait(46,naim.ravno()),naim.ravno(),
    iceb_u_kolbait(4,row[7]),iceb_u_kolbait(4,row[7]),row[7],
    kolih,iceb_prcn(cena));
    fprintf(f1," %10s\n",iceb_prcn(bb));
   }

  itogo+=bb;

 }
*sumt=itogo;
}

/************************/
/*Выдача шапки накладной*/
/************************/
void		sapnak(short nnds,FILE *f1)
{
if(nnds == 1)
 {
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");

  fprintf(f1,gettext(" N |    Наименование продукции       |Ед. |Колич-|Ц е н а|  Ц е н а  | Сумма  |Кол.|Крат-|\n"));
  fprintf(f1,gettext("   |           (товару)              |изм.|ество | с НДС |  без НДС  |без НДС |м-ст|ность|\n"));
  fprintf(f1,"\
------------------------------------------------------------------------------------------\n");
 }

if(nnds == 0)
 {

  fprintf(f1,"\
------------------------------------------------------------------------------------------------------------------------\n");

  fprintf(f1,gettext(" N |  Номенклатурный  |       Наименование продукции                 |Ед. |Количество|  Ц е н а  | С у м м а|Кол.|Крат-|\n"));
  fprintf(f1,gettext("   |     номер        |              (товару)                        |изм.|          |           |          |м-ст|ность|\n"));

  fprintf(f1,"\
------------------------------------------------------------------------------------------------------------------------\n");
 }

if(nnds == 2)
 {

  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");

  fprintf(f1,gettext(" N |   Номер заказа     |       Наименование продукции                 |Ед. |Количество|Факт.|  Ц е н а  | С у м м а|\n"));
  fprintf(f1,gettext("   |                    |              (товару)                        |изм.|          |кол. |           |          |\n"));

  fprintf(f1,"\
───────────────────────────────────────────────────────────────────────────────────────────────────────────────────────\n");
 }
}
/**********************************/
/*Вывод количество мест           */
/**********************************/
void	rasdokkr(double kol,double kolt,double kratz,FILE *ff)
{
int		dop=0;
double		ikolt=0.;
char		bros[512];
if(kolt != 0.)
 {
  modf(kolt,&ikolt);
  dop=(int)(kol-ikolt*kratz);
  if(dop > 0.)
   sprintf(bros,"%.f/%d",ikolt,dop);
  else
   sprintf(bros,"%.f",ikolt);

  fprintf(ff," %-4s %4.4g\n",bros,kratz);
 }
else
  fprintf(ff,"\n");
  
}
