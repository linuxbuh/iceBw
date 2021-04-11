/*$Id: l_uplavt0.h,v 1.1 2012-05-20 08:54:02 sasa Exp $*/
/*02.05.2012	02.05.2012	Белых А.И.	l_uplavt0.h
Реквизиты для поиска записей
*/
class l_uplavt0_rek
 {
  public:
   class iceb_u_str kod; /*код автомобиля*/
   class iceb_u_str naim; /*наименование автомобиля*/
   class iceb_u_str gos_nomer; /*Государственный номер*/
   class iceb_u_str kod_vod; /*код водителя*/
            
   l_uplavt0_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    gos_nomer.new_plus("");
    kod_vod.new_plus("");
   }
 
 };
