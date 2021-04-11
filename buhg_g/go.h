/*$Id: go.h,v 1.10 2013/09/26 09:46:48 sasa Exp $*/
/*05.09.2013	18.03.2004	Белых А.И.	go.h
Реквизиты расчета главной книги
*/

class go_rr
 {
  public:
  class iceb_u_str datan;  // дата начала
  class iceb_u_str datak;  //дата конца
  class iceb_u_str shet;   //счет
  class iceb_u_str kodgr; //код группы контрагента
  class iceb_u_str naimshet;
  short metka_r;   //0-по субсчетам 1-по счетам
  short vds;       //0-счет 1-субсчет
  short stat;      //0-балансовый 1-не балансовый
  short saldo;     //0-свернутое 3-развернутое

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
  int kolih_znak;  /*Максимальное количество знаков в номере счёта для расчёта по счетам со свёрнутым сальдо*/

  /*Для счетов с развёрнутым сальдо*/
  int metka_oth; /*0-журнал ордер 1-распечатка проводок*/  
  int metkasort; /*тип сортировки контрагентов 0-без сортировки
                                                1-сортировка по коду контрагента в символическом виде
                                                2-сортировка по коду контрагента в цифровом виде
                                                3-сортировка по наименованию контрагента
                                                4-сортировка по группе контрагента
                                                */
  class iceb_u_str kod_val; /*Код валюты*/

  go_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    shet.new_plus("");
    kodgr.new_plus("");
    metka_r=0;
    vds=0;
    stat=0;
    saldo=0;
    kolih_znak=0;

    metka_oth=0;
    metkasort=0;
    kod_val.plus("");
   }
 };

