/*$Id: iceb_spks.h,v 1.5 2013/08/13 06:10:13 sasa Exp $*/
/*27.02.2016	18.12.2003	Белых А.И.	iceb_spks.h
Реквизиты для расчёта сальдо по контрагенту
*/

class iceb_spks_data
 {
  public:

  static class iceb_u_str datn;
  static class iceb_u_str datk;
  static class iceb_u_str shets;

  class iceb_u_str shetsrs;  //Счет с развернутым сальдо по которому делаем расчет
  class iceb_u_str shet;
  class iceb_u_str kontr; //Код контрагента для которого делаем расчет
  class iceb_u_str naikontr; //Наименование контрагента для которого делаем расчет
  int mks; /*если равно 1 то встречные проводки не берутся в расчёт*/
  class iceb_u_spisok imaf;
  class iceb_u_spisok naim;
  
  //Конструкор
  iceb_spks_data()
   {
    clear_zero();
   }

  void clear_zero()
   {
    mks=0;
    kontr.new_plus("");
    shet.new_plus("");
    shetsrs.new_plus("");

    datn.new_plus("");
    datk.new_plus("");
    shets.new_plus("");
   }  
 };
