/*$Id: l_f1dfvs.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*22.04.2015	22.04.2015	Белых А.И.	l_f1dfvs.h
Реквизиты для поиска
*/
class l_f1dfvs_rek
 {
  public:
   class iceb_u_str inn;
   class iceb_u_str fio;
   int metkap;   
   
   l_f1dfvs_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    inn.new_plus("");
    fio.new_plus("");
    metkap=0;
   }
 
 };
