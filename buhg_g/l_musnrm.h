/*$Id:$*/
/*10.03.2017	10.03.2017	Белых А.И.	l_musnrm.h
Реквизиты для работы
*/
class l_musnrm_rek
 {
  public:
   class iceb_u_str kodd;
   class iceb_u_str naimd;
   class iceb_u_str kodm;
   class iceb_u_str naimm;
   int metka_poi;
      
   l_musnrm_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    metka_poi=0;
    kodd.new_plus("");
    naimd.new_plus("");
    kodm.new_plus("");
    naimm.new_plus("");
   }
 
 };
