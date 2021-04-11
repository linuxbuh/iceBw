/*$Id: poizardokw.c,v 1.9 2013/05/17 14:58:23 sasa Exp $*/
/*24.11.2016	27.07.2008	Белых А.И.	poizardokw.c
Поиск документа в подсистеме заработной платы по номеру документа
*/
#include "buhg_g.h"

void  l_poizardok(const char *nomdok,GtkWidget *wpredok);


void poizardokw(GtkWidget *wpredok)
{

iceb_u_str repl;

repl.plus(gettext("Введите номер документа"));
iceb_u_str nomdok;

if(iceb_menu_vvod1(&repl,&nomdok,20,"",wpredok) != 0)
 return;

char strsql[512];
SQL_str row;
SQLCURSOR cur;

sprintf(strsql,"select datd,nomd,td,god,datsd from Zardok where nomd='%s' order by datd desc",nomdok.ravno());

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
  
  class iceb_u_str datadok(iceb_u_datzap(row[0]));
  
  short prn=atoi(row[2]);
  l_zar_dok_zap(prn,atoi(row[3]),datadok.ravno(),row[1],wpredok);
    


  return;
 }

l_poizardok(nomdok.ravno(),wpredok);

}
