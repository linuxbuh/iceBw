/*$Id: iceb_rnfxml.h,v 1.3 2012-08-29 07:51:20 sasa Exp $*/
/*03.04.2016	12.10.2006	Белых А.И.	iceb_rnfxml.h
Реквизиты шапки документов в xml формате
*/

class iceb_rnfxml_data
 {
  public:
  short dt,mt,gt;
  class iceb_u_str kod_edrpuo;
  class iceb_u_str kod_oblasti;
  class iceb_u_str kod_admin_r;
  class iceb_u_str naim_kontr;
  class iceb_u_str adres;
  class iceb_u_str innn;
  class iceb_u_str nspnds;
  class iceb_u_str kod_dpi;
  class iceb_u_str kod_tabl;
  class iceb_u_str telefon;
    
  int kod_gr_voz_tar; //Код группы возвратная тара  
  iceb_rnfxml_data()
   {
    kod_edrpuo.plus("");
    kod_oblasti.plus("");
    kod_admin_r.plus("");
    naim_kontr.plus("");
    kod_tabl.plus("");
    adres.plus("");
    innn.plus("");
    nspnds.plus("");
    kod_dpi.plus("");
    telefon.plus("");
    iceb_u_poltekdat(&dt,&mt,&gt);   
    kod_gr_voz_tar=0;
   }
 };
