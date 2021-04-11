/*$Id: str_nn_xml.c,v 5.9 2013/05/17 14:56:12 sasa Exp $*/
/*06.12.2018	17.03.2016	Белых А.И.	iceb_nn10_str_xml.c
Выгрузка строки налоговой накладной в формате xml.
*/
#include "iceb_libbuh.h"

extern class xml_rek_nn_data rek_zag_nn;

void iceb_nn10_str_xml(int nomer,double kolih,double cena,double suma,const char *ei,
const char *naim_mat,const char *kodtov,const char *kod_stavki,const char *kod_lgoti,
int metka_imp_tov, /*метка импортного товара*/
const char *ku,    /*код послуги згідно з ДКПП*/
const char *kdstv, /*код виду сільскогосподарського товаровиробника*/
double spnv, /*сумма податку на додану вартість*/
FILE *ff_xml,
GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;
class iceb_u_str kodei("");


/*Узнаём код единицы измерения*/
sprintf(strsql,"select kei from Edizmer where kod='%s'",sql_escape_string(ei,0));
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  kodei.new_plus(row[0]); 




fprintf(ff_xml,"  <RXXXXG3S ROWNUM=\"%d\">%s</RXXXXG3S>\n",nomer,iceb_u_filtr_xml(naim_mat));


if(kodtov[0] != '\0')
 fprintf(ff_xml,"  <RXXXXG4 ROWNUM=\"%d\">%s</RXXXXG4>\n",nomer,kodtov);
else
 fprintf(ff_xml,"  <RXXXXG4 ROWNUM=\"%d\" xsi:nil=\"true\" />\n",nomer);

fprintf(ff_xml,"  <RXXXXG32 ROWNUM=\"%d\">%d</RXXXXG32>\n",nomer,metka_imp_tov);

if(ku[0] != '\0')
 fprintf(ff_xml,"  <RXXXXG33 ROWNUM=\"%d\">%s</RXXXXG33>\n",nomer,ku);



fprintf(ff_xml,"  <RXXXXG4S ROWNUM=\"%d\">%s</RXXXXG4S>\n",nomer,iceb_u_filtr_xml(ei));


if(kodei.getdlinna() > 1)
 fprintf(ff_xml,"  <RXXXXG105_2S ROWNUM=\"%d\">%s</RXXXXG105_2S>\n",nomer,iceb_u_filtr_xml(kodei.ravno()));
else
 fprintf(ff_xml,"  <RXXXXG105_2S ROWNUM=\"%d\" xsi:nil=\"true\" />\n",nomer);


fprintf(ff_xml,"  <RXXXXG5 ROWNUM=\"%d\">%.10g</RXXXXG5>\n",nomer,kolih);
fprintf(ff_xml,"  <RXXXXG6 ROWNUM=\"%d\">%s</RXXXXG6>\n",nomer,iceb_prcn(cena));


if(kod_stavki[0] != '\0')
 fprintf(ff_xml,"  <RXXXXG008 ROWNUM=\"%d\">%s</RXXXXG008>\n",nomer,kod_stavki);
else
 fprintf(ff_xml,"  <RXXXXG008 ROWNUM=\"%d\" xsi:nil=\"true\" />\n",nomer);


if(kod_lgoti[0] != '\0')
 fprintf(ff_xml,"  <RXXXXG009 ROWNUM=\"%d\">%s</RXXXX009>\n",nomer,kod_stavki);
else
 fprintf(ff_xml,"  <RXXXXG009 ROWNUM=\"%d\" xsi:nil=\"true\" />\n",nomer);
 

fprintf(ff_xml,"  <RXXXXG010 ROWNUM=\"%d\">%.2f</RXXXXG010>\n",nomer,suma);

if(kdstv[0] != '\0')
 fprintf(ff_xml,"  <RXXXXG011 ROWNUM=\"%d\">%s</RXXXXG011>\n",nomer,kdstv);

fprintf(ff_xml,"  <RXXXXG11_10 ROWNUM=\"%d\">%.10g</RXXXXG11_10>\n",nomer,spnv);

}
