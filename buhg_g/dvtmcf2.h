/*$Id: dvtmcf2.h,v 1.3 2011-01-13 13:49:50 sasa Exp $*/
/*13.10.2014	28.11.2004	Белых А.И.	dvtmcf2.h
Реквизиты для получения отчета - движения товатно-материальных ценностей форма 2
*/

class dvtmcf2_rr
 {
  public:
  
  static class iceb_u_str datan;
  static class iceb_u_str datak;
  static class iceb_u_str shet;
  static class iceb_u_str sklad;
  static class iceb_u_str grupa;
  static class iceb_u_str kodmat;
  static class iceb_u_str nds;
  static class iceb_u_str kontr;
  static class iceb_u_str kodop;
  static int pr; /*0-всё 1-приходы 2-расходы*/
  static class iceb_u_str nomdok;
  
  static int metka_sort;  
  static int metka_cena; /*0-в учётных ценах 1-в ценах в документе*/
    
  class spis_oth oth;

  dvtmcf2_rr()
   {
    clear_data();
   }  

  void clear_data()
   {
    metka_sort=0;
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    sklad.new_plus("");
    grupa.new_plus("");
    kodmat.new_plus("");
    kontr.new_plus("");
    kodop.new_plus("");
    pr=0;
    nomdok.new_plus("");
    nds.new_plus("");
    metka_cena=0;
   }
  
 };
