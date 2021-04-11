/* $Id: makkorw.c,v 1.12 2013/05/17 14:58:22 sasa Exp $ */
/*12.07.2015	20.01.1999	Белых А.И.	makkorw.c
Работа с файлом настройки для корректировки суммы по документа
Если все в норме возвращаем 0
                            1- ошибка
*/
#include	"buhg_g.h"

extern SQL_baza bd;

int	        makkorw(iceb_u_spisok *MENU,
short tr, //1-создать меню 2-взять настройку для выбранной корректировки
iceb_u_str *naikor, //Наименование корректировки*/
iceb_u_double *maskor,
const char *imaf)
{
class iceb_u_str br(""),br1("");
short		ns,i,i1;

SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }


if(tr == 1)
 {
  ns=0;
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;

    if(iceb_u_SRAV(row_alx[0],"Наименование корректировки",1) == 0)
     {
      iceb_u_pole(row_alx[0],&br,2,'|');
      MENU->plus(br.ravno());
      ns++;
     }
   }
 }

if(tr == 2)
 {
  while(cur_alx.read_cursor(&row_alx) != 0)
   {
    if(row_alx[0][0] == '#')
     continue;

    if(iceb_u_SRAV(row_alx[0],"Наименование корректировки",1) == 0)
     {
      iceb_u_pole(row_alx[0],&br,2,'|');
      if(iceb_u_SRAV(naikor->ravno(),br.ravno(),0) == 0)
       {

        cur_alx.read_cursor(&row_alx);
        iceb_u_pole(row_alx[0],&br,2,'|');
        ns=iceb_u_pole2(br.ravno(),',');        
        if(ns == 0 && br.ravno()[0] != '\0')
         ns=1;
        maskor->make_class(ns*2+1);        
        maskor->plus(ns,0);
        for(i=1; i < ns+1; i++)
         {
          if(iceb_u_pole(br.ravno(),&br1,i,',') != 0)
            br1.new_plus(br.ravno()); 
          maskor->plus(br1.ravno_atof(),i);
         }
        cur_alx.read_cursor(&row_alx);
        iceb_u_pole(row_alx[0],&br,2,'|');
        i1=1;
        for(i=ns+1; i < ns*2+1; i++)
         {
          if(iceb_u_pole(br.ravno(),&br1,i1,',') != 0)
            br1.new_plus(br.ravno()); 
          i1++;
          maskor->plus(br1.ravno_atof(),i);
         }
       }
     }
    
   }
 }


return(0);
}
