/*$Id: l_zarzvan.h,v 1.3 2011-11-05 09:49:45 sasa Exp $*/
/*21.10.2011	23.06.2006	Белых А.И.	l_zarzvan.h
Реквизиты для работы со списком званий
*/
class l_zarzvan_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str sdzz;  
   l_zarzvan_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    sdzz.new_plus("");
   }
 
 };
