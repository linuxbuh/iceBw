/*$Id: poixdkdokw.c,v 1.9 2013/05/17 14:58:23 sasa Exp $*/
/*12.07.2015	14.10.2005	Белых А.И.	poixdkdokw.c
Поиск документа в платёжных документах по номеру документа
*/
#include "buhg_g.h"

void  l_poixdkdok(const char *tablica,const char *nomdok,GtkWidget *wpredok);


void poixdkdokw(const char *tablica,GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,"",wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd,nomd from %s where nomd='%s' \
order by datd desc",tablica,nomdok.ravno());


int koldok=iceb_sql_readkey(strsql,&row,&cur,wpredok);

if(koldok == 0)
 {
  repl.new_plus(gettext("Не найдено ни одного документа с номером"));
  repl.plus(" ");
  repl.plus(nomdok.ravno());
  repl.plus(" !");
  iceb_menu_soob(&repl,wpredok);
  return;
 }

if(koldok == 1)
 {
  
  short d,m,g;
  iceb_u_rsdat(&d,&m,&g,row[0],2);
  
  readpdokw(tablica,g,nomdok.ravno(),wpredok);

  xdkdok(tablica,wpredok);

  return;
 }

l_poixdkdok(tablica,nomdok.ravno(),wpredok);

}


