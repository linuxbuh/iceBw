/*$Id: specrasw.h,v 1.3 2011-01-13 13:49:53 sasa Exp $*/
/*12.03.2017	17.05.2005	Белых А.И.	specrasw.h
Реквизиты для расчёта разузлования изделия
*/

class specrasw_rr
 {
  public:

  class iceb_u_str kolih;

  class iceb_u_str kod_izdel;
  class iceb_u_str naim_izdel;
  specrasw_rr()
   {
    clear_data();
   }
  void clear_data()
   {
    kolih.new_plus("");
   }
 };
