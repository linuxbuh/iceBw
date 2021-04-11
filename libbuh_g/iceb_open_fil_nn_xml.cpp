/*$Id: open_fil_nn_xmlw.c,v 1.9 2014/02/28 05:21:00 sasa Exp $*/
/*08.04.2016	13.10.2006	Белых А.И.	iceb_open_fil_nn_xmlw.c
Открытие файла для налогового документа в фармате xml
*/
#include <errno.h>
#include "iceb_libbuh.h"

extern class xml_rek_nn_data rek_zag_nn;
extern iceb_u_str kat_for_nal_nak; //Каталог для налоговых накладных

int iceb_open_fil_nn_xml(char *imaf_xml,int pnd,int tipz,
short mn,short gn,
class iceb_rnfxml_data *rek_zag_nn,
FILE **ff_xml,
GtkWidget *wpredok)
{

if(kat_for_nal_nak.getdlinna() > 1)
 sprintf(imaf_xml,"%s%s%.4s%010d%s100%07d%1d%02d%04d%4.4s.XML",
 kat_for_nal_nak.ravno(),
 G_DIR_SEPARATOR_S,
 rek_zag_nn->kod_dpi.ravno(),
 atoi(rek_zag_nn->kod_edrpuo.ravno()),
 iceb_getkoddok(1,mn,gn,1),
 pnd,
 1,
 mn,gn,
 rek_zag_nn->kod_dpi.ravno());
else
 sprintf(imaf_xml,"%.4s%010d%s100%07d%1d%02d%04d%4.4s.XML",
 rek_zag_nn->kod_dpi.ravno(),
 atoi(rek_zag_nn->kod_edrpuo.ravno()),
 iceb_getkoddok(1,mn,gn,1),
 pnd,
 1,
 mn,gn,
 rek_zag_nn->kod_dpi.ravno());
 
if((*ff_xml = fopen(imaf_xml,"w")) == NULL)
 {
  iceb_er_op_fil(imaf_xml,"",errno,wpredok);
  return(1);
 }


return(0);

}
