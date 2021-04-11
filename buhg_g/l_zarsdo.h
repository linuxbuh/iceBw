/*$Id: l_zarsdo.h,v 1.2 2011-01-13 13:49:51 sasa Exp $*/
/*01.05.2020	30.05.2016	Белых А.И.	l_zarsdo.h
Реквизиты для работы со списком должностных окладов
*/
class l_zarsdo_rek
 {
  public:
   short metka_poi; /*0-поиск не делать 1-делать*/
   class iceb_u_str tabnom;
   class iceb_u_str dataz;
   class iceb_u_str kod_nah;
   class iceb_u_str fio;
   class iceb_u_str koment;
   class iceb_u_str shet;
   class iceb_u_str podr;
   int metka_sr; /*метка способа расчёта*/
   int metka_sort; /*метка сортировки 0-по табельному номеру 1-по дате начала действия оклада 2-только последние изменения по табельному номеру*/
   int metka_uv; /*мета уволенных 0-все 1-только работающие 2-только уволенные*/
   l_zarsdo_rek()
    {
     metka_poi=0;
     clear_data();
    }
  void clear_data()
   {
    metka_sr=0;
    tabnom.new_plus("");
    dataz.new_plus("");
    kod_nah.new_plus("");
    fio.new_plus("");
    koment.new_plus("");
    shet.new_plus("");
    podr.new_plus("");
    metka_sort=0;
    metka_uv=0;
   }
 
 };

class l_zarsdo_menu_mr
 {
  public:
   class iceb_u_spisok strsr;

   l_zarsdo_menu_mr()
    {
     strsr.plus(gettext("Оплата за месяц, расчёт по дням"));//0
     strsr.plus(gettext("Оплата за месяц, расчёт по часам"));//1
     strsr.plus(gettext("Оплата за месяц, без расчёта"));//2
     strsr.plus(gettext("Оплата за час"));//3
     strsr.plus(gettext("Оплата за день"));//4
     strsr.plus(gettext("Оплата по тарифу, расчёт по дням"));//5
     strsr.plus(gettext("Оплата по тарифу, расчёт по часам"));//6
     
    }
 };
