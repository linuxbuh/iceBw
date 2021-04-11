/*$Id: iceb_l_opmup.h,v 1.4 2011-01-13 13:49:59 sasa Exp $*/
/*30.11.2016	05.05.2004	Белых А.И.	iceb_l_opmup.h
реквизиты кодов операций для подсистемы "Материальный учёт"
*/


class  opmup_rek
 {
  public:
  class iceb_u_str kod;
  class iceb_u_str naim;
  int vidop; //0-внешняя 1-внутрунняя 2-изменение реквизитов учёта
  int prov; //0-проводки нужно делать 1-нет  

  opmup_rek()
   {
    clear_zero();
   }
  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
    vidop=prov=0;
   }
 };
 


