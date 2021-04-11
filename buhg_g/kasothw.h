/*$Id: kasothw.h,v 1.7 2013/08/13 06:09:36 sasa Exp $*/
/*17.07.2013	10.03.2006	Белых А.И.	kasothw.h
Класс для получения отчёта в подсистеме "Учёт кассовых ордеров"
*/

class kasothw_rr
 {
  public:
  class iceb_u_str datan;
  class  iceb_u_str datak;
  class  iceb_u_str kodop;
  class  iceb_u_str kassa;
  class  iceb_u_str shet;
  class iceb_u_str kod_kontr;
  int  metka_pr;   //0-всё 1-только приходы 2-только расходы

  class iceb_u_str naim_oth; //наименование отчёта
    
  kasothw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    kodop.new_plus("");
    kassa.new_plus("");
    shet.new_plus("");
    kod_kontr.new_plus("");
    metka_pr=0;
   }
 };
