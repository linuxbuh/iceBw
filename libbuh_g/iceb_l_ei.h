/*$Id: iceb_l_ei.h,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*27.02.2016	05.05.2004	Белых А.И.	iceb_l_ei.h
*/



class iceb_l_ei_rek
 {
  public:
  class iceb_u_str kod;
  class iceb_u_str naim;
  class iceb_u_str kodei;
  
  iceb_l_ei_rek()
   {
    clear_zero();
   }

  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
    kodei.new_plus("");
   }
 };

