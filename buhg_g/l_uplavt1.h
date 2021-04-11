/*$Id: l_uplavt1.h,v 1.1 2012-05-20 08:54:03 sasa Exp $*/
/*20.04.2012	20.04.2012	Белых А.И.	l_uplavt1.h
Реквизиты для работы 
*/
class l_uplavt1_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_uplavt1_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
