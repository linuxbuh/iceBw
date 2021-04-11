/* $Id:$ */
/*19.06.2019    21.04.2018      Белых А.И.      rasnakw.c
Распечатка накладных в "Материальном учёте"
*/
#include        <errno.h>
#include        <math.h>
#include	"buhg_g.h"

void rasnak_vztr(short dd,short md,short gd,int skl,const char *nomdok,double *sumt,int simv_plus,FILE *f1,GtkWidget *wpredok);
double rasnak_end(int tipz,short dd,short md,short gd,const char *nomdok,int skl,int dlinna,double itogo,float pnds,int lnds,int mnt,int simv_plus,FILE *ff,GtkWidget *wpredok);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern short    srtnk; /*0-не включена 1-включена сортировка записей в накладной*/
extern short	vtara; /*Код группы возвратная тара*/
extern short    obzap; /*0-не объединять записи 1-обединять*/
extern class iceb_u_spisok sp_fio_klad; /*выбранные фамилии кладовщиков по складам*/
extern class iceb_u_int sp_kod_sklad; /*список кодов складов для фамилий кладовщиков*/
extern SQL_baza bd;

int rasnakw(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
const char *imaf,   // имя файла
short lnds,  //Льготы по НДС*
int *simv_plus,
FILE *f1,
double ves, //Вес
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
int             i;
class iceb_u_str nomn("");  /*Номенклатурный номер*/
double          itogo;
double          itogo1;
double          bb=0.,bb1=0.;
short           d,m,g;
short           dd,md,gd;
short		mnt; /*Метка наличия тары*/
double		sumt; /*Сумма по таре*/
class iceb_u_str kdtr(""); /*Коды тары*/
short		mtpr; /*Метка первого и второго прохода*/
short		mppu; /*Метка первого прохода услуг*/
double		mest;
class iceb_u_str naimpr("");
int		skl1;
short           mvnpp; /*0-внешняя 1 -внутреняя 2-изменение стоимости*/
int             kodm,kodmz,nk;
double		kol,sum;
double		kolt=0.;
double		kratn,kratz,cenan,cenaz;
class iceb_u_str ein(""),eiz("");
class iceb_u_str naim(""),naimz("");
SQL_str         row,row1,rowtmp;
SQLCURSOR cur,cur1,curtmp;
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
short		kgrm; /*код группы материалла*/
class iceb_u_str shu(""); /*счет учета материалла*/
double		kolih,cena;
class iceb_u_str fmoll(""); //Фамилия материально-ответственного
class iceb_u_str nomz("");
class iceb_u_str nomzz("");
class iceb_u_str mesqc("");
class iceb_u_str kodkontr00("00");
short m_tara=0; //Не возвратная тара- обычная
double itogo_mest=0.;
mest=mnt=0;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {

  sprintf(strsql,"%s-%s\n%s=%d.%d.%d %s=%s %s=%d",
  __FUNCTION__,gettext("Не найден документ !"),
  gettext("Дата"),
  dg,mg,gg,
  gettext("Номер документа"),
  nomdok,
  gettext("Код склада"),
  skl);

  iceb_menu_soob(strsql,wpredok);
  return(1);
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

sprintf(strsql,"select naik,vido from %s where kod='%s'",bros,kop.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {

  sprintf(strsql,gettext("Не найден код операции %s !"),kop.ravno());
  iceb_menu_soob(strsql,wpredok);

 }
else
 {

  mvnpp=atoi(row[1]);
  naimpr.new_plus(row[0]);
  
 }

class iceb_u_str nadpis("");
iceb_poldan("Надпись на расходной накладной",&nadpis,"matnast.alx",wpredok);
  


iceb_poldan("Код тары",&kdtr,"matnast.alx",wpredok);


if(tipz == 2)
 {
  if(iceb_u_SRAV("00",kodkontr00.ravno(),0) == 0)
    kodkontr00.new_plus(iceb_getk00(0,kop.ravno(),wpredok));
 }

/*Читаем реквизиты организации свои */


sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(iceb_sql_readkey(bros,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kodkontr00.ravno());
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  if(row[16][0] == '\0')
   {
    if(iceb_u_polen(row[1],&nmo,1,'(') != 0)
     nmo.new_plus(row[1]);
   }
  else
    nmo.new_plus(row[16]);
    
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
if(iceb_u_atof(kpos.ravno()) != 0. || kpos.ravno()[0] != '0')
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kpos.ravno());
  if(iceb_sql_readkey(bros,&row,&cur,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код контрагента"),kpos.ravno());
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {

    if(row[16][0] == '\0')
     nmo1.new_plus(row[1]);
    else
     nmo1.new_plus(row[16]);
    
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
int kol_znak_nomn=0;
if(sortdokmuw(dg,mg,gg,skl,nomdok,imatmptab,&tabtmp,&kol_znak_nomn,wpredok) != 0)
 return(1);


sprintf(strsql,"select * from %s",imatmptab);

if(srtnk == 1)
  sprintf(strsql,"select * from %s order by kgrm asc,naim asc",imatmptab);

int kolstrtmp=0;
if((kolstrtmp=curtmp.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора!"),strsql,wpredok);
  return(1);
 }

int metka_open_fil=0;

if(f1 == NULL)
 {
  if((f1 = fopen(imaf,"w")) == NULL)
   {
    iceb_er_op_fil(imaf,"",errno,wpredok);
    return(1);
   }
  metka_open_fil=1;
 }

//if(ots != 0)
// fprintf(f1,"\x1b\x6C%c",ots); /*Установка левого поля*/

//fprintf(f1,"\x1B\x33%c",30-kor); /*Уменьшаем межстрочный интервал*/
//fprintf(f1,"\x12"); /*Отмена ужатого режима*/
//fprintf(f1,"\x1B\x4D"); /*12-знаков*/



if(nadpis.ravno()[0] != '\0' && tipz == 2 && mvnpp == 0)
 fprintf(f1,"%s\n",nadpis.ravno());
if(tipz == 1)
 fprintf(f1,gettext("\
  Кому                                                        От кого\n"));
if(tipz == 2)
 fprintf(f1,gettext("\
   От кого                                                    Кому\n"));

fprintf(f1,"%-*.*s ",iceb_u_kolbait(60,nmo.ravno()),iceb_u_kolbait(60,nmo.ravno()),nmo.ravno());
fprintf(f1,"\x1B\x45N%s %-.*s\x1B\x46\n",
kpos.ravno(),
iceb_u_kolbait(60,nmo1.ravno()),nmo1.ravno());

for(int nom=60; nom < iceb_u_strlen(nmo.ravno()) || nom < iceb_u_strlen(nmo1.ravno()); nom+=60)
 {
  if(iceb_u_strlen(nmo.ravno()) > nom)
   fprintf(f1,"%-*.*s ",
   iceb_u_kolbait(60,iceb_u_adrsimv(nom,nmo.ravno())),
   iceb_u_kolbait(60,iceb_u_adrsimv(nom,nmo.ravno())),
   iceb_u_adrsimv(nom,nmo.ravno()));
  else
   fprintf(f1,"%60s "," ");
   
  if(iceb_u_strlen(nmo1.ravno()) > nom)
    fprintf(f1,"\x1B\x45%-.*s\x1B\x46",
    iceb_u_kolbait(60,iceb_u_adrsimv(nom,nmo1.ravno())),
    iceb_u_adrsimv(nom,nmo1.ravno()));

  fprintf(f1,"\n");
 }


memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));
sprintf(bros,"%s %s",gettext("Адрес"),gor.ravno());
sprintf(bros1,"%s %s",gettext("Адрес"),gor1.ravno());

fprintf(f1,"%-*.*s %-*.*s\n",
iceb_u_kolbait(60,bros),iceb_u_kolbait(60,bros),bros,
iceb_u_kolbait(60,bros1),iceb_u_kolbait(60,bros1),bros1);

if(iceb_u_strlen(bros) > 60 || iceb_u_strlen(bros1) > 60)
 fprintf(f1,"%-*.*s %-*.*s\n",
 iceb_u_kolbait(60,iceb_u_adrsimv(60,bros)),
 iceb_u_kolbait(60,iceb_u_adrsimv(60,bros)),
 iceb_u_adrsimv(60,bros),
 iceb_u_kolbait(60,iceb_u_adrsimv(60,bros1)),
 iceb_u_kolbait(60,iceb_u_adrsimv(60,bros1)),
 iceb_u_adrsimv(60,bros1));
  

sprintf(bros,"%s %s",gettext("Код ЕГРПОУ"),pkod.ravno());
sprintf(bros1,"%s %s",gettext("Код ЕГРПОУ"),pkod1.ravno());
fprintf(f1,"%-*s %s\n",iceb_u_kolbait(60,bros),bros,bros1);

sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh.ravno(),
gettext("МФО"),mfo.ravno());

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1.ravno(),
gettext("МФО"),mfo1.ravno());

fprintf(f1,"%-*s %s\n",iceb_u_kolbait(60,bros),bros,bros1);


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


fprintf(f1,"%-*.*s %s\n",iceb_u_kolbait(60,bros),iceb_u_kolbait(60,bros),bros,bros1);
if(iceb_u_strlen(bros) > 60)
 fprintf(f1,"%s\n",iceb_u_adrsimv(60,bros));  

sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn.ravno());

sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1.ravno());

fprintf(f1,"%-*s %s\n",iceb_u_kolbait(60,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds.ravno());
sprintf(bros1,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds1.ravno());
fprintf(f1,"%-*s %s\n",iceb_u_kolbait(60,bros),bros,bros1);

sprintf(bros,"%s %s",gettext("Телефон"),tel.ravno());

sprintf(bros1,"%s %s",gettext("Телефон"),tel1.ravno());
fprintf(f1,"%-*s %s\n",iceb_u_kolbait(60,bros),bros,bros1);


//fprintf(f1,"\x1B\x50"); /*10-знаков*/



class iceb_u_spisok fio_klad;
class iceb_u_str fio_klad_vibr("");


/*Читаем наименование склада*/
sprintf(strsql,"select naik,fmol,np from Sklad where kod=%d",skl);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skl);
  iceb_menu_soob(strsql,wpredok);
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
//      if((vozv=dirmasiv(&fio_klad,-1,-1,0,gettext("Выберите фамилию кладовщика"),0,&kk)) >= 0)
        fio_klad_vibr.new_plus(fio_klad.ravno(vozv));
        
     }

    sp_kod_sklad.plus(skl);
    sp_fio_klad.plus(fio_klad_vibr.ravno());
   
   }
  else
    fio_klad_vibr.new_plus(sp_fio_klad.ravno(nom_skl));
 }

fprintf(f1,"%s\n",naiskl.ravno());

if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  if(iceb_u_atof(kpos.ravno()) == 0. && iceb_u_polen(kpos.ravno(),bros,sizeof(bros),2,'-') == 0 && kpos.ravno()[0] == '0')
   {
    iceb_u_polen(kpos.ravno(),bros,sizeof(bros),2,'-');
    skl1=(int)iceb_u_atof(bros);
    /*Читаем наименование склада*/
    sprintf(strsql,"select naik,fmol from Sklad where kod=%d",skl1);
    naiskl.new_plus("");
    class iceb_u_str fmol("");
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
     {
      iceb_beep();
      sprintf(strsql,gettext("Не найден склад %d в списке складов !"),skl1);
      iceb_menu_soob(strsql,wpredok);
     }
    else
     {
      naiskl.new_plus(row[0]);
      fmol.new_plus(row[1]);
     }
    if(tipz == 1)
     {
      
      sprintf(strsql,"%s: %s %d <= %s %d %s",
      naimpr.ravno(),gettext("Склад"),skl,
      gettext("Склад"),skl1,naiskl.ravno());
      
      fprintf(f1,"%.*s\n",iceb_u_kolbait(70,strsql),strsql);

      sprintf(strsql,"%22s %s <= %s"," ",fmoll.ravno(),fmol.ravno());
      fprintf(f1,"%.*s\n",iceb_u_kolbait(70,strsql),strsql);
            
     }

    if(tipz == 2)
     {
      sprintf(strsql,"%s: %s %d => %s %d %s",
      naimpr.ravno(),gettext("Склад"),skl,
      gettext("Склад"),skl1,naiskl.ravno());

      fprintf(f1,"%.*s\n",iceb_u_kolbait(70,strsql),strsql);

      sprintf(strsql,"%22s %s >= %s"," ",fmoll.ravno(),fmol.ravno());
      fprintf(f1,"%.*s\n",iceb_u_kolbait(70,strsql),strsql);

     }
   }
  else
    fprintf(f1,"%s\n",naimpr.ravno());
 }
else
 { 
  bros[0]='\0';

  if(iceb_poldan("Наименование операции в накладной",bros,"matnast.alx",wpredok) == 0)
   if(iceb_u_SRAV(bros,"Вкл",1) == 0)
    fprintf(f1,"%s\n",naimpr.ravno());
   
 }


sprintf(bros,"N%s",nomdok);  
if(tipz == 2 && iceb_poldan("Перенос символа N",bros,"matnast.alx",wpredok) == 0)
 {
  if(iceb_u_SRAV(bros,"Вкл",1) == 0)
   {
    memset(bros,'\0',sizeof(bros));
    iceb_u_polen(nomdok,bros,sizeof(bros),1,'-');
    iceb_u_polen(nomdok,bros1,sizeof(bros1),2,'-');
    strcat(bros,"-N");  
    strcat(bros,bros1);  
    
   }       
  else
    sprintf(bros,"N%s",nomdok);  

 } 
iceb_mesc(md,1,&mesqc);


if(tipz == 1)
 if(gor_kontr1.getdlinna() > 1)
  fprintf(f1,"%s\n",gor_kontr1.ravno());

if(tipz == 2) 
 if(gor_kontr1.getdlinna() > 1)
  fprintf(f1,"%s\n",gor_kontr.ravno());
 
if(tipz == 1)
  fprintf(f1,"\
                                       %s  %s\n",
  gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),bros);
if(tipz == 2)
  fprintf(f1,"\
                                       %s  %s\n",
  gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros);


fprintf(f1,"\
                                          %d %s %d%s\n",dd,mesqc.ravno(),gd,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(f1,"%s N",gettext("Доверенность"));
  iceb_u_polen(row[0],bros,sizeof(bros),1,'#');
  fprintf(f1," %s",bros);
  iceb_u_polen(row[0],bros,sizeof(bros),2,'#');
  fprintf(f1," %s %s",gettext("от"),bros);
 }

//fprintf(f1,"\n");


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
//    if(d == 0 || iceb_u_SRAV1(g,m,d,gd,md,dd) <= 0)
    if(d == 0 || iceb_u_sravmydat(d,m,g,dd,md,gd) >= 0)
     {
      fprintf(f1,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
      gettext("Конечная дата расчёта"),row[0]);

     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   nomnn.new_plus(row[0]);
   
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


//fprintf(f1,"\x0F"); //Включение ужатого режима печати


class iceb_rnl_c rh;

if(kol_znak_nomn > 18)
 {
  *simv_plus=rh.simv_plus=kol_znak_nomn-18;
 }
rasnak_sap(&rh,NULL,f1);

itogo=itogo1=0.;
mtpr=0;

naz:;
mnt=0;
i=0;
kol=kolt=sum=0.;
nomn.new_plus("");
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
//  kolt=0.;

  if(vtara != 0 && kgrm == vtara)
   {
    mnt++;
    continue;
   } 

  memset(bros,'\0',sizeof(bros));
  sprintf(bros,"%d",kodm);
  if(mtpr == 0)
   if(iceb_u_proverka(kdtr.ravno(),bros,0,1) == 0)
    {  
     m_tara++;
     continue;
    }
  if(mtpr == 1)
   if(iceb_u_proverka(kdtr.ravno(),bros,0,1) != 0)
    continue;
    
  if(obzap == 1)
   {
    if((kodmz != 0 && kodmz != kodm) || (kratz != 0 && kratz != kratn) ||
    (cenaz != 0. && cenaz != cenan) || (eiz.ravno()[0] != '\0' && iceb_u_SRAV(eiz.ravno(),ein.ravno(),0) != 0)\
     || (naimz.ravno()[0] != '\0' && iceb_u_SRAV(naimz.ravno(),naim.ravno(),0) != 0))
     {
      i++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
      i,
      iceb_u_kolbait(18+*simv_plus,nomn.ravno()),nomn.ravno(),
      iceb_u_kolbait(46,naimz.ravno()),iceb_u_kolbait(46,naimz.ravno()),naimz.ravno(),
      iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
      kol,iceb_prcn(cenaz));
      
      fprintf(f1," %10s",iceb_prcn(sum));
      rasdokkr(kol,kolt,kratz,f1);

      if(iceb_u_strlen(naimz.ravno()) > 46)
       {
        fprintf(f1,"%3s %*s %s\n"," ",18+*simv_plus," ",iceb_u_adrsimv(46,naimz.ravno()));
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

      i++;
      fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
      i,
      iceb_u_kolbait(18+*simv_plus,nomn.ravno()),nomn.ravno(),
      iceb_u_kolbait(46,naim.ravno()),iceb_u_kolbait(46,naim.ravno()),naim.ravno(),
      iceb_u_kolbait(4,ein.ravno()),iceb_u_kolbait(4,ein.ravno()),ein.ravno(),
      kolih,iceb_prcn(cenan));
      
      fprintf(f1," %10s",iceb_prcn(bb));
      rasdokkr(kol,mest,kratn,f1);
      if(iceb_u_strlen(naim.ravno()) > 46)
       {
        for(int nom=0; nom < *simv_plus; nom++)
         fprintf(f1," ");
        fprintf(f1,"%3s %18s %s\n"," "," ",iceb_u_adrsimv(46,naim.ravno()));
       }

   }

  kol+=kolih;
  kolt+=mest;
  sum+=bb;
  itogo+=bb;


 }

if(obzap == 1)
 {
  i++;

  fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
  i,
  iceb_u_kolbait(18+*simv_plus,nomn.ravno()),nomn.ravno(),
  iceb_u_kolbait(46,naimz.ravno()),iceb_u_kolbait(46,naimz.ravno()),naimz.ravno(),
  iceb_u_kolbait(4,eiz.ravno()),iceb_u_kolbait(4,eiz.ravno()),eiz.ravno(),
  kol,iceb_prcn(cenaz));
  fprintf(f1," %10s",iceb_prcn(sum));
  rasdokkr(kol,kolt,kratz,f1);
  if(iceb_u_strlen(naimz.ravno()) > 46)
   {
    fprintf(f1,"%3s %18s %s\n"," "," ",iceb_u_adrsimv(46,naimz.ravno()));
   }
 }

mtpr++;
/*
printw("mtpr=%d kdtr=%s mtar=%d\n",mtpr,kdtr,mtar);
OSTANOV();
*/
if(mtpr == 1 && m_tara != 0) /*Распечатываем отдельно тару*/
 {
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
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

mppu=0;/*метка первого прохода услуг*/
class iceb_u_str naiusl("");

if(kolstr != 0)
while(cur.read_cursor(&row) != 0)
 {
  kolih=atof(row[4]);
  cena=atof(row[5]);
  cena=iceb_u_okrug(cena,okrcn);
  
  nomn.new_plus(gettext("**Услуги***"));
  if(kolih > 0)
    bb=cena*kolih;
  else
    bb=cena;
  
  bb=iceb_u_okrug(bb,okrg1);
  bb1=cena+(cena*pnds/100.);
  bb1=iceb_u_okrug(bb1,okrg1);

  naiusl.new_plus("");
  if(atoi(row[10]) != 0)
   {
    sprintf(strsql,"select naius from Uslugi where kodus=%s",row[10]);
    if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) == 1)
     naiusl.new_plus(row1[0]);
   }  
  if(naiusl.getdlinna() <= 1)
   naiusl.new_plus(row[7]);
  else
   {
//    strcat(naiusl," ");
//    strcat(naiusl,row[7]);
    naiusl.plus(" ",row[7]);
   }

    fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
    ++i,
    iceb_u_kolbait(18+*simv_plus,nomn.ravno()),nomn.ravno(),
    iceb_u_kolbait(46,naiusl.ravno()),iceb_u_kolbait(46,naiusl.ravno()),naiusl.ravno(),
    iceb_u_kolbait(4,row[3]),iceb_u_kolbait(4,row[3]),row[3],
    kolih,iceb_prcn(cena));
    fprintf(f1," %10s\n",iceb_prcn(bb));
  
  mppu++;
  
  itogo+=bb;

 }

fprintf(f1,"ICEB_LST_END\n");


int dlinna=0; 

memset(strsql,'\0',sizeof(strsql));
if(itogo_mest > 0.)
   sprintf(strsql,"%.f",itogo_mest);
dlinna=96+*simv_plus;
fprintf(f1,"\
------------------------------------------------------------------------------------------------------------------------\n\
%*s: %10s %-4s\n",iceb_u_kolbait(dlinna,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo),strsql);


itogo1=itogo;

if(mvnpp == 0) /*Внешнее перемещение*/
 {
  
  itogo1=rasnak_end(tipz,dd,md,gd,nomdok,skl,dlinna,itogo,pnds,lnds,mnt,*simv_plus,f1,wpredok);

 }
else
 { 
  sumt=0.;
  if(mnt != 0)
     rasnak_vztr(dd,md,gd,skl,nomdok,&sumt,*simv_plus,f1,wpredok);
  itogo1+=sumt;
 }
memset(str,'\0',sizeof(str));
iceb_u_preobr(itogo1,str,0);


fprintf(f1,"%s:%s\n",gettext("Сумма прописью"),str);
if(lnds != 0 && mvnpp == 0 )
 fprintf(f1,"%s\n",gettext("Без НДС"));
 
//fprintf(f1,"\x1B\x50"); /*10-знаков*/
//fprintf(f1,"\x1B\x32"); /*Межстрочный интервал 1/6 дюйма*/

class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&rukov,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);

if(mvnpp == 0)
 {
  iceb_insfil("matnakend_dp.alx",f1,wpredok);
 }

//fprintf(f1,"\x0F"); /*Включение ужатого режима печати*/
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

//fprintf(f1,"\x12"); /*Отмена ужатого режима печати*/




if(mvnpp == 0 && tipz == 2 ) /*Внешнее перемещение*/
 {
  if(iceb_nalndoh(wpredok) == 0)
    fprintf(f1,"%s\n",gettext("Предприятие является плательщиком налога на доход на общих основаниях."));
//  fprintf(f1,"\n");
  if(ves != 0)
    fprintf(f1,"%s: %.2f %s\n",
    gettext("Вес"),ves,gettext("Кг."));

  fprintf(f1,"\x0E"); /*Включение режима удвоенной ширины*/
  fprintf(f1,gettext("Благодарим за сотрудничество !"));
  fprintf(f1,"\x14"); /*Выключение режима удвоенной ширины*/
 }


iceb_podpis(f1,wpredok);

//if(ots != 0)
// fprintf(f1,"\x1b\x6C%c",1); /*Установка левого поля*/


if(metka_open_fil == 1)
 {
  fclose(f1);
 
  class iceb_rnl_c rh;
  iceb_ustpeh(imaf,3,&rh.orient,wpredok);
  rh.simv_plus=*simv_plus;
  iceb_rnl(imaf,&rh,&rasnak_sap,wpredok);

 }
 
return(0);
}

/*****************/
/*Распечатка тары*/
/*****************/

void rasnak_vztr(short dd,short md,short gd,int skl,const char *nomdok,
double *sumt,int simv_plus,FILE *f1,
GtkWidget *wpredok)
{
double		itogo,bb;
char		nomn[512];
SQL_str         row,row1;
char		strsql[512];
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
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

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
  
  /*Читаем наименование материалла*/
  naim.new_plus("");
  sprintf(strsql,"select naimat,kodgr from Material where kodm=%s",row[4]);

  if(iceb_sql_readkey(strsql,&row1,&cur1,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",gettext("Не найден код материалла"),row[4]);
    iceb_menu_soob(strsql,wpredok);
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
    sprintf(strsql,"select shetu from Kart where sklad=%s and nomk=%d",row[1],nk);
    if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
     { 
      shu.new_plus(row1[0]);
     }
   }

  sprintf(nomn,"%s.%s.%s.%d",row[1],shu.ravno(),row[4],nk);

  bb=cena*kolih;
  bb=iceb_u_okrug(bb,okrg1);
  i++;
  if(i == 1)
   fprintf(f1,"%s:\n",gettext("Оборотная тара"));


  fprintf(f1,"%3d %-*s %-*.*s %-*.*s %10.10g %11s",
  i,
  iceb_u_kolbait(18+simv_plus,nomn),nomn,
  iceb_u_kolbait(46,naim.ravno()),iceb_u_kolbait(46,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(4,row[7]),iceb_u_kolbait(4,row[7]),row[7],
  kolih,iceb_prcn(cena));
  fprintf(f1," %10s\n",iceb_prcn(bb));

  itogo+=bb;

 }
*sumt=itogo;
}
/************************/
/*Выдача шапки накладной*/
/************************/
void rasnak_sap(class iceb_rnl_c *rh,int *nom_str,FILE *ff)
{
if(nom_str != NULL)
  *nom_str+=4;


for(int nom=0; nom < rh->simv_plus; nom++)
 fprintf(ff,"-");
 
fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");

//fprintf(ff,gettext(" N |  Номенклатурный  |       Наименование продукции                 |Ед. |Количество|  Ц е н а  | С у м м а|Кол.|Крат-|\n"));
//fprintf(ff,gettext("   |       номер      |       (товаров,работ,услуг)                  |изм.|          |           |          |м-ст|ность|\n"));


fprintf(ff,"%s",gettext(" N |  Номенклатурный  "));

for(int nom=0; nom < rh->simv_plus; nom++)
 fprintf(ff," ");

fprintf(ff,"%s",gettext("|       Наименование продукции                 |Ед. |Количество|  Ц е н а  | С у м м а|Кол.|Крат-|\n"));

fprintf(ff,"%s",gettext("   |       номер      "));

for(int nom=0; nom < rh->simv_plus; nom++)
 fprintf(ff," ");

fprintf(ff,"%s",gettext("|       (товаров,работ,услуг)                  |изм.|          |           |          |м-ст|ность|\n"));

for(int nom=0; nom < rh->simv_plus; nom++)
 fprintf(ff,"-");

fprintf(ff,"\
------------------------------------------------------------------------------------------------------------------------\n");
}

/***************/
/*Печать концовки*/
/*******************/
double rasnak_end(int tipz,
short dd,short md,short gd,
const char *nomdok,int skl,int dlinna,double itogo,float pnds,
int lnds,
int mnt,
int simv_plus,
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

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
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
    
    if(sql_readkey(&bd,strsql,&row,&cur) == 1)
      bb=atof(row[0]);
   }

  bb=iceb_u_okrug(bb,0.01);

  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(dlinna+1,bros),bros,bb);

 }

double sumt=0.;

if(mnt != 0)
   rasnak_vztr(dd,md,gd,skl,nomdok,&sumt,simv_plus,ff,wpredok);
      
double itogo1=(itogo+sumkor)+bb+sumt;
strcpy(strsql,gettext("Итого"));

fprintf(ff,"%*s: %10.2f\n",iceb_u_kolbait(dlinna,strsql),strsql,itogo1);
return(itogo1);
}
