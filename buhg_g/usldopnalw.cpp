/*$Id: usldopnalw.c,v 1.11 2013/08/13 06:09:57 sasa Exp $*/
/*13.07.2015	21.03.2002	Белых А.И.	usldopnalw.c
Определение процента и наименования дополнительного налога
Если вернули 0 нашли налог
             1 нет
*/

#include        "buhg_g.h"

extern SQL_baza bd;

int usldopnalw(int tipz, // 1+ 2 -
const char *kodop, //Код операции
float *procent,
class iceb_u_str *naim)
{
class iceb_u_str bros("");
int	vozvr=1;

*procent=0.;


SQL_str row_alx;
class SQLCURSOR cur_alx;
int kolstr=0;
char strsql[512];
const char *imaf_alx={"uslnast.alx"};

sprintf(strsql,"select str from Alx where fil='%s' order by ns asc",imaf_alx);
if((kolstr=cur_alx.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,NULL);
  return(1);
 }

if(kolstr == 0)
 {
  sprintf(strsql,"%s %s!",gettext("Не найдена настройка"),imaf_alx);
  iceb_menu_soob(strsql,NULL);
  return(1);
 }

while(cur_alx.read_cursor(&row_alx) != 0)
 {
  if(row_alx[0][0] == '#')
   continue;
  
  if(iceb_u_polen(row_alx[0],&bros,2,'|') != 0)
    continue;
  if(tipz == 1 && iceb_u_strstrm(bros.ravno(),"+") != 1)
    continue;
  if(tipz == 2 && iceb_u_strstrm(bros.ravno(),"-") != 1)
    continue;
    
  if(iceb_u_polen(row_alx[0],&bros,3,'|') != 0)
    continue;
  if(iceb_u_proverka(bros.ravno(),kodop,0,1) != 0)
    continue;

  if(iceb_u_polen(row_alx[0],&bros,4,'|') != 0)
    continue;
  *procent=bros.ravno_atof();
  if(iceb_u_polen(row_alx[0],&bros,5,'|') != 0)
    continue;
  naim->new_plus(bros.ravno());
  vozvr=0;
  break;  
 }
 
return(vozvr);
}
