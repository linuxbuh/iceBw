/*$Id: xdksend.c,v 1.24 2013/08/13 06:09:58 sasa Exp $*/
/*18.10.2019	17.05.2006	Белых А.И.	xdksend.c
Формирование файла для передачи в подсистемы Банк-Клиент
*/
#include "buhg_g.h"

int l_banks(int *nomb,class iceb_u_str *imamah,class iceb_u_str *katalog,class iceb_u_str *imafil,class iceb_u_str *parol,class iceb_u_str*,class iceb_u_str*,GtkWidget *wpredok);
int b_prominvest(const char *tabl,GtkWidget *wpredok);
int b_nadra(const char *tabl,GtkWidget *wpredok);
int b_aval(const char *tabl,GtkWidget *wpredok);
int b_ukrinbank(const char *tabl,GtkWidget *wpredok);
int b_privatinvestw(const char *tabl,GtkWidget *wpredok);
int b_brokbiznesw(const char *tabl,GtkWidget *wpredok);
int b_pervinvw(const char *tabl,GtkWidget *wpredok);
int b_ukrpromw(const char *tabl,GtkWidget *wpredok);
int b_ukreximw(const char *tabl,GtkWidget *wpredok);
int b_b2_bank_on_linew(const char *tabl,GtkWidget *wpredok);
int b_ukrsocw(const char *tabl,GtkWidget *wpredok);
int b_iBank_2_UAw(const char *tabl,GtkWidget *wpredok);
int b_privatw(const char *tabl,GtkWidget *wpredok);
int b_tinyw(const char *tabl,GtkWidget *wpredok);
int b_soft_revieww(const char *tabl,GtkWidget *wpredok);
int b_skarbw(const char *tabl,GtkWidget *wpredok);
int b_ukrbiznesw(const char *tabl,GtkWidget *wpredok);
int b_akb_imexbankw(const char *tabl,GtkWidget *wpredok);
int b_corp2w(const char *tabl,class iceb_u_str *imafdbf_out,GtkWidget *wpredok);
int b_stilw(const char *tabl,GtkWidget *wpredok);

extern SQL_baza bd;

void xdksend(const char *tablica,GtkWidget *wpredok)
{
char bros[2048];
char strsql[512];

//printf("%s-tablica=%s\n",__FUNCTION__,tablica);

sprintf(strsql,"select datd,nomd from %s where vidpl='1' limit 1",tablica);
if(iceb_sql_readkey(strsql,wpredok) <= 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного документа отмеченного для передчи в банк"),wpredok);
  return;
 }


int nomerb=0;
class iceb_u_str imamah("");
class iceb_u_str katalog("");
class iceb_u_str imafil("");
class iceb_u_str parol("");
class iceb_u_str comanda("");
class iceb_u_str script("");

if(l_banks(&nomerb,&imamah,&katalog,&imafil,&parol,&comanda,&script,wpredok) != 0)
  return;

if(script.getdlinna() <= 1)
 script.new_plus("bankcp");
 
/*
    printw("\nnomerb=%d imamah=%s katalog=%s imafil=%s\n",
    nomerb,imamah,katalog,imafil);
    OSTANOV();
*/
if(nomerb == 1) //Проминвест банк
  b_prominvest(tablica,wpredok);
if(nomerb == 2) //Банк Надра
  b_nadra(tablica,wpredok);
if(nomerb == 3) //Банк Аваль
  b_aval(tablica,wpredok);
if(nomerb == 4) //Укринбанк
  b_ukrinbank(tablica,wpredok);
if(nomerb == 5) //Приватинвест
  b_privatinvestw(tablica,wpredok);
if(nomerb == 6) //Брокбизнесбанк
  b_brokbiznesw(tablica,wpredok);
if(nomerb == 7) //Первый инвестиционный
  b_pervinvw(tablica,wpredok);

if(nomerb == 8) //Укрпромбанк
 {
  
  b_ukrpromw(tablica,wpredok);

  //Узнаём номер дня недели
  struct tm *bf;
  time_t vrem;
  time(&vrem);
  bf=localtime(&vrem);
  sprintf(bros,"%d",bf->tm_wday+1); //Нумерация дней недели начинаестся с ноля
  //Третий знак в имени файла номер дня недели
//  imafil[2]=bros[0]; 
  iceb_u_vstav(&imafil,bros,2,2,1);
  if(iceb_menu_vvod1(gettext("Введите имя файла"),&imafil,64,"",wpredok) != 0)
     return;

 }


if(nomerb == 9) //УкрЕсим банк
  b_ukreximw(tablica,wpredok);
if(nomerb == 10) //Б2 bank on line
  b_b2_bank_on_linew(tablica,wpredok);
if(nomerb == 11) //Укрсоцбанк
  b_ukrsocw(tablica,wpredok);
if(nomerb == 12) //iBank 2 UA
  b_iBank_2_UAw(tablica,wpredok);

if(nomerb == 13) //Приват Банк
 {
  b_privatw(tablica,wpredok);

/***********************
  if(b_privatw(tablica,wpredok) == 0)
   {
    iceb_menu_soob("Платёжки записаны в таблицу JBKL_SND !",wpredok);
    unlink("plat.txt");
    //Снимаем метку передачи в банк
    sprintf(strsql,"update %s set vidpl='' where vidpl='1'",tablica);
    iceb_sql_zapis(strsql,1,0,wpredok);
   }
  return;
************************/
 }

if(nomerb == 14) //финансы и кредит
  b_soft_revieww(tablica,wpredok);

if(nomerb == 15) //система клиент-банк "Tiny"
  b_tinyw(tablica,wpredok);

if(nomerb == 16) //система клиент-банк "СКАРБ"
  b_skarbw(tablica,wpredok);

if(nomerb == 17) //система клиент-банк для бана Укрбизнесбанк
  b_ukrbiznesw(tablica,wpredok);

if(nomerb == 18)
 b_akb_imexbankw(tablica,wpredok);

if(nomerb == 19) /*CORP2*/
 b_corp2w(tablica,&imafil,wpredok);

if(nomerb == 20) /*"Стиль"*/
 b_stilw(tablica,wpredok);

int i=0;
if(comanda.getdlinna() <= 1)
 {
  sprintf(bros,"%s %s %s %s %s %s",script.ravno(),imamah.ravno(),"plat.txt",imafil.ravno(),katalog.ravno(),parol.ravno());

  i=system(bros);
 }
else 
 {
  iceb_u_zvstr(&comanda,"ICEB_FIL",imafil.ravno());
  i=system(comanda.ravno());
 }
iceb_u_str repl;
if(i != 0)
 {
  
  sprintf(bros,gettext("Передача файла не произошла ! Код ошибки %d"),i);

  repl.plus(bros);
  if(comanda.getdlinna() <= 1)
   {
    sprintf(bros,"%d %s %s %s %s %s %s",nomerb,imamah.ravno(),"plat.txt",imafil.ravno(),katalog.ravno(),parol.ravno(),script.ravno());
    repl.ps_plus(bros);
   }
  else
   repl.ps_plus(comanda.ravno());
  iceb_menu_soob(&repl,wpredok);
 }
if(i == 0)
 {
  
  repl.plus(gettext("Передача файла произошла успешно."));
  if(comanda.getdlinna() <= 1)
    repl.ps_plus(imafil.ravno());
  
  iceb_menu_soob(&repl,wpredok);
  
  unlink("plat.txt");
  /*Снимаем метку передачи в банк*/
  sprintf(strsql,"update %s set vidpl='' where vidpl='1'",tablica);
  iceb_sql_zapis(strsql,1,0,wpredok);
 }     


}
