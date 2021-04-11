/* $Id: vztbw.c,v 1.7 2013/08/13 06:09:58 sasa Exp $ */
/*13.07.2015    05.03.1997      Белых А.И.      vztbw.c
Взять табель на человека за месяц
*/
#include        "buhg_g.h"

extern SQL_baza bd;

void vztbw(int tn,
short m,short g,
short *dn,double *has,
short *kdn,
short met, //0-на экран 1-в файл 2-никуда
FILE *ff,
GtkWidget *view,
GtkTextBuffer *buffer,
GtkWidget *wpredok)
{
long		kolstr;
SQL_str         row,row1;
char		strsql[512];
class iceb_u_str nait("");
float		dnei,hass,kdnei;

*dn=*kdn=0;
*has=0.;

sprintf(strsql,"select kodt,dnei,has,kdnei from Ztab where \
god=%d and mes=%d and tabn=%d",g,m,tn);
SQLCURSOR cur;
SQLCURSOR curr;

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);
  return;
 }

if(kolstr == 0)
 {
  if(met == 0)
   {
    
    sprintf(strsql,gettext("За %d.%dг. табель не введено !"),m,g);
    iceb_printw(strsql,buffer,view);
    
    iceb_printw("\n",buffer,view);
   }
  if(met == 1)
   {
    fprintf(ff,gettext("За %d.%dг. табель не введено !"),m,g);
    fprintf(ff,"\n");
   }
   return;
 }

while(cur.read_cursor(&row) != 0)
 {

  /*Читаем наименование табеля*/
  sprintf(strsql,"select naik from Tabel where kod=%s",
  row[0]);
  
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) == 1)
    nait.new_plus(row1[0]);
  else
    nait.new_plus("");

  dnei=atof(row[1]);
  hass=atof(row[2]);
  kdnei=atof(row[3]);
  if(met == 0)
   {
    sprintf(strsql,"%s %-*s %2.f %3.f %2.f\n",
    row[0],iceb_u_kolbait(30,nait.ravno()),nait.ravno(),atof(row[1]),atof(row[2]),atof(row[3]));
    iceb_printw(strsql,buffer,view);
   
   }
  if(met == 1)
    fprintf(ff,"%3s %-*s д%2.f г%3.f кд%2.f\n",
    row[0],iceb_u_kolbait(40,nait.ravno()),nait.ravno(),dnei,hass,kdnei);
  *dn=(short)(*dn+dnei);
  *has=*has+hass;
  *kdn=(short)(*kdn+kdnei);
 }

}


