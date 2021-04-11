/*$Id: l_kasdoks.h,v 1.5 2011-01-13 13:49:50 sasa Exp $*/
/*16.01.2017	24.01.2006	Белых А.И.	l_kasdoks.h
Реквизиты поиска записей в списке кассовых ордеров
*/

class kasdoks_rek
 {
  public:
  class iceb_u_str nomdok;
  class iceb_u_str datan;    
  class iceb_u_str datak;    
  class iceb_u_str kodop;    
  class iceb_u_str kassa;    
  class iceb_u_str shet;    
  class iceb_u_str pr_ras;    
  class iceb_u_str nomer_bl;
  class iceb_u_str fio;
    
  short      metka_poi;  //0-нет поиска 1-есть поск
  short      metka_pros; //0-все 1-не подтверждённые 2-без проводок    

  kasdoks_rek()
   {
    clear();
   }
  
  void clear()
   {
    fio.new_plus("");
    nomdok.new_plus("");
    datan.new_plus("");
    datak.plus_tek_dat();
    kodop.new_plus("");
    kassa.new_plus("");
    shet.new_plus("");
    pr_ras.new_plus("");
    nomer_bl.new_plus("");
    metka_poi=0;
    metka_pros=0;
   }
 
 };
