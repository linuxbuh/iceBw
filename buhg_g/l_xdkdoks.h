/*$Id: l_xdkdoks.h,v 1.6 2013/12/31 11:49:15 sasa Exp $*/
/*16.01.2017	06.04.2006	Белых А.И.	l_kasdoks.h
Реквизиты поиска записей в списке платёжных поручений
*/

class xdkdoks_rek
 {
  public:
  iceb_u_str nomdok;
  iceb_u_str datan;    
  iceb_u_str datak;    
  iceb_u_str kodop;    
  iceb_u_str suma;
  
  short      metka_poi;  //0-нет поиска 1-есть поск
  short      metka_pros; //0-все 1-не подтверждённые 2-без проводок 3-помеченные для экспорта
//  short metka_pp; /*0-все 1-только помеченые для передачи в банк*/

  xdkdoks_rek()
   {
//    metka_pp=0;
    clear();
   }
  
  void clear()
   {
    nomdok.new_plus("");
    datan.new_plus("");
    datak.plus_tek_dat();
    kodop.new_plus("");
    suma.new_plus("");
    metka_poi=0;
    metka_pros=0;
   }
 
 };
