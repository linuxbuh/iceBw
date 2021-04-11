/*$Id: zag_nn_xml.c,v 5.15 2014/07/31 07:09:18 sasa Exp $*/
/*06.12.2018	17.03.2016	Белых А.И.	iceb_nn10_zag_xml.c
Заголовок налоговой накладной в формате xml.
*/
#include "iceb_libbuh.h"

void iceb_nn10_zag_xml(int tipz,
const char *nomdok,
short dnp,short mnp,short gnp, //дата выписки налоговой накладной
short dd,short md,short gd,   //дата документа
const char *kontr00,/*код контрагента 00*/
const char *kontr,  
const char *vid_dok,
FILE *ff_xml,
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
char strsql[512];
class iceb_u_str naim_kontr00("");
class iceb_u_str inn_kontr00("");
class iceb_u_str kod_filiala00("");
class iceb_u_str kod_edrpu00("");
class iceb_u_str naim_kontr("");
class iceb_u_str inn_kontr("");
class iceb_u_str kod_filiala("");
class iceb_u_str kod_edrpu("");


//читаем реквизиты контрагента
sprintf(strsql,"select naikon,innn,pnaim,kod from Kontragent where kodkon='%s'",kontr00);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[2][0] == '\0')
    naim_kontr00.new_plus(row[0]);
  else
    naim_kontr00.new_plus(row[2]);

  inn_kontr00.new_plus(row[1]);
  kod_edrpu00.new_plus(row[3]);
 }

sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Код филиала%%'",kontr00);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) >= 1)
  iceb_u_polen(row[0],&kod_filiala00,2,'|');

//читаем реквизиты контрагента
sprintf(strsql,"select naikon,innn,pnaim,kod from Kontragent where kodkon='%s'",kontr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 {
  if(row[2][0] == '\0')
    naim_kontr.new_plus(row[0]);
  else
    naim_kontr.new_plus(row[2]);

  inn_kontr.new_plus(row[1]);
  kod_edrpu.new_plus(row[3]);  
 }

sprintf(strsql,"select zapis from Kontragent1 where kodkon='%s' and zapis like 'Код филиала%%'",kontr);
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) >= 1)
  iceb_u_polen(row[0],&kod_filiala,2,'|');

fprintf(ff_xml," <DECLARBODY>\n");

class iceb_u_str tip_prih("");
if(iceb_u_polen(vid_dok,&tip_prih,2,',') == 0)
 {
  if(tip_prih.getdlinna() > 1)
   {
    fprintf(ff_xml,"  <HORIG1>1</HORIG1>\n");
    fprintf(ff_xml,"  <HTYPR>%s</HTYPR>\n",tip_prih.ravno());
   }
 }
  
if(dnp != 0)
    fprintf(ff_xml,"  <HFILL>%02d%02d%04d</HFILL>\n",dnp,mnp,gnp);
else
    fprintf(ff_xml,"  <HFILL>%02d%02d%04d</HFILL>\n",dd,md,gd);

fprintf(ff_xml,"  <HNUM>%s</HNUM>\n",nomdok);

if(tipz == 1)
 {

  fprintf(ff_xml,"  <HNAMEBUY>%s</HNAMEBUY>\n",iceb_u_filtr_xml(naim_kontr.ravno()));

  if(iceb_u_SRAV(inn_kontr00.ravno(),"100000000000",0) == 0 || iceb_u_SRAV(vid_dok,"ПНЕ,02",0) == 0)
   fprintf(ff_xml,"  <HNAMESEL>Неплатник</HNAMESEL>\n");
  else
   fprintf(ff_xml,"  <HNAMESEL>%s</HNAMESEL>\n",iceb_u_filtr_xml(naim_kontr00.ravno()));

  if(inn_kontr.ravno()[0] != '\0')
    fprintf(ff_xml,"  <HKBUY>%s</HKBUY>\n",inn_kontr.ravno());
  if(inn_kontr00.getdlinna() > 1)
    fprintf(ff_xml,"  <HKSEL>%s</HKSEL>\n",inn_kontr00.ravno());

  if(kod_filiala.getdlinna() > 1)
    fprintf(ff_xml,"  <HNUM2>%s</HNUM2>\n",kod_filiala.ravno());
 
  if(kod_filiala00.getdlinna() > 1)
   fprintf(ff_xml,"  <HFBUY>%s</HFBUY>\n",kod_filiala00.ravno());
    
  if(kod_edrpu.getdlinna() > 1 &&
  iceb_u_SRAV(inn_kontr.ravno(),"100000000000",0) != 0 && 
  iceb_u_SRAV(inn_kontr.ravno(),"300000000000",0) != 0  && 
  iceb_u_SRAV(inn_kontr.ravno(),"400000000000",0) != 0  &&
  iceb_u_SRAV(inn_kontr.ravno(),"600000000000",0) != 0 )
     fprintf(ff_xml,"  <HTINSEL>%s</HTINSEL>\n",kod_edrpu.ravno());
    
  if(kod_edrpu00.getdlinna() > 1 &&
  iceb_u_SRAV(inn_kontr00.ravno(),"100000000000",0) != 0 && 
  iceb_u_SRAV(inn_kontr00.ravno(),"300000000000",0) != 0  && 
  iceb_u_SRAV(inn_kontr00.ravno(),"400000000000",0) != 0  &&
  iceb_u_SRAV(inn_kontr00.ravno(),"600000000000",0) != 0 )
     fprintf(ff_xml,"  <HTINBUY>%s</HTINBUY>\n",kod_edrpu00.ravno());

 }

if(tipz == 2)
 {

  fprintf(ff_xml,"  <HNAMESEL>%s</HNAMESEL>\n",iceb_u_filtr_xml(naim_kontr00.ravno()));


  if(iceb_u_SRAV(inn_kontr.ravno(),"100000000000",0) == 0 || iceb_u_SRAV(vid_dok,"ПНЕ,02",0) == 0)
   fprintf(ff_xml,"  <HNAMEBUY>Неплатник</HNAMEBUY>\n");
  else
   fprintf(ff_xml,"  <HNAMEBUY>%s</HNAMEBUY>\n",iceb_u_filtr_xml(naim_kontr.ravno()));

  if(inn_kontr00.getdlinna() > 1)
   fprintf(ff_xml,"  <HKSEL>%s</HKSEL>\n",inn_kontr00.ravno());
  if(inn_kontr.ravno()[0] != '\0')
   fprintf(ff_xml,"  <HKBUY>%s</HKBUY>\n",inn_kontr.ravno());


  if(kod_filiala00.getdlinna() > 1)
     fprintf(ff_xml,"  <HNUM2>%s</HNUM2>\n",kod_filiala00.ravno());

  if(kod_filiala.getdlinna() > 1)
     fprintf(ff_xml,"  <HFBUY>%s</HFBUY>\n",kod_filiala.ravno());


  if(kod_edrpu00.getdlinna() > 1 &&
  iceb_u_SRAV(inn_kontr00.ravno(),"100000000000",0) != 0 && 
  iceb_u_SRAV(inn_kontr00.ravno(),"300000000000",0) != 0  && 
  iceb_u_SRAV(inn_kontr00.ravno(),"400000000000",0) != 0  &&
  iceb_u_SRAV(inn_kontr00.ravno(),"600000000000",0) != 0 )
     fprintf(ff_xml,"  <HTINSEL>%s</HTINSEL>\n",kod_edrpu00.ravno());
    
  if(kod_edrpu.getdlinna() > 1 &&
  iceb_u_SRAV(inn_kontr.ravno(),"100000000000",0) != 0 && 
  iceb_u_SRAV(inn_kontr.ravno(),"300000000000",0) != 0  && 
  iceb_u_SRAV(inn_kontr.ravno(),"400000000000",0) != 0  &&
  iceb_u_SRAV(inn_kontr.ravno(),"600000000000",0) != 0 )
     fprintf(ff_xml,"  <HTINBUY>%s</HTINBUY>\n",kod_edrpu.ravno());

    

 }


}

