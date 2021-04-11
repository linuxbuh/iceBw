/*$Id: buh_rnr_shw.h,v 1.1 2013/05/17 15:01:15 sasa Exp $*/
/*20.04.2013	20.04.2013	Белых А.И.	buh_rnr_shw.h
Расчёт шахматки по счёту
*/

class buh_rnr_shw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str shet;

  iceb_u_spisok imaf;
  iceb_u_spisok naimf;
    
  buh_rnr_shw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
   }
 };
