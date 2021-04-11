/*$Id:$*/
/*07.11.2014	03.10.2014	Белых А. И.	mugetdppw.c
Получение даты последнего прихода по заданному коду материала
*/
#include "buhg_g.h"
#include "mugetdpp.h"

extern SQL_baza bd;

int mugetdppw(int kodmat,class mugetdpp_str *rk,const char *datan,const char *datak,GtkWidget *wpredok )
{
char strsql[1024];
SQL_str row,row1,row2;
class SQLCURSOR cur,cur1,cur2;
int kolstr=0,kolstr1=0;
class iceb_u_str datap("");
int metka_z=0;


rk->clear();

sprintf(strsql,"select sklad,nomk,ei,shetu from Kart where kodm=%d order by sklad asc",kodmat);
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
  return(1);
 }

if(kolstr == 0)
 {
  return(1); /*Нет карточек по материалу*/
 }

while(cur.read_cursor(&row) != 0)
 {
  
  if(datan[0] == '\0')
    sprintf(strsql,"select * from Zkart where sklad=%s and nomk=%s and tipz=1 order by datdp desc limit 1",row[0],row[1]);
  else
   {
    class iceb_u_str dn(iceb_u_tosqldat(datan));
    class iceb_u_str dk(iceb_u_tosqldat(datak));
    sprintf(strsql,"select * from Zkart where datdp >= '%s' and datdp <= '%s' and sklad=%s and nomk=%s and tipz=1  order by datdp desc limit 1",
    dn.ravno(),dk.ravno(),row[0],row[1]);
   }  
    
  if((kolstr1=cur1.make_cursor(&bd,strsql)) < 0)
   {
    iceb_msql_error(&bd,__FUNCTION__,strsql,wpredok);
    return(1);
   }

     
  while(cur1.read_cursor(&row1) != 0)
   {
    metka_z=0;
    datap.new_plus(iceb_u_datzap(row1[3]));

    if(iceb_u_sravmydat(rk->datap.ravno(),datap.ravno()) < 0)
     {
      
      metka_z++;      
      rk->datap.new_plus(datap.ravno());
      rk->cena=atof(row1[7]);
      rk->kolih=atof(row1[6]);
      rk->ei.new_plus(row[2]);
      rk->shetu.new_plus(row[3]);
      rk->sklad=atoi(row[0]);
      rk->nomkart=atoi(row[1]);
            
      //Узнаём код контрагента
      sprintf(strsql,"select kontr from Dokummat where datd='%s' and sklad=%s and nomd='%s'",row1[4],row[0],row1[2]);
      if(iceb_sql_readkey(strsql,&row2,&cur2,wpredok) == 1)
        rk->kodkontr.new_plus(row2[0]);
      else
        rk->kodkontr.new_plus("");

     }         
   }

  if(metka_z > 0)
   {
    rk->dataps.plus(rk->datap.ravno());
    rk->cenas.plus(rk->cena,-1);
    rk->kolihs.plus(rk->kolih,-1);
    rk->eis.plus(rk->ei.ravno());
    rk->shetus.plus(rk->shetu.ravno());
    rk->sklads.plus(rk->sklad,-1);
    rk->nomkarts.plus(rk->nomkart,-1);
    rk->kodkontrs.plus(rk->kodkontr.ravno());
   }
 }

if(rk->sklad == 0)
 return(1); /*не найдено ни одного подходящего прихода*/

return(0);
}
