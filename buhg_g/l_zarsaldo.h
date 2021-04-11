/*$Id: l_zarsaldo.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*18.09.2014	18.09.2014	Белых А.И.	l_zarsaldo.h
Реквизиты поиска в справочнике сальдо
*/
class l_zarsaldo_rek
 {
  public:
   class iceb_u_str god;
   class iceb_u_str tabnom;
   class iceb_u_str fio;
   int metka_poi;   

   l_zarsaldo_rek()
    {
     clear_data();
     metka_poi=0;
    }

  void clear_data()
   {
    god.new_plus("");
    tabnom.new_plus("");
    fio.new_plus("");
   }
 
 };
