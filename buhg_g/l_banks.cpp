/* $Id: l_banks.c,v 1.14 2014/07/31 07:08:26 sasa Exp $ */
/*11.07.2015	12.05.2001	Белых А.И.	l_banks.c
Создание меню выбора банка
Если вернули 0 выбрали банк
*/
#include        "buhg_g.h"

extern SQL_baza bd;

int l_banks(int *nomb,//Номер банка
class iceb_u_str *imamah,//Имя машины в сети
class iceb_u_str *katalog, //Каталог
class iceb_u_str *imafil, //Имя файла для импорта
class iceb_u_str *parol, //Пароль
class iceb_u_str *comanda,
class iceb_u_str *script,
GtkWidget *wpredok)
{
class iceb_u_str stroka("");
int	kolbank=0;
char	bros[1024];
int	nomer=0;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='banki.alx' order by ns asc");
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),"banki.alx");
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }



//Создаем класс
class iceb_u_spisok NASTR;
class iceb_u_spisok stroks_menu;

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),2,'|') != 0)
   continue;  
  if((*nomb=atoi(bros)) == 0)
   continue;

  if(iceb_u_polen(row_alx[0],bros,sizeof(bros),1,'|') != 0)
    continue;  

  stroks_menu.plus(bros);
  if(iceb_u_pole2(row_alx[0],'|') == 3) //команда
   {
    iceb_u_polen(row_alx[0],comanda,3,'|');
    sprintf(bros,"%d|%s",*nomb,comanda->ravno());
    NASTR.plus(bros);
   }
  else
   {

    if(iceb_u_pole(row_alx[0],imamah,3,'|') != 0)
      continue;  
    if(iceb_u_pole(row_alx[0],katalog,4,'|') != 0)
      continue;  
    if(iceb_u_pole(row_alx[0],imafil,5,'|') != 0)
      continue;  
    if(iceb_u_pole(row_alx[0],parol,6,'|') != 0)
      continue;  
    iceb_u_polen(row_alx[0],script,7,'|');

    sprintf(bros,"%d|%s|%s|%s|%s|%s",*nomb,imamah->ravno(),katalog->ravno(),imafil->ravno(),parol->ravno(),script->ravno());
    NASTR.plus(bros);
   }
 }

kolbank=stroks_menu.kolih();

if(kolbank == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одного банка !"),wpredok);
  return(1);
 }

if(kolbank > 1)
 {
  iceb_u_str titl;
  iceb_u_str zagolovok;
  iceb_u_spisok punkt_m;

  titl.plus(gettext("Выбор"));


  zagolovok.plus(gettext("Выберите нужное"));

  for(int ii=0; ii < kolbank ; ii++)
    punkt_m.plus(stroks_menu.ravno(ii));


  nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,wpredok);
  if(nomer == -1)
   return(1);
 }
else
 nomer=0;

stroka.new_plus(NASTR.ravno(nomer));
iceb_u_polen(stroka.ravno(),nomb,1,'|');
iceb_u_polen(stroka.ravno(),comanda,2,'|');

if(iceb_u_pole2(stroka.ravno(),'|') > 3)
 {
  iceb_u_pole(stroka.ravno(),imamah,2,'|');
  iceb_u_pole(stroka.ravno(),katalog,3,'|');
  iceb_u_pole(stroka.ravno(),imafil,4,'|');
  iceb_u_pole(stroka.ravno(),parol,5,'|');
  iceb_u_polen(stroka.ravno(),script,6,'|');
  comanda->new_plus("");
 }

/**********
iceb_u_pole(NASTR.ravno(nomer),bros,1,'|');
*nomb=atoi(bros);

iceb_u_pole(NASTR.ravno(nomer),imamah,2,'|');
iceb_u_pole(NASTR.ravno(nomer),katalog,3,'|');
iceb_u_pole(NASTR.ravno(nomer),imafil,4,'|');
iceb_u_pole(NASTR.ravno(nomer),parol,5,'|');
**************/

return(0);
}


