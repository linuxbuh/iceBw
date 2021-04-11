/* $Id: rasdokuosw.c,v 1.21 2013/08/13 06:09:50 sasa Exp $ */
/*24.02.2016	10.02.2001	Белых А.И.	rasdokuosw.c
Распечатка накладной для учета основных средств
*/
#include        <errno.h>
#include	"buhg_g.h"

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern SQL_baza bd;

void rasdokuosw(const char *data_dok, //Дата документа
int podr,
const char *nomdok, //Номер документа
int tipz,  //1-приход 2-расход
GtkWidget *wpredok)
{
short dg,mg,gg;
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
char		strsql[2048];
SQL_str         row,row1;
SQLCURSOR curr;
int		kolstr;
char		bros[1024],bros1[1024];
class iceb_u_str kontr("");
class iceb_u_str kodop("");
class iceb_u_str osnov("");
int		mvnpp;
class iceb_u_str naimpr("");
int		podr1;
class iceb_u_str naipod("");
double		bs;
class iceb_u_str naim("");
FILE 		*ff;
char		imaf[64];
double		cena=0.;
double		itogo=0.;
double		pnds=0.;
class iceb_u_str dover("");
class iceb_u_str sherez("");
short		dvd=0,mvd=0,gvd=0;
double		sumands=0.;
short		lnds=0;
int		kodot1=0;
int		kodot2=0;
class iceb_u_str fio_matot("");

iceb_u_rsdat(&dg,&mg,&gg,data_dok,1);

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,osnov,dover,datvd,sherez,nds,\
kodol,kodolv,podrv,pn from Uosdok where datd='%04d-%02d-%02d' and nomd='%s' \
and tipz=%d",gg,mg,dg,nomdok,tipz);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) != 1)
 {

  sprintf(strsql,"Ненайден документ! Дата:%d.%d.%d Документ:%s",
  dg,mg,gg,nomdok);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

kontr.new_plus(row[0]);
kodop.new_plus(row[1]);

osnov.new_plus(row[2]);
dover.new_plus(row[3]);
iceb_u_rsdat(&dvd,&mvd,&gvd,row[4],2);
sherez.new_plus(row[5]);
lnds=atoi(row[6]);
kodot1=atoi(row[7]);
kodot2=atoi(row[8]);
podr1=atoi(row[9]);

pnds=atof(row[10]);

/*Читаем реквизиты организации свои */

class iceb_u_str kodkontr00("00");
if(tipz == 2)
 kodkontr00.new_plus(iceb_getk00(2,kodop.ravno(),wpredok));

sprintf(bros,"select * from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(iceb_sql_readkey(bros,&row,&curr,wpredok) != 1)
 {
  sprintf(strsql,"%s %s %s!",gettext("Не найден код контрагента"),kodkontr00.ravno(),gettext("в списке контрагентов"));
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
if(iceb_u_SRAV("00-",kontr.ravno(),1) != 0)
 {

  sprintf(bros,"select * from Kontragent where kodkon='%s'",kontr.ravno());
  if(iceb_sql_readkey(bros,&row,&curr,wpredok) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найден код контрагента"),kontr.ravno());
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
    gor_kontr1.new_plus(row[17]);    
   }
 }

sprintf(strsql,"select innom,bs,bsby,cena from Uosdok1 \
where datd='%04d-%02d-%02d' and nomd='%s' and tipz=%d",
gg,mg,dg,nomdok,tipz);
class SQLCURSOR cur;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 { 
  return;
 } 
sprintf(imaf,"uos%s.lst",nomdok);
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }

if(tipz == 1)
 fprintf(ff,"%-*s %s\n",
 iceb_u_kolbait(50,gettext("Кому")),
 gettext("Кому"),
 gettext("От кого"));

if(tipz == 2)
 fprintf(ff,"%-*s %s\n",
 iceb_u_kolbait(50,gettext("От кого")),
 gettext("От кого"),
 gettext("Кому"));

fprintf(ff,"%-*.*s N%s %-*.*s\n",
iceb_u_kolbait(50,nmo.ravno()),iceb_u_kolbait(50,nmo.ravno()),nmo.ravno(),
kontr.ravno(),
iceb_u_kolbait(50,nmo1.ravno()),iceb_u_kolbait(50,nmo1.ravno()),nmo1.ravno());

for(int nom=50; nom < iceb_u_strlen(nmo.ravno()) || nom < iceb_u_strlen(nmo1.ravno()); nom+=50)
 {
  if(iceb_u_strlen(nmo.ravno()) > nom)
   fprintf(ff,"%-*.*s ",
   iceb_u_kolbait(50,iceb_u_adrsimv(nom,nmo.ravno())),
   iceb_u_kolbait(50,iceb_u_adrsimv(nom,nmo.ravno())),
   iceb_u_adrsimv(nom,nmo.ravno()));
  else
   fprintf(ff,"%50s ","");
   
  if(iceb_u_strlen(nmo1.ravno()) > nom)
    fprintf(ff,"%-.*s",
    iceb_u_kolbait(50,iceb_u_adrsimv(nom,nmo1.ravno())),
    iceb_u_adrsimv(nom,nmo1.ravno()));

  fprintf(ff,"\n");
 }

memset(bros,'\0',sizeof(bros));
memset(bros1,'\0',sizeof(bros1));

sprintf(bros,"%s %s",gettext("Адрес"),gor.ravno());
sprintf(bros1,"%s %s",gettext("Адрес"),gor1.ravno());


fprintf(ff,"%-*.*s %-*.*s\n",iceb_u_kolbait(50,bros),iceb_u_kolbait(50,bros),bros,iceb_u_kolbait(50,bros1),iceb_u_kolbait(50,bros1),bros1);

if(iceb_u_strlen(bros) > 50 || iceb_u_strlen(bros1) > 50)
 fprintf(ff,"%-*.*s %-*.*s\n",
 iceb_u_kolbait(50,iceb_u_adrsimv(50,bros)),
 iceb_u_kolbait(50,iceb_u_adrsimv(50,bros)),
 iceb_u_adrsimv(50,bros),
 iceb_u_kolbait(50,iceb_u_adrsimv(50,bros1)),
 iceb_u_kolbait(50,iceb_u_adrsimv(50,bros1)),
 iceb_u_adrsimv(50,bros1));

sprintf(bros,"%s %s",gettext("Код ОКПО"),pkod.ravno());
sprintf(bros1,"%s %s",gettext("Код ОКПО"),pkod1.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(50,bros),bros,bros1);

sprintf(bros,"%s %s %s %s",gettext("Р/С"),nsh.ravno(),
gettext("МФО"),mfo.ravno());

sprintf(bros1,"%s %s %s %s",gettext("Р/С"),nsh1.ravno(),
gettext("МФО"),mfo1.ravno());

fprintf(ff,"%-*s %s\n",iceb_u_kolbait(50,bros),bros,bros1);

sprintf(bros,"%s %s %s %s",gettext("в"),nmb.ravno(),
gettext("гор."),grb.ravno());
sprintf(bros1,"%s %s %s %s",gettext("в"),nmb1.ravno(),
gettext("гор."),grb1.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(50,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Налоговый номер"),inn.ravno());

sprintf(bros1,"%s %s",
gettext("Налоговый номер"),inn1.ravno());

fprintf(ff,"%-*s %s\n",iceb_u_kolbait(50,bros),bros,bros1);

sprintf(bros,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds.ravno());
sprintf(bros1,"%s %s",
gettext("Номер сви-ства плательщика НДС"),npnds1.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(50,bros),bros,bros1);

sprintf(bros,"%s %s",gettext("Телефон"),tel.ravno());
sprintf(bros1,"%s %s",gettext("Телефон"),tel1.ravno());
fprintf(ff,"%-*s %s\n",iceb_u_kolbait(50,bros),bros,bros1);


/*Читаем наименование операции*/
if(tipz == 1)
 strcpy(bros,"Uospri");
if(tipz == 2)
 strcpy(bros,"Uosras");

mvnpp=0;

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros,kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&curr) != 1)
 {
  sprintf(strsql,gettext("Не найден код операции %s !"),kodop.ravno());
  iceb_menu_soob(strsql,wpredok);
 }
else
 {

  mvnpp=atoi(row[1]);
  naimpr.new_plus(row[0]);
  
 }
class iceb_u_str prinql("");
if(mvnpp == 1) /*Внутреннее перемещение*/
 {
  /*Читаем наименование склада*/
  sprintf(strsql,"select naik from Uospod where kod=%d",
  podr1);

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    naipod.new_plus("");
    sprintf(strsql,"%s %d !",gettext("Не найден код подразделения"),podr1);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    naipod.new_plus(row[0]);

  if(tipz == 1)
   {
    sprintf(strsql,"%s:\n%s %d <= %s %d %s",
    naimpr.ravno(),gettext("Подразделение"),podr,
    gettext("Подразделение"),podr1,naipod.ravno());
    fprintf(ff,"\n%.*s\n",iceb_u_kolbait(70,strsql),strsql);
   }
  if(tipz == 2)
   {
    sprintf(strsql,"%s:\n%s %d => %s %d %s",
       naimpr.ravno(),gettext("Подразделение"),podr,
       gettext("Подразделение"),podr1,naipod.ravno());
    fprintf(ff,"\n%.*s\n",iceb_u_kolbait(70,strsql),strsql);

   }
  //Читаем фамилию материально-ответственного лица

  sprintf(strsql,"select naik from Uosol where kod=%d",kodot1);

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код мат.-ответственного"),kodot1);
    iceb_menu_soob(strsql,wpredok);
   }
  else
    fio_matot.new_plus(row[0]);

  if(tipz == 1)
   {
    sprintf(bros1,"%d %s <= ",kodot1,fio_matot.ravno());
    prinql.new_plus(fio_matot.ravno());
   }
  if(tipz == 2)
    sprintf(bros1,"%d %s => ",kodot1,fio_matot.ravno());

  //Читаем фамилию материально-ответственного лица

  sprintf(strsql,"select naik from Uosol where kod=%d",kodot2);

  if(sql_readkey(&bd,strsql,&row,&curr) != 1)
   {
    naipod.new_plus("");
    sprintf(strsql,"%s %d !",gettext("Не найден код мат.-ответственного"),kodot2);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    naipod.new_plus(row[0]);
    prinql.new_plus(row[0]);
   }
  sprintf(bros,"%d %s\n",kodot2,naipod.ravno());
  strcat(bros1,bros);
  fprintf(ff,"%.*s\n",iceb_u_kolbait(70,bros1),bros1);
  
 }
else
 fprintf(ff,"\n%s",naimpr.ravno());
fprintf(ff,"\n");

if(tipz == 1)
 fprintf(ff,"\n%s\n",gor_kontr1.ravno());
if(tipz == 2) 
 fprintf(ff,"\n%s\n",gor_kontr.ravno());


if(tipz == 1)
  fprintf(ff,"\
                   %s  N%s\n",
  gettext("ПРИХОДНАЯ НАКЛАДНАЯ"),nomdok);
if(tipz == 2)
  fprintf(ff,"\
                   %s  N%s\n",
  gettext("РАСХОДНАЯ НАКЛАДНАЯ"),nomdok);

memset(bros,'\0',sizeof(bros));
iceb_mesc(mg,1,bros);
fprintf(ff,"\
                  %d %s %d%s\n",
dg,bros,gg,gettext("г."));

if(dover.ravno()[0] != '\0')
  fprintf(ff,"%sN%s %s %d.%d.%d%s\n",gettext("Доверенность"),
  dover.ravno(),gettext("от"),
  dvd,mvd,gvd,gettext("г."));

if(osnov.ravno()[0] != '\0')
  fprintf(ff,"%s: %s\n",gettext("Основание"),osnov.ravno());


if(sherez.ravno()[0] != '\0')
   fprintf(ff,"%s: %s\n",gettext("Через кого"),sherez.ravno());

 
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,gettext("\
Инвентарный номер|                    Наименование                        |Е.и.|Кол|    Цена     |    Сумма    |\n"));
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");
      
while(cur.read_cursor(&row) != 0)
 {
  bs=atof(row[2]);
  if(bs == 0.)
   bs=atof(row[1]);
  cena=atof(row[3]);
  /*Читаем наименование основного средства*/

  sprintf(strsql,"select naim from Uosin where innom=%s",
  row[0]);

  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    naim.new_plus("");
    sprintf(strsql,"Не найден инвентарный номер %s !",row[0]);
    iceb_menu_soob(strsql,wpredok);
   }
  else
   naim.new_plus(row1[0]);
  if(tipz == 1 || mvnpp == 1)
   {
    fprintf(ff,"%-17s %-*.*s Шт   %3d %13.2f %13.2f\n",
    row[0],
    iceb_u_kolbait(56,naim.ravno()),iceb_u_kolbait(56,naim.ravno()),naim.ravno(),
    1,bs,bs);
    itogo+=bs;
   }
  else
  if(tipz == 2)
   {
    fprintf(ff,"%-17s %-*.*s Шт   %-3d %13.2f %13.2f\n",
    row[0],
    iceb_u_kolbait(56,naim.ravno()),iceb_u_kolbait(56,naim.ravno()),naim.ravno(),
    1,cena,cena);
    itogo+=cena;
   }
}
fprintf(ff,"\
----------------------------------------------------------------------------------------------------------------\n");
fprintf(ff,"\
%*s:%13.2f\n",iceb_u_kolbait(97,gettext("Итого")),gettext("Итого"),itogo);

if(mvnpp == 0) /*Внешнее перемещение*/
 {
  if(lnds == 0 )
   {
    sprintf(bros,"%s %.2f%% :",gettext("НДС"),pnds);
    sumands=itogo*pnds/100.;
    sumands=iceb_u_okrug(sumands,0.01);
    fprintf(ff,"%*s%13.2f\n",iceb_u_kolbait(98,bros),bros,sumands);
   }

  itogo=itogo+sumands;
  itogo=iceb_u_okrug(itogo,0.01);

  strcpy(bros,gettext("Итого к оплате:"));
  fprintf(ff,"%*s%13.2f\n",iceb_u_kolbait(98,bros),bros,itogo);
 } 

memset(strsql,'\0',sizeof(strsql));
iceb_u_preobr(itogo,strsql,0);
fprintf(ff,"%s:%s\n",gettext("Сумма прописью"),strsql);

iceb_insfil("uosnakend_dp.alx",ff,wpredok); /*вставка текста до подписей*/

class iceb_fioruk_rk rukov;
class iceb_fioruk_rk glavbuh;
iceb_fioruk(1,&rukov,wpredok);
iceb_fioruk(2,&glavbuh,wpredok);

fprintf(ff,"\n%*s____________%-*s %*s____________%s\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
iceb_u_kolbait(20,rukov.famil_inic.ravno()),rukov.famil_inic.ravno(),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"),
glavbuh.famil_inic.ravno());

if(mvnpp == 1) /*Внутреннее перемещение*/
 fprintf(ff,"\n%*s____________%-*s %*s____________%s\n",
 iceb_u_kolbait(20,gettext("Отпустил")),gettext("Отпустил"),
 iceb_u_kolbait(20,fio_matot.ravno()),fio_matot.ravno(),
 iceb_u_kolbait(20,gettext("Принял")),gettext("Принял"),
 prinql.ravno());
else
 fprintf(ff,"\n%*s____________%-*s %*s____________%s\n",
 iceb_u_kolbait(20,gettext("Отпустил")),gettext("Отпустил"),
 iceb_u_kolbait(20,fio_matot.ravno()),fio_matot.ravno(),
 iceb_u_kolbait(20,gettext("Принял")),gettext("Принял"),
 sherez.ravno());

iceb_insfil("uosnakend_pp.alx",ff,wpredok); /*вставка текста после подписей*/

if(mvnpp == 0 && tipz == 2) /*Внешнее перемещение*/
 {
  fprintf(ff,"\n");
  fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
  fprintf(ff,"%s",gettext("Благодарим за сотрудничество !"));
  fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
  fprintf(ff,"\n");
 }
iceb_podpis(ff,wpredok);
fclose(ff);


class iceb_u_spisok imafil;
class iceb_u_spisok naimf;
imafil.plus(imaf);
naimf.plus(gettext("Распечатка накладной"));
iceb_ustpeh(imaf,3,wpredok);

iceb_rabfil(&imafil,&naimf,wpredok);
}

