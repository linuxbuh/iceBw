/*$Id: l_upldok3.h,v 1.1 2012-05-20 08:54:03 sasa Exp $*/
/*23.04.2012	23.04.2012	Белых А.И.	l_upldok3.h
Реквизиты для работы 
*/
class l_upldok3_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   
   l_upldok3_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
   }
 
 };
