/*$Id: iceb_l_grupmat.h,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*28.02.2016	30.12.2003	Белых А.И.	iceb_l_grupmat.h
Реквизиты для справочника групп материалов
*/

class  grupmat_rek
 {
  public:
  class iceb_u_str kod;
  class iceb_u_str naim;

  grupmat_rek()
   {
    clear_zero();
   }

  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
   }
 };
 

