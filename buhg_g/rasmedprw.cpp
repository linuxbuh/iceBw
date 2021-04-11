/*$Id: rasmedprw.c,v 1.23 2013/09/26 09:46:53 sasa Exp $*/
/*12.07.2015	07.07.2002	Белых А.И.	rasmedprw.c
Распечатка накладной на медицинские препараты
*/
#include        <errno.h>
#include	"buhg_g.h"

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern SQL_baza  bd;

void rasmedprw(short dg,short mg,short gg, //дата документа
int skl,  //Склад
const char *nomdok, //Номер документа
GtkWidget *wpredok)
{
char		strsql[1024];
class iceb_u_str bros("");
SQL_str		row,row1;
class iceb_u_str kpos("");
class iceb_u_str kop("");
class iceb_u_str nomnn("");
class iceb_u_str nn1("");
int		tipz=0;
FILE		*ff;
char		imaf[64];
int		ots=4;
int		kor=0;
class iceb_u_str naiskl("");
int		kolstr=0;
short		d,m,g;
SQLCURSOR cur;

/*Читаем шапку документа*/
sprintf(strsql,"select * from Dokummat \
where datd='%04d-%02d-%02d' and sklad=%d and nomd='%s'",
gg,mg,dg,skl,nomdok);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
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
kpos.new_plus(row[3]);
kop.new_plus(row[6]);
tipz=atoi(row[0]);
nomnn.new_plus(row[5]);
nn1.new_plus(row[11]);

/*Читаем льготы по ндс*/
int lnds=0;
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,11);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 lnds=atoi(row[0]);

sprintf(imaf,"nmed%d.lst",getpid());
if((ff = fopen(imaf,"w")) == NULL)
 {
  iceb_er_op_fil(imaf,"",errno,wpredok);
  return;
 }
int kolstrok=0;
rashapw(tipz,&kolstrok,"00",kpos.ravno(),ots,kor,ff,wpredok);


/*Читаем наименование операции*/
if(tipz == 1)
 bros.new_plus("Prihod");
if(tipz == 2)
 bros.new_plus("Rashod");

int mvnpp=0;

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros.ravno(),kop.ravno());
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
  
 }

/*Читаем наименование склада*/
sprintf(strsql,"select naik,fmol from Sklad where kod=%d",
skl);

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
 }

fprintf(ff,"\n%s",naiskl.ravno());

//sprintf(bros,"N%s",nomdok);  
bros.new_plus("N");
bros.plus(nomdok);
if(tipz == 1)
  fprintf(ff,"\n\
                     %s  %s\n",
  gettext("АКТ ПРИЁМКИ"),bros.ravno());

if(tipz == 2)
  fprintf(ff,"\n\
                   %s  %s\n",
  gettext("РАСХОДНАЯ НАКЛАДНАЯ"),bros.ravno());

iceb_mesc(mg,1,&bros);
fprintf(ff,"\
                      %d %s %d%s\n",
dg,bros.ravno(),gg,gettext("г."));

/*Читаем доверенность*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,1);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
 {
  fprintf(ff,gettext("Доверенность"));
  fprintf(ff," N");
  iceb_u_polen(row[0],&bros,1,'#');
  fprintf(ff," %s",bros.ravno());
  iceb_u_polen(row[0],&bros,2,'#');
  fprintf(ff," %s %s",gettext("от"),bros.ravno());
 }
fprintf(ff,"\n");


/*Читаем через кого*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,2);

if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   fprintf(ff,"%s: %s\n",gettext("Через кого"),row[0]);


if(tipz == 2)
 {
  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,3);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
    fprintf(ff,"%s: %s\n",gettext("Основание"),row[0]);

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,9);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)
   {
    iceb_u_rsdat(&d,&m,&g,row[0],0);
    if(d == 0 || iceb_u_sravmydat(d,m,g,dg,mg,gg) >= 0)
     {
      fprintf(ff,"\x1B\x45%s: %s\x1B\x45\x1b\x46\n",
      gettext("Конечная дата расчёта"),row[0]);
     }
   }

  sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
  gg,nomdok,skl,10);

  if(sql_readkey(&bd,strsql,&row,&cur) == 1)

  if(nomnn.ravno()[0] != '\0')
    fprintf(ff,"%s: %s\n",
    gettext("Номер налоговой накладной"),nomnn.ravno());
    
 }
if(nn1.ravno()[0] != '\0' && tipz == 1)
 fprintf(ff,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1.ravno());
if(nn1.ravno()[0] != '\0' && tipz == 2 && mvnpp > 0 )
 fprintf(ff,"%s: %s\n",
 gettext("Номер встречной накладной"),nn1.ravno());

//fprintf(ff,"\x1B\x4D"); /*12-знаков*/
fprintf(ff,"\x0F");  /*Ужатый режим*/

fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

fprintf(ff," N |    Наименование продукции       |Ед. |Колич-| Цена с |  Ц е н а  | Сумма  |   Серия  |Регистрация препарата| \n");
fprintf(ff,"   |           (товару)              |изм.|ество |  НДС   |  без НДС  |без НДС |          |                     | \n");
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");


sprintf(strsql,"select nomkar,kodm,kolih,cena,ei,dnaim \
from Dokummat1 where datd='%04d-%02d-%02d' and \
sklad=%d and nomd='%s' order by kodm asc",
gg,mg,dg,skl,nomdok);


if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

int nomerpp=0;
class iceb_u_str naim("");
double  cenasnds=0.;
double  cenabeznds=0.;
double  sumabeznds=0.;
double  kolih=0.;
class iceb_u_str seriq("");
class iceb_u_str rnd("");
short	dr,mr,gr; //Дата регистрации мед-препарата
double	itogo=0.;
SQLCURSOR cur1;
while(cur.read_cursor(&row) != 0)
 {
  /*Читаем наименование материалла*/
  sprintf(strsql,"select naimat from Material where kodm=%s",
  row[1]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    naim.new_plus("");
    printf("Не нашли наименование материалла по коду %s\n",row[1]);
   }
  else
   {
    naim.new_plus(row1[0]);
    if(row[5][0] != '\0')
     {
      naim.plus(" ",row[5]);
     }
   }

  cenabeznds=atof(row[3]);
  cenabeznds=iceb_u_okrug(cenabeznds,okrcn);
  
  kolih=atof(row[2]);

  sumabeznds=cenabeznds*kolih;
  sumabeznds=iceb_u_okrug(sumabeznds,okrg1);
  itogo+=sumabeznds;
  cenasnds=cenabeznds+(cenabeznds*pnds/100.);
  cenasnds=iceb_u_okrug(cenasnds,okrg1);

  //Читаем карточку материалла
  sprintf(strsql,"select datv,innom,rnd from Kart where sklad=%d and \
nomk=%s",skl,row[0]);
  if(sql_readkey(&bd,strsql,&row1,&cur1) == 1)
   {
    iceb_u_rsdat(&dr,&mr,&gr,row1[0],2);    
    seriq.new_plus(row1[1]);
    rnd.new_plus(row1[2]);
   }  
  else
   {
    dr=mr=gr=0;
    rnd.new_plus("");
    seriq.new_plus("");
   }
  if(dr != 0)
   {
    sprintf(strsql,"%d.%d.%d%s",dr,mr,gr,gettext("г."));
    rnd.plus(" ",strsql);
   }
  if(lnds == 0) 
   fprintf(ff,"%3d %-*.*s %-*s %6.6g %8.2f %11.11g %8.2f %-*s %s\n",
   ++nomerpp,
   iceb_u_kolbait(33,naim.ravno()),iceb_u_kolbait(33,naim.ravno()),naim.ravno(),
   iceb_u_kolbait(4,row[4]),row[4],
   kolih,cenasnds,cenabeznds,sumabeznds,
   iceb_u_kolbait(10,seriq.ravno()),seriq.ravno(),
   rnd.ravno());
  else
   fprintf(ff,"%3d %-*.*s %-*s %6.6g %8s %11.11g %8.2f %-*s %s\n",
   ++nomerpp,
   iceb_u_kolbait(33,naim.ravno()),iceb_u_kolbait(33,naim.ravno()),naim.ravno(),
   iceb_u_kolbait(4,row[4]),row[4],
   kolih," ",cenabeznds,sumabeznds,
   iceb_u_kolbait(10,seriq.ravno()),seriq.ravno(),
   rnd.ravno());
  
  if(iceb_u_strlen(naim.ravno()) > 33)
    fprintf(ff,"%4s %s\n"," ",iceb_u_adrsimv(33,naim.ravno()));
 }
fprintf(ff,"\
-----------------------------------------------------------------------------------------------------------------\n");

fprintf(ff,"\
%*s: %10s\n",iceb_u_kolbait(67,gettext("Итого")),gettext("Итого"),iceb_prcn(itogo));

/*Читаем сумму корректировки*/
sprintf(strsql,"select sodz from Dokummat2 \
where god=%d and nomd='%s' and sklad=%d and nomerz=%d",
gg,nomdok,skl,13);

double sumkor=0.;

if(sql_readkey(&bd,strsql,&row,&cur1) == 1)
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
  iceb_u_kolbait(68,strsql),strsql,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);

  if(sumkor < 0)
    sprintf(strsql,gettext("Итого со снижкой :"));
  if(sumkor > 0)
    sprintf(strsql,gettext("Итого с надбавкой :"));

  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(68,strsql),strsql,sumkor+itogo);

 }

bb=0.;

if(lnds == 0 )
 {
  sprintf(strsql,"%s %.2f%% :",gettext("НДС"),pnds);
  bb=(itogo+sumkor)*pnds/100.;
  bb=iceb_u_okrug(bb,0.01);
  fprintf(ff,"%68s %10.2f\n",strsql,bb);
 }

double itogo1=(itogo+sumkor)+bb;

bros.new_plus("");
if(tipz == 2)
  bros.new_plus(gettext("Итого к оплате:"));
if(tipz == 1 && lnds == 0)
  bros.new_plus(gettext("Итого:"));

if(bros.ravno()[0] != '\0')
  fprintf(ff,"%*s %10.2f\n",iceb_u_kolbait(68,bros.ravno()),bros.ravno(),itogo1);

if(tipz == 2)
 {
  class iceb_u_str otvkh("");
  iceb_poldan("Ответственный за качество",&otvkh,"matnast.alx",wpredok);
  
  fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
  fprintf(ff,"%s.\n",gettext("Разрешено к реализации"));
  fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
  fprintf(ff,"%s_____________/%s/\n",gettext("Ответственный за качество"),otvkh.ravno());
  fprintf(ff,"\n\%s%s\n",gettext("\
Руководитель__________________        Главный  бухгалтер_______________\n\n"),
  gettext("\
Отпустил______________________        Принял__________________________"));
  if(iceb_nalndoh(wpredok) == 0)
    fprintf(ff,"%s\n",gettext("Предприятие является плательщиком налога на доход на общих основаниях"));

  fprintf(ff,"\x0E"); /*Включение режима удвоенной ширины*/
  fprintf(ff,gettext("Благодарим за сотрудничество !"));
  fprintf(ff,"\x14"); /*Выключение режима удвоенной ширины*/
 }
 
if(tipz == 1)
  fprintf(ff,"\n\n\
%*s_____________________ %*s_____________________\n\n\
%*s_____________________\n\
%15s_____________________\n\
%15s_____________________\n",
iceb_u_kolbait(15,gettext("Руководитель")),gettext("Руководитель"),
iceb_u_kolbait(15,gettext("Принял")),gettext("Принял"),
iceb_u_kolbait(15,gettext("Комиссия")),gettext("Комиссия"),
" "," ");

iceb_podpis(ff,wpredok);



fclose(ff);

iceb_u_spisok imafils;
iceb_u_spisok naimf;
imafils.plus(imaf);
naimf.plus(gettext("Распечатка накладной на мед-препараты"));
iceb_ustpeh(imaf,1,wpredok);

iceb_rabfil(&imafils,&naimf,wpredok);

}
