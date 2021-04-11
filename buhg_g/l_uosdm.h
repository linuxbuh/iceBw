/*$Id: l_uosdm.h,v 1.3 2013/09/05 07:24:33 sasa Exp $*/
/*01.03.2009	01.03.2009	Белых А.И.	l_uosdm.h
Реквизиты для работы со справочником драгоценных металлов
*/
class l_uosdm_rek
 {
  public:
   class iceb_u_str kod;
   class iceb_u_str naim;
   class iceb_u_str ei;
   
   l_uosdm_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    kod.new_plus("");
    naim.new_plus("");
    ei.new_plus("");
   }
 
 };
