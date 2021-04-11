/*$Id: l_uslpodr.h,v 1.4 2011-11-05 09:49:45 sasa Exp $*/
/*07.10.2011	17.07.2005	Белых А.И.	uslpodr.h
Реквизиты для работы со списком групп услуг.
*/
class uslpodr_rek
 {
  public:
  iceb_u_str kod;
  iceb_u_str naim;    
  class iceb_u_str np;
  class iceb_u_str fo;
  class iceb_u_str dol;
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  uslpodr_rek()
   {
    clear();
   }
  
  void clear()
   {
    kod.new_plus("");
    naim.new_plus("");
    np.new_plus("");
    fo.new_plus("");
    dol.new_plus("");
    metka_poi=0;
   }
 
 };
