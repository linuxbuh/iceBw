/*$Id: prov_rek_data.h,v 1.1 2012-01-12 12:35:36 sasa Exp $*/
/*10.01.2012	10.01.2012	Белых А.И.	prov_rek_data.h
Реквизиты проводки
*/

class  prov_rek_data
 {
  public:
  int        val; //0- балансовые проводки -1 небалансовые
  class iceb_u_str datap;
  class iceb_u_str shet;
  class iceb_u_str shetk;
  class iceb_u_str kontr;
  class iceb_u_str kontrk;
  class iceb_u_str debet;  //Вводятся и символьные значения
  class iceb_u_str kredit;
  class iceb_u_str kodop;
  class iceb_u_str nomdok;
  class iceb_u_str koment;
  class iceb_u_str metka;
  class iceb_u_str kekv;
  class iceb_u_str datadok;
  time_t     vremz;
  int podr;
  int tipz;
  int        ktoz;
  
  //Конструктор
  prov_rek_data()
   {
    clear_zero();
   }

  void clear_zero()
   {
    val=0;
    shet.new_plus("");
    shetk.new_plus("");
    datap.new_plus("");
    koment.new_plus("");
    debet.new_plus("");
    kredit.new_plus("");
    kontr.new_plus("");
    kontrk.new_plus("");
    nomdok.new_plus("");
    kodop.new_plus("");
    metka.new_plus("");
    kekv.new_plus("");
    datadok.new_plus("");
    vremz=0;
    ktoz=0;
    podr=tipz=0;
   }
 };