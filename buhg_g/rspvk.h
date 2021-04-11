/*$Id: rspvk.h,v 1.6 2013/12/31 11:49:16 sasa Exp $*/
/*02.07.2019	28.03.2004	Белых А.И.	rspvk.h
Реквизиты расчета сальдо по всем контрагентам
*/

class rspvk_rr
 {
  public:
  static iceb_u_str datan;
  static iceb_u_str datak;
  static iceb_u_str shet;
  static iceb_u_str kodgr;
  static iceb_u_str kontr;
  static short metka_r; /*0-расчёт без определения даты возникновения сальдо 1-с определением даты*/
  static short par; /*0-учесть встречные проводки 1-нет*/
  void clear_data()
   {
    metka_r=0;
    par=1;
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    kodgr.new_plus("");
    kontr.new_plus("");
   }
 };

