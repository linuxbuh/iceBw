/*$Id: usl_act_vrw.c,v 1.16 2013/08/13 06:09:57 sasa Exp $*/
/*13.07.2015	22.02.2005	Белых А.И.	usl_act_vrw.c
Распечатка акта выполненых работ, списания материалов для подсистемы "Учёт услуг"
*/
#include        <errno.h>
#include	"buhg_g.h"

int usl_act_vr(short dd,short md,short gd,const char *nomdok,const char*,const char *dover,const char*,FILE *ff,GtkWidget*);

extern double	okrcn;  /*Округление цены*/
extern double   okrg1; /*Округление*/
extern SQL_baza bd;

int usl_act_vrw(short dg,short mg,short gg, //Дата документа
int podr, //Подразделение
const char *nomdok, //Номер документа
iceb_u_str *imafil, //имя файла с распечаткой
int tipz,  //1-приход 2-расход
GtkWidget *wpredok)
{
class iceb_u_str nmo(""),nmo1("");  /*Наименование организации*/
char		strsql[1024];
SQL_str row;
SQLCURSOR cur;
int		kolstr;
class iceb_u_str kodop("");
class iceb_u_str kontr("");
double sumkor;
iceb_u_str	naimnal;
naimnal.plus("");
float		procent=0.;
class iceb_u_str kodkontr00("");
class iceb_u_str bros("");
short lnds=0; //Льготы по НДС
class iceb_u_str dover("");
short		mvnpp;
float pnds=0.;
/*Читаем шапку документа*/
sprintf(strsql,"select kontr,kodop,sumkor,datop,osnov,nds,dover,sherez,pn from Usldokum \
where datd='%04d-%02d-%02d' and podr=%d and nomd='%s' and tp=%d",
gg,mg,dg,podr,nomdok,tipz);

if(sql_readkey(&bd,strsql,&row,&cur) != 1)
 {
  iceb_u_str repl;
  
  repl.plus("%s-Не найден документ !",__FUNCTION__),
  
  sprintf(strsql,"Дата:%d.%d.%d Документ:%s Подразделение:%d",
  dg,mg,gg,nomdok,podr);
  
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


usldopnalw(tipz,kodop.ravno(),&procent,&naimnal);

  
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
  iceb_u_str repl;
  repl.plus(gettext("Не найден код операции"));
  repl.plus(" ");
  repl.plus(kodop.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);  
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

class iceb_u_str ispolnitel("");

if(tipz == 1) 
 {
  strncpy(strsql,nmo.ravno(),sizeof(strsql)-1);
  ispolnitel.new_plus(nmo1.ravno());
 }
if(tipz == 2) 
 {
  strncpy(strsql,nmo1.ravno(),sizeof(strsql)-1);
  ispolnitel.new_plus(nmo.ravno());
 } 

if (usl_act_vr(dg,mg,gg,nomdok,strsql,dover.ravno(),ispolnitel.ravno(),ff,wpredok) != 0)
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
class iceb_u_str artikul("");

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
   sprintf(strsql,"select naimat,artikul from Material where kodm=%d",
   kodzap);

  if(metka == 1)
   sprintf(strsql,"select naius,art from Uslugi where kodus=%d",
   kodzap);

  if(sql_readkey(&bd,strsql,&row1,&curr) != 1)
   {
//    printw("%s\n",strsql);
    if(metka == 0)
     sprintf(strsql,"%s %d !",gettext("Не найден код материалла"),kodzap);
    if(metka == 1)
     sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),kodzap);
    iceb_menu_soob(strsql,wpredok);
    naim.new_plus("");
    artikul.new_plus("");
   }
  else
   {
    naim.new_plus(row1[0]);
    if(row[5][0] != '\0')
     {
      naim.plus(" ",row[5]);
     }    
    artikul.new_plus(row1[1]);
   }

  fprintf(ff," %3d %-*.*s %-*.*s %-*.*s %6.6g %8.8g %10.2f\n",
  ++nom_str,
  iceb_u_kolbait(30,naim.ravno()),iceb_u_kolbait(30,naim.ravno()),naim.ravno(),
  iceb_u_kolbait(7,artikul.ravno()),iceb_u_kolbait(7,artikul.ravno()),artikul.ravno(),
  iceb_u_kolbait(6,row[4]),iceb_u_kolbait(6,row[4]),row[4],
  kolih,cena,suma);
  if(iceb_u_strlen(naim.ravno()) > 30)
     fprintf(ff,"%3s %s\n"," ",iceb_u_adrsimv(30,naim.ravno()));
 }
fprintf(ff,"\
-----------------------------------------------------------------------------\n");
int dlina=66;
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

    fprintf(ff,"%*s%10.2f/З %s %.2f\n",dlina+1,
    strsql,sumkor,gettext("НДС"),sumkor+sumkor*pnds/100.);

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
  itogonds=iceb_u_okrug(itogonds,okrg1);
  bros.new_plus(gettext("Итого к оплате:"));
  if(tipz == 1)
    bros.new_plus(gettext("Итого:"));
  fprintf(ff,"%*s%10.2f\n",iceb_u_kolbait(dlina+1,bros.ravno()),bros.ravno(),itogonds);
 }

fprintf(ff,"\n%*s_________________ %*s_________________\n\n",
iceb_u_kolbait(20,gettext("Руководитель")),gettext("Руководитель"),
iceb_u_kolbait(20,gettext("Главный бухгалтер")),gettext("Главный бухгалтер"));

fprintf(ff,"\n%*s_________________ %*s_________________\n",
iceb_u_kolbait(20,gettext("Исполнил")),gettext("Исполнил"),
iceb_u_kolbait(20,gettext("Принял")),gettext("Принял"));

iceb_podpis(ff,wpredok);


fclose(ff);
iceb_ustpeh(imafil->ravno(),1,wpredok);
return(0);

}
/*******************************/
/*Формирование шапки документа*/
/*******************************/

int usl_act_vr(short dd,short md,short gd,
const char *nomdok,
const char *naim_kontr,
const char *dover,
const char *ispolnitel,
FILE *ff,
GtkWidget *wpredok)
{
SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"usl_act_vr.alx"};

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
     iceb_u_vstav(&stroka,nomdok,39,75,1);
     break;

    case 5:
     iceb_u_vstav(&stroka,naim_kontr,10,42,1);
     iceb_u_vstav(&stroka,dover,58,75,1);
     break;

    case 6:
     iceb_u_vstav(&stroka,&naim_kontr[33],1,42,1);
     break;

    case 7:
     iceb_u_vstav(&stroka,ispolnitel,12,42,1);
     break;

    case 8:
     iceb_u_vstav(&stroka,&ispolnitel[31],1,42,1);
     break;

    case 9:
     sprintf(bros,"%02d.%02d.%d",dd,md,gd);
     iceb_u_vstav(&stroka,bros,17,36,1);
     break;
     
   }
  fprintf(ff,"%s",stroka.ravno());
 }
return(0);
}
