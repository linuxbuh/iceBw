/*$Id: zar_pr_insw.c,v 1.3 2013/08/13 06:10:01 sasa Exp $*/
/*13.07.2015	26.11.2011	Белых А.И.	zar_ins_prw.c
Вставка записи в таблицу протокола ход расчёта
*/
#include "buhg_g.h"


void zar_pr_insw(const char *sodzap,GtkWidget *wpredok)
{
extern int zar_pr_nomzap;
extern short zar_pr_mes;
extern short zar_pr_god;
extern int zar_pr_tabn;
extern int zar_pr_tipz;
extern FILE *zar_pr_ffp;

if(zar_pr_ffp != NULL)
 {
  fprintf(zar_pr_ffp,"%s",sodzap);
  return;
 }

if(zar_pr_tipz < 0 && zar_pr_ffp == NULL)
 return;

gchar *strsql=g_strdup_printf("insert into Zarpr values(%d,%d,'%04d-%02d-01',%d,'%s')",zar_pr_tabn,zar_pr_tipz,zar_pr_god,zar_pr_mes,++zar_pr_nomzap,sql_escape_string(sodzap,0));

if(zar_pr_tipz > 0)
 iceb_sql_zapis(strsql,1,0,wpredok);


g_free(strsql);

}
