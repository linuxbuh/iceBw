/*$Id: l_uslop.h,v 1.5 2012-01-04 11:13:32 sasa Exp $*/
/*30.12.2011	28.07.2005	Белых А.И.	l_uslop.h
Реквизиты для работы со списком операций подсистемы "Учёт услуг"
*/
class uslop_rek
 {
  public:
  class iceb_u_str kod;
  class iceb_u_str naim;    
  class iceb_u_str shet;
  class iceb_u_str kontr;
  short vido; //0-внешняя 1-внутренняя 2-смена стоимости
  short prov; //0-проводки нужно делать 1-не нужно
  short      metka_poi;  //0-нет поиска 1-есть поск
    
  uslop_rek()
   {
    clear();
   }
  
  void clear()
   {
    kod.new_plus("");
    naim.new_plus("");
    shet.new_plus("");
    kontr.new_plus("");
    vido=0;
    prov=0;
    metka_poi=0;
   }
 
 };
