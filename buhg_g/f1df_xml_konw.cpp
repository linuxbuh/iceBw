/*$Id: f1df_xml_konw.c,v 1.11 2013/05/17 14:58:20 sasa Exp $*/
/*16.03.2015	23.04.2007	Белых А.И.	f1df_xml_konw.c
Концовка xml файла формы 1ДФ
*/

#include "buhg_g.h"

void f1df_xml_konw(double isumadn,
double isumad,
double isumann,
double isuman,
int nomer_xml_str,
int kolr,
int klls,
const char *naim_kontr,
const char *telef_kontr,
int TYP,
double doh_nah_vs, /*Общая сумма начисленного дохода для расчёта военного сбора*/
double doh_vip_vs, /*Общая сумма выплаченного дохода для расчёта военного сбора*/
double vs_nah, /*военный сбор начисленный*/
double vs_vip, /*военній сбор перечисленный*/
FILE *ff_xml,
GtkWidget *wpredok)
{
fprintf(ff_xml,"  <R01G03A>%.2f</R01G03A>\n",isumadn);
fprintf(ff_xml,"  <R01G03>%.2f</R01G03>\n",isumad);
fprintf(ff_xml,"  <R01G04A>%.2f</R01G04A>\n",isumann);
fprintf(ff_xml,"  <R01G04>%.2f</R01G04>\n",isuman);

fprintf(ff_xml,"  <R02G01I>%d</R02G01I>\n",nomer_xml_str);
fprintf(ff_xml,"  <R02G02I>%d</R02G02I>\n",kolr);
fprintf(ff_xml,"  <R02G03I>%d</R02G03I>\n",klls);

fprintf(ff_xml,"  <R0205G03A>%.2f</R0205G03A>\n",doh_nah_vs);
fprintf(ff_xml,"  <R0205G03>%.2f</R0205G03>\n",doh_vip_vs);
fprintf(ff_xml,"  <R0205G04A>%.2f</R0205G04A>\n",vs_nah);
fprintf(ff_xml,"  <R0205G04>%.2f</R0205G04>\n",vs_vip);
class iceb_u_str bros("");
class iceb_u_str fioruk("");
class iceb_u_str telef("");
  
class iceb_u_str imaf_nzar("zarnast.alx");

iceb_poldan("Табельный номер руководителя",&bros,imaf_nzar.ravno(),wpredok);
class iceb_u_str inn("");
char strsql[512];
SQL_str row1;
class SQLCURSOR curr;
if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"%s-%s %s!",
    __FUNCTION__,
    gettext("Не найден табельный номер руководителя"),
    bros.ravno());
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    fioruk.new_plus(row1[0]);
    inn.new_plus(row1[1]);
    telef.new_plus(row1[2]);
   }
 } 
if(inn.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HKBOS>%s</HKBOS>\n",inn.ravno());
if(fioruk.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HBOS>%s</HBOS>\n",fioruk.ravno_filtr_xml());
if(telef.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HTELBOS>%s</HTELBOS>\n",telef.ravno_udsimv("-/.()+,"));

iceb_poldan("Табельный номер бухгалтера",&bros,imaf_nzar.ravno(),wpredok);
fioruk.new_plus("");
inn.new_plus("");
telef.new_plus("");

if(bros.ravno()[0] != '\0')
 {
  sprintf(strsql,"select fio,inn,telef from Kartb where tabn=%s",bros.ravno());
  if(iceb_sql_readkey(strsql,&row1,&curr,wpredok) != 1)
   {
    sprintf(strsql,"%s-%s %s!",
    __FUNCTION__,
    gettext("Не найден табельный номер главного бухгалтера"),
    bros.ravno());
    iceb_menu_soob(strsql,wpredok);
   }
  else
   {
    fioruk.new_plus(row1[0]);
    inn.new_plus(row1[1]);
    telef.new_plus(row1[2]);
   }
 }
if(inn.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HKBUH>%s</HKBUH>\n",inn.ravno());
if(fioruk.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HBUH>%s</HBUH>\n",fioruk.ravno_filtr_xml());
if(telef.ravno()[0] != '\0')
  fprintf(ff_xml,"  <HTELBUH>%s</HTELBUH>\n",telef.ravno_udsimv("-/.()+,"));

short dt,mt,gt;
iceb_u_poltekdat(&dt,&mt,&gt);

fprintf(ff_xml,"  <HFILL>%02d%02d%04d</HFILL>\n",dt,mt,gt);


if(TYP == 1)
 {
  if(naim_kontr[0] != '\0')
    fprintf(ff_xml,"  <HFO>%s<HFO>\n",iceb_u_filtr_xml(naim_kontr));
  if(telef_kontr[0] != '\0')
    fprintf(ff_xml,"  <HTELFO>%s<HTELFO>\n",iceb_u_filtr_xml(telef_kontr));
 }

fprintf(ff_xml," </DECLARBODY>\n");
fprintf(ff_xml,"</DECLAR>\n");
fclose(ff_xml);

}
