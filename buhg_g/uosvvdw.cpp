/*$Id: uosvvdw.c,v 1.2 2013/09/26 09:46:57 sasa Exp $*/
/*13.07.2015	19.06.2013	Белых А.И.	uosvvd.c
Вставка в документ всех основных средств, которые числятся за подразделением/материально ответственным
*/
#include "buhg_g.h"

extern SQL_baza bd;

void uosvvd(int tipz,short dd,short md,short gd,const char *nomdok,int podr,int kodmol,const char *kodop,int podr1,int kodmol1,const char *nomdokp,GtkWidget *wpredok)
{
char strsql[1024];
SQL_str row;
class SQLCURSOR cur;
int kolstr=0;
int vido=0;

if(tipz == 1)
 {
  iceb_menu_soob(gettext("Это возможно только для расходных документов!"),wpredok);
  return;
 }

sprintf(strsql,"select vido from Uosras where kod='%s'",kodop);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"%s-%s %s!",__FUNCTION__,gettext("Не найден код операции"),kodop);
  iceb_menu_soob(strsql,wpredok);
  return;
 }
vido=atoi(row[0]);
int kol=1;
if(vido == 2)
 kol=0;

sprintf(strsql,"select innom from Uosin");

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  iceb_menu_soob(gettext("Не найдено ни одной записи !"),wpredok);
  return;
 }
int ktoi=iceb_getuid(wpredok);
float kolstr1=0.;
int innom=0;
int podrp=0,kodotl=0;
class bsizw_data bal_st;

class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,wpredok);

while(cur.read_cursor(&row) != 0)
 {
//  strzag(LINES-1,0,kolstr,++kolstr1);
  iceb_pbar(gdite.bar,kolstr,++kolstr1);

  innom=atoi(row[0]);
  
  if(poiinw(innom,dd,md,gd,&podrp,&kodotl,wpredok) != 0)
   continue;

  if(podr != podrp || kodmol != kodotl)
   continue;

  bsizw(innom,podr,dd,md,gd,&bal_st,NULL,wpredok);

/****************
strcpy(strsql,"CREATE TABLE Uosdok1 (\
datd	DATE not null default '0000-00-00',\
tipz	smallint not null default 0,\
podt	smallint not null default 0,\
innom   int unsigned not null default 0,\
nomd	varchar(20) not null default '',\
podr	smallint not null default 0,\
kodol   int not null default 0,\
kol     smallint not null default 0,\
bs	double(12,2) not null default 0,\
iz	double(12,2) not null default 0,\
kind	float not null default 0,\
kodop   varchar(20) not null default '',\
ktoz	smallint unsigned not null default 0,\
vrem	int unsigned not null default 0,\
bsby	double(12,2) not null default 0,\
izby    double(12,2) not null default 0,\
cena	double(14,2) not null default 0,\
shs     varchar(20) not null default '',\
unique(datd,nomd,innom),\
index(innom,datd)) ENGINE = MYISAM");

0  datd	  дата документа
1  tipz	  1-приход 2-расход
2  podt	  0-не подтверждена 1-подтверждена
3  innom  инвентарный номер
4  nomd	  номер документа
5  podr   подразделение
6  kodol  код ответственного лица
7  kol    количество : +1 приход -1 расход 0 изменение стоимости
8  bs	  балансовая стоимость для налогового учёта
9  iz	  износ для налогового учёта
10 kind	  коэффициент индексации
11 kodop  код операции
12 ktoz   кто записал
13 vrem   время записи
14 bsby	  балансовая стоимость для бух-учета
15 izby	  износ для бух-учета
16 cena   цена продажи
17 shs    счёт получения/списания
*/

  sprintf(strsql,"insert into Uosdok1 (datd,tipz,podt,innom,nomd,podr,kodol,kol,bs,iz,kind,kodop,ktoz,vrem,bsby,izby,cena,shs) \
values ('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,%.2f,%f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",
  gd,md,dd,tipz,0,innom,nomdok,podr,kodmol,kol*-1,bal_st.kbsnu,bal_st.kiznu,0.,kodop,ktoi,time(NULL),bal_st.kbsbu,bal_st.kizbu,0.,"");
  
  iceb_sql_zapis(strsql,1,0,wpredok);

  if(nomdokp[0] != '\0')
   {
    sprintf(strsql,"insert into Uosdok1 (datd,tipz,podt,innom,nomd,podr,kodol,kol,bs,iz,kind,kodop,ktoz,vrem,bsby,izby,cena,shs) \
  values ('%04d-%02d-%02d',%d,%d,%d,'%s',%d,%d,%d,%.2f,%.2f,%f,'%s',%d,%ld,%.2f,%.2f,%.2f,'%s')",
    gd,md,dd,1,0,innom,nomdokp,podr1,kodmol1,kol,bal_st.kbsnu,bal_st.kiznu,0.,kodop,ktoi,time(NULL),bal_st.kbsbu,bal_st.kizbu,0.,"");
    
    iceb_sql_zapis(strsql,1,0,wpredok);
   }     
 }








}
