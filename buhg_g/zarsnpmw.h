/*$Id: zarsnpmw.h,v 1.1 2012-01-24 11:43:13 sasa Exp $*/
/*20.01.2012	20.01.2012	Белых А.И.	zarsnpmw.h
Класс для получения отчёта
*/

class zarsnpmw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str podr;
  class iceb_u_str tabnom;
  class iceb_u_str kod_nah;
  class iceb_u_str kod_kateg;

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
  class iceb_u_str naim_oth; //наименование отчёта
    
  zarsnpmw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    podr.new_plus("");
    tabnom.new_plus("");
    kod_nah.new_plus("");
    kod_kateg.new_plus("");
   }
 };
