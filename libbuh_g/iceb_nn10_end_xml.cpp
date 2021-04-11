/*$Id: end_nn_xml.c,v 5.8 2014/07/31 07:09:16 sasa Exp $*/
/*06.12.2018	17.03.2016	Белых А.И.	iceb_nn10_end_xml.cpp
Концовка файла с налоговой накладной в формате xml
*/
#include "iceb_libbuh.h"

void iceb_nn10_end_xml(const char *kodop,
const char *imaf_nastr,
int lnds,
double sumdok,
double suma_nds,
double suma_vt,
FILE *ff_xml,
GtkWidget *wpredok)  
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_u_str fio(iceb_getfioop(wpredok));
class iceb_u_str inn_op("");

fprintf(ff_xml,"  <R04G11>%.2f</R04G11>\n",sumdok);

if(suma_nds != 0.)
 fprintf(ff_xml,"  <R03G11>%.2f</R03G11>\n",suma_nds);
else
 fprintf(ff_xml,"  <R03G11 xsi:nil=\"true\" />\n");

if(lnds == 0) //20% НДС
 {
  fprintf(ff_xml,"  <R03G7>%.2f</R03G7>\n",suma_nds);
  fprintf(ff_xml,"  <R03G109 xsi:nil=\"true\" />\n");
  fprintf(ff_xml,"  <R01G7>%.2f</R01G7>\n",sumdok-suma_nds);
  fprintf(ff_xml,"  <R01G109 xsi:nil=\"true\" />\n");
  fprintf(ff_xml,"  <R01G9 xsi:nil=\"true\" />\n");
  fprintf(ff_xml,"  <R01G8 xsi:nil=\"true\" />\n");
  fprintf(ff_xml,"  <R01G10 xsi:nil=\"true\" />\n");
 }

if(lnds == 1) //0% на територии Украины
 {
  fprintf(ff_xml,"  <R01G8>%.2f</R01G8>\n",sumdok);
 }
if(lnds == 2) //0% экспорт
 {
  fprintf(ff_xml,"  <R01G9>%.2f</R01G9>\n",sumdok);
 }
if(lnds == 3) //звільнено від ПДВ
 {
  fprintf(ff_xml,"  <R01G10>%.2f</R01G10>\n",sumdok);
 }
if(lnds == 4) //7% НДС
 {
  fprintf(ff_xml,"  <R03G109>%.2f</R03G109>\n",suma_nds);
  fprintf(ff_xml,"  <R01G109>%.2f</R01G109>\n",sumdok);
 }

if(suma_vt != 0.)
 fprintf(ff_xml,"  <R02G11>%.2f</R02G11>\n",suma_vt);
else
 fprintf(ff_xml,"  <R02G11 xsi:nil=\"true\" />\n");





/*Узнаём индивидуальный налоговый номер оператора*/
sprintf(strsql,"select tn from icebuser where login='%s'",iceb_u_getlogin());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(atoi(row[0]) != 0)
   {
    class iceb_fioruk_rk rp;
    if(iceb_get_rtn(atoi(row[0]),&rp,wpredok) == 0)
     {
      inn_op.new_plus(rp.inn.ravno());
      fio.new_plus(rp.fio.ravno());
     }
   }
 }

if(fio.getdlinna() > 1)
 fprintf(ff_xml,"  <HBOS>%s</HBOS>\n",fio.ravno_filtr_xml());
else
 fprintf(ff_xml,"  <HBOS xsi:nil=\"true\" />\n");

if(inn_op.getdlinna() > 1)
 fprintf(ff_xml,"  <HKBOS>%s</HKBOS>\n",inn_op.ravno_filtr_xml());
else
 fprintf(ff_xml,"  <HKBOS xsi:nil=\"true\" />\n");

if(kodop[0] != '\0' && imaf_nastr[0] != '\0')
 {
  class iceb_u_str punkt_zak("");
  class iceb_u_str naim_nast("Пункт закона освобождения от НДС для операции ");
  naim_nast.plus(kodop);

  iceb_poldan(naim_nast.ravno(),&punkt_zak,imaf_nastr,wpredok);
  if(punkt_zak.getdlinna() > 1)
    fprintf(ff_xml,"  <R003G10S>%s</R003G10S>\n",punkt_zak.ravno_filtr_xml());
  else
    fprintf(ff_xml,"  <R003G10S xsi:nil=\"true\" />\n");
 } 
else
  fprintf(ff_xml,"  <R003G10S xsi:nil=\"true\" />\n");

fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");

}
