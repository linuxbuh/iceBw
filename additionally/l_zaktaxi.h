/*$Id: l_zaktaxi.h,v 1.1 2013/08/13 06:26:34 sasa Exp $*/
/*02.08.2013	02.08.2013	Белых А.И.	l_zaktaxi.h
Реквизиты для работы со списком категорий
*/
class l_zaktaxi_rek
 {
  public:
  iceb_u_str kodk; //Код клиента
  iceb_u_str kv;     // код водителя - читается вместе с фамилией
  iceb_u_str fio;    // фамилия заказчика
  iceb_u_str telef;  // телефон клиента
  iceb_u_str kolp;    //количество пассажиров
  iceb_u_str adreso;//  адрес отуда забрать клиента
  iceb_u_str adresk;//  адрес куда отвезти
  iceb_u_str koment;//  коментарий
  iceb_u_str suma;   // сумма
  iceb_u_str kodzav; //Код завершения

  //Реквизиты только в меню ввода записи
  iceb_u_str datvz;  // дата когда нужно выполнить заказ
  iceb_u_str vremvz;  //время когда нужно выполнить заказ

  //Реквизиты только в меню поиска записей
  iceb_u_str datan;
  iceb_u_str datak;
  iceb_u_str kod_operatora;
   
   l_zaktaxi_rek()
    {
     clear_data();
    }

  void clear_data()
   {
    kodk.new_plus("");
    kv.new_plus("");
    fio.new_plus("");
    telef.new_plus("");
    kolp.new_plus("");
    adreso.new_plus("");
    adresk.new_plus("");
    koment.new_plus("");
    suma.new_plus("");
    kodzav.new_plus("");
    datvz.new_plus("");
    vremvz.new_plus("");
    datan.new_plus("");
    datak.new_plus("");
    kod_operatora.new_plus("");
   }
 
 };

gboolean l_zaktaxi_create_list (class l_zaktaxi_data *data);
