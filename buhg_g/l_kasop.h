/*$Id: l_kasop.h,v 1.8 2013/09/05 07:24:33 sasa Exp $*/
/*04.09.2013	18.01.2006	Белых А.И.	l_kasop.h
Реквизиты для работы со списком операций подсистемы "Учёт услуг"
*/
class kasop_rek
 {
  public:
  class iceb_u_str kod;
  class iceb_u_str naim;    
  class iceb_u_str shet;  //Счёт
  class iceb_u_str shetk; //Счёт корреспондент
  class iceb_u_str kod_cn;
  class iceb_u_str kod_val;
  short prov; //0-проводки не нужно делать 1-нужно

  short      metka_poi;  //0-нет поиска 1-есть поск
    
  kasop_rek()
   {
    clear();
   }
  
  void clear()
   {
    kod.new_plus("");
    naim.new_plus("");
    shet.new_plus("");
    shetk.new_plus("");
    kod_cn.new_plus("");
    kod_val.new_plus("");    
    metka_poi=0;
    prov=0;
   }
 
 };
