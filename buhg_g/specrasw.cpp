/*$Id: specrasw.c,v 1.11 2013/08/13 06:09:53 sasa Exp $*/
/*11.03.2017	17.05.2005	Белых А.И.	specrasw.c
Расчёт разузлования изделия
*/
#include "buhg_g.h"
#include "specrasw.h"

int specrasw_r(class specrasw_rr*,class spis_oth *oth,GtkWidget*);



void specrasw(const char *kod_izdel,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
SQLCURSOR cur;
//Читаем наименование изделия
sprintf(strsql,"select naimat from Material where kodm=%s",kod_izdel);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  sprintf(strsql,"specrasw-%s %s !",gettext("Не найден код материалла"),kod_izdel);
  iceb_menu_soob(strsql,wpredok);
  return;
 }

class specrasw_rr data;
data.kod_izdel.new_plus(kod_izdel);
data.naim_izdel.new_plus(row[0]);


data.kolih.new_plus("");
iceb_u_spisok repl;
repl.plus(gettext("Расчёт количества материалов на изделие"));

sprintf(strsql,"%s: %s %s",gettext("Изделие"),
data.kod_izdel.ravno(),
data.naim_izdel.ravno());

repl.plus(strsql);

repl.plus(gettext("Введите количество изделий для которого нужно сделать расчёт"));

if(iceb_menu_vvod1(&repl,&data.kolih,20,"",wpredok) != 0)
 return; 
class spis_oth oth;

if(specrasw_r(&data,&oth,wpredok) != 0)
 return;

//iceb_rabfil(&data.imaf,&data.naimf,wpredok);
iceb_rabfil(&oth,wpredok);

}
