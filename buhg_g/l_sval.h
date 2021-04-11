/*$Id: l_sval.h,v 1.1 2013/09/05 07:24:33 sasa Exp $*/
/*05.09.2013	05.09.2013	Белых А.И.	l_sval.h
Реквизиты для работы со списком валют
*/
class l_sval_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str koment;
   
   l_sval_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    koment.new_plus("");
   }
 
 };
