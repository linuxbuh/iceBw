/*$Id:$*/
/*09.10.2015	22.09.2015	Белых А.И.	iceb_vstavfil.c
Вставка в строки заготовки и запись в файл
*/
#include "iceb_libbuh.h"
extern SQL_baza bd;

int iceb_vstavfil(class iceb_vstavfil_rk *rek,const char *naimfil,FILE *ff,GtkWidget *wpredok)
{
char strsql[1024];
int kolstr=0;
class SQLCURSOR cur;
SQL_str row;

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",naimfil);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s-%s %s !",__FUNCTION__,gettext("Не найден файл"),naimfil);
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }
int nom_str=0;
class iceb_u_str stroka("");

while(cur.read_cursor(&row) != 0)
 {
  if(row[0][0] == '#')
   continue;
  nom_str++;
  stroka.new_plus(row[0]);
    
  for(int nom=0; nom < rek->str.kolih(); nom++)
   {
    if(rek->nom_str.ravno(nom) == nom_str)
      iceb_u_vstav(&stroka,rek->str.ravno(nom),rek->poz_begin.ravno(nom),rek->poz_end.ravno(nom),rek->hag.ravno(nom));
   }
  fprintf(ff,"%s",stroka.ravno());
    
  
 }
 
return(nom_str);
}
