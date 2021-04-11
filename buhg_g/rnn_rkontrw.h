/*$Id: rnn_rkontrw.h,v 1.1 2012-01-24 11:43:12 sasa Exp $*/
/*20.01.2012	20.01.2012	Белых А.И.	rnn_rkontrw.h
Класс для получения отчёта
*/

class rnn_rkontrw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str inn;
  int metka_pr; //0-всё 1-только прихов 2-только расход

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
  class iceb_u_str naim_oth; //наименование отчёта
    
  rnn_rkontrw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    inn.new_plus("");
    metka_pr=0;
   }
 };
