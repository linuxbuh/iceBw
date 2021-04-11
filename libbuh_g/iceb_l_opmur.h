/*$Id: iceb_l_opmur.h,v 1.5 2012-01-04 11:13:35 sasa Exp $*/
/*28.12.2011	09.05.2004	Белых А.И.	iceb_l_opmur.h
Реквизиты справочника операций расхода для подсистемы "Материальный учёт"
*/

class  opmur_rek
 {
  public:
  class iceb_u_str kod;
  class iceb_u_str naim;
  int        vidop; //0-внешняя 1-внутрунняя 2-изменения стоимости
  int        prov; //0-проводки нужно делать 1-нет  
  class iceb_u_str sheta;
  class iceb_u_str kontr;
    
  opmur_rek()
   {
    clear_zero();
   }

  void clear_zero()      
   {
    kod.new_plus("");  
    naim.new_plus("");  
    vidop=prov=0;
    sheta.new_plus("");
    kontr.new_plus("");
   }
 };
 


