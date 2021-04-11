/*$Id: usl_shet_fakw.c,v 1.15 2013/08/13 06:09:57 sasa Exp $*/
/*13.07.2015	22.02.2005	Белых А.И.	usl_shet_fakw.c
Распечатка счёта фактуры для подсистемы "Учёт услуг"
Если вернули 0-сделали докумет
             1-нет
*/
#include        <errno.h>
#include	"buhg_g.h"

int usl_shet_fak_sap(short dd,short md,short gd,const char*,const char *ras_shet,const char *mfo,const char *kod,const char *naim_kontr,const char *dover,const char *herez,FILE *ff,GtkWidget*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern SQL_baza bd;

int usl_shet_fakw(short dg,short mg,short gg, //Дата документа
int podr, //Подразделение
const char *nomdok, //Номер документа
iceb_u_str *imafil, //имя файла с распечаткой
int tipz,  //1-приход 2-расход
GtkWidget *wpredok)
{
class iceb_u_str nmo(""),nmo1("");  /*Наименование организации*/
class iceb_u_str pkod(""),pkod1(""); //Код ЕДРПУО
class iceb_u_str mfo(""),mfo1("");  /*МФО*/
class iceb_u_str nsh(""),nsh1("");  /*Номер счета*/
char		strsql[1024];
SQL_str row;
SQLCURSOR cur;
int		kolstr;
class iceb_u_str kodop("");
class iceb_u_str kontr("");
double sumkor;
iceb_u_str naimnal;
float		procent=0.;
class iceb_u_str kodkontr00("");
class iceb_u_str bros("");
short lnds=0; //Льготы по НДС
class iceb_u_str dover("");
class iceb_u_str sherez("");
short		mvnpp;
float pnds=0.;

/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,sumkor,datop,osnov,nds,dover,sherez,pn from Usldokum \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gg,mg,dg,podr,nomdok,tipz);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;

  sprintf(strsql,"usl_shet_fakw-%s !",gettext("Не найден документ"));

  repl.plus_ps(strsql);

  sprintf(strsql,"%s:%d.%d.%d %s:%s %s:%d",
  gettext("Дата"),
  dg,mg,gg,
  gettext("Документ"),
  nomdok,
  gettext("Подразделение"),
  podr);

  repl.ps_plus(strsql);

  iceb_menu_soob(&repl,wpredok);
  return(1);
 }
pnds=atof(row[8]);
kontr.new_plus(row[0]);
kodop.new_plus(row[1]);
sumkor=atof(row[2]);


lnds=atoi(row[5]);

dover.new_plus(row[6]);

sherez.new_plus(row[7]);

naimnal.new_plus("");
usldopnalw(tipz,kodop.ravno(),&procent,&naimnal);

int kol_str_ots=0;
//читаем количество сторок отступа для логотипа

if(iceb_poldan("Количество строк отступа для счёта с логотипом",&bros,"uslnast.alx",wpredok ) == 0)
  kol_str_ots=bros.ravno_atoi();
  
/*Читаем наименование операции*/
if(tipz == 1)
 bros.new_plus("Usloper1");
if(tipz == 2)
 bros.new_plus("Usloper2");

mvnpp=0;

sprintf(strsql,"select naik,vido from %s where kod='%s'",
bros.ravno(),kodop.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),kodop.ravno());
  iceb_menu_soob(strsql,wpredok);
 }
else
 {

  mvnpp=atoi(row[1]);
  
 }

kodkontr00.new_plus("00");
if(tipz == 2)
 {
  if(iceb_poldan(kodop.ravno(),&bros,"uslnast.alx",wpredok) == 0)
   {
    kodkontr00.new_plus(bros.ravno());
   }
 }
//printw("\nkodkontr00=%s\n",kodkontr00);
//OSTANOV();

/*Читаем реквизиты организации свои */


sprintf(strsql,"select * from Kontragent where kodkon='%s'",kodkontr00.ravno());
if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  sprintf(strsql,"%s %s !",
  gettext("Не найден код контрагента"),kodkontr00.ravno());
  iceb_menu_soob(strsql,wpredok);
 }
else
 {
  nmo.new_plus(row[1]);
  pkod.new_plus(row[5]);
  mfo.new_plus(row[6]);
  nsh.new_plus(row[7]);
 } 



/*Читаем реквизиты организации чужие*/
/*При внутреннем перемещении не читать*/
if(kontr.ravno_atof() != 0. || kontr.ravno()[0] != '0')
 {

  sprintf(strsql,"select * from Kontragent where kodkon='%s'",kontr.ravno());
  if(sql_readkey(&bd,strsql,&row,&cur) != 1)
   {
    sprintf(strsql,"%s %s !",
    gettext("Не найден код контрагента"),kontr.ravno());
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    if(row[16][0] == '\0')
      nmo1.new_plus(row[1]);
    else
      nmo1.new_plus(row[16]);
    pkod1.new_plus(row[5]);
    mfo1.new_plus(row[6]);
    nsh1.new_plus(row[7]);
    
   }
 }

sprintf(strsql,"select metka,kodzap,kolih,cena,ei,dnaim from Usldokum1 \
where datd='%d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d order by metka,kodzap asc",
gg,mg,dg,podr,nomdok,tipz);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
  return(1);

sprintf(strsql,"shfk%d.lst",getpid());
imafil->new_plus(strsql);
FILE *ff;

if((ff = fopen(imafil->ravno(),"w")) == NULL)
 {
  iceb_er_op_fil(imafil->ravno(),"",errno,wpredok);
  return(1);
 }
iceb_u_startfil(ff);

for(int i=0; i < kol_str_ots; i++)
 fprintf(ff,"\n");

fprintf(ff,"\x1b\x6C%c",10); /*Установка левого поля*/

if(tipz == 1)
if(usl_shet_fak_sap(dg,mg,gg,nomdok,nsh1.ravno(),mfo1.ravno(),pkod1.ravno(),nmo.ravno(),dover.ravno(),sherez.ravno(),ff,wpredok) != 0)
 {
  fclose(ff);
  unlink(imafil->ravno());
  return(1);
 }

if(tipz == 2)
if(usl_shet_fak_sap(dg,mg,gg,nomdok,nsh.ravno(),mfo.ravno(),pkod.ravno(),nmo1.ravno(),dover.ravno(),sherez.ravno(),ff,wpredok) != 0)
 {
  fclose(ff);
  unlink(imafil->ravno());
  return(1);
 }


SQLCURSOR curr;
SQL_str row1;
int metka;
int kodzap;
double kolih;
double cena;
double cenasnd;
double itogo=0.;
double suma;
class iceb_u_str naim("");
int nom_str=0;
while(cur.read_cursor(&row) != 0)
 {
  metka=atoi(row[0]);
  kodzap=atoi(row[1]);
  kolih=atof(row[2]);
  cena=atof(row[3]);
  cena=iceb_u_okrug(cena,okrcn);
  suma=cena*kolih;
  suma=iceb_u_okrug(suma,okrg1);
  itogo+=suma;
  cenasnd=cena+cena*pnds/100.;
  cenasnd=iceb_u_okrug(cenasnd,okrg1);
  
  /*Читаем наименование услуги*/

  if(metka == 0)
   sprintf(strsql,"select naimat from Material where kodm=%d",
   kodzap);

  if(metka == 1)
   sprintf(strsql,"select naius from Uslugi where kodus=%d",
   kodzap);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
    if(metka == 0)
     sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodzap);
    if(metka == 1)
     sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodzap);
    iceb_menu_soob(strsql,wpredok);
    naim.new_plus("");
   }
  else
   {
    naim.new_plus(row1[0]);
    if(row[5][0] != '\0')
     {
      naim.plus(" ",row[5]);
     }    
   }
  fprintf(ff," %3d %-*.*s %6.6g %-*.*s %8.8g %10.2f\n",
  ++nom_str,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  kolih,
  iceb_u_kolbait(6,row[4]),iceb_u_kolbait(6,row[4]),row[4],
  cena,suma);

  if(iceb_u_strlen(naim.ravno()) > 30)
     fprintf(ff,"%3s %s\n"," ",iceb_u_adrsimv(30,naim.ravno()));
 }
fprintf(ff,"\
---------------------------------------------------------------------\n");
int dlina=58;
fprintf(ff,"%*s:%10.2f\n",iceb_u_kolbait(dlina,gettext("Итого")),gettext("Итого"),itogo);

if(mvnpp == 0) /*Внешнее перемещение*/
 {


  double bb=0.;

  if(sumkor != 0)
   {
    bb=sumkor*100./itogo;
    bb=iceb_u_okrug(bb,0.01);
    if(sumkor < 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Снижка"),bb*(-1));
    if(sumkor > 0)
      sprintf(strsql,"%s %.6g%% :",gettext("Надбавка"),bb);

    fprintf(ff,"%*s%10.2f/З %s %.2f\n",
    iceb_u_kolbait(dlina+1,strsql),strsql,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);

    if(sumkor < 0)
      sprintf(strsql,gettext("Итого со снижкой :"));
    if(sumkor > 0)
      sprintf(strsql,gettext("Итого с надбавкой :"));

    fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,strsql),strsql,sumkor+itogo);

   }

  double bbproc=0.;
  if(procent != 0.)
   {
    sprintf(strsql,"%s %.2f%% :",naimnal.ravno(),procent);
    bbproc=(itogo+sumkor)*procent/100.;
    bbproc=iceb_u_okrug(bbproc,0.01);
    fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,strsql),strsql,bbproc);

   }

  bb=0.;

  if(lnds == 0 )
   {
    double sumandspr=0.;
    if(tipz == 1)
     {
      sprintf(strsql,"select sumnds from Usldokum where \
datd='%04d-%d-%d' and podr=%d and nomd='%s' and tp=%d",
      gg,mg,dg,podr,nomdok,tipz);
      if(sql_readkey(&bd,strsql,&row,&curr) == 1)
        sumandspr=atof(row[0]);
     }

    sprintf(strsql,"%s %.2f%% :",gettext("НДС"),pnds);
    if(sumandspr != 0.)
      bb=sumandspr;
    else        
      bb=(itogo+sumkor)*pnds/100.;
    bb=iceb_u_okrug(bb,0.01);
    fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,strsql),strsql,bb);
   }

  double itogonds=(itogo+sumkor)+bb+ bbproc;
  bros.new_plus(gettext("Итого к оплате:"));
  if(tipz == 1)
    bros.new_plus(gettext("Итого"));
  fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,bros.ravno()),bros.ravno(),itogonds);
 }

fprintf(ff,"\n%*s______________________\n\n",iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"));
fprintf(ff,"%*s______________________\n",iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"));


iceb_insfil("usl_shet_fak_end.alx",ff,wpredok);


iceb_podpis(ff,wpredok);
fprintf(ff,"\x1b\x6C%c",1); /*Установка левого поля*/


fclose(ff);
return(0);

}
/*******************************/
/*Формирование шапки документа*/
/*******************************/

int usl_shet_fak_sap(short dd,short md,short gd,
const char *nomdok,
const char *ras_shet,
const char *mfo,
const char *kod,
const char *naim_kontr,
const char *dover,
const char *herez,
FILE *ff,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"usl_shet_fak.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
class iceb_u_str stroka("");
char bros[512];
int nomer_str=0;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;

  nomer_str++;
  stroka.new_plus(row_alx[0]);
  switch(nomer_str)
   {
    case 2:
     iceb_u_vstav(&stroka,ras_shet,15,67,1);
     break;

    case 3:
     iceb_u_vstav(&stroka,mfo,15,67,1);
     break;

    case 4:
     iceb_u_vstav(&stroka,kod,15,67,1);
     break;

    case 6:
     iceb_u_vstav(&stroka,nomdok,21,39,1);
     sprintf(bros,"%02d.%02d.%d",dd,md,gd);     
     iceb_u_vstav(&stroka,bros,58,69,1);
     break;

    case 8:
     iceb_u_vstav(&stroka,naim_kontr,12,67,1);
     break;

    case 11:
     iceb_u_vstav(&stroka,dover,16,27,1);
     iceb_u_vstav(&stroka,herez,34,67,1);
     break;
     
   }
  fprintf(ff,"%s",stroka.ravno());
 }
return(0);
}
