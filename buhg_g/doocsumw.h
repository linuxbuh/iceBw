/*$Id: doocsumw.h,v 1.4 2012-09-06 07:10:02 sasa Exp $*/
/*05.09.2012	06.05.2005	Белых А.И.	doocsumw.h
Реквизиты для получени отчёта по дооценкам
*/
class doocsumw_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str sklad;
  static class iceb_u_str kodop;
  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
  
  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    sklad.new_plus("");
    kodop.new_plus("");
   }
 };
