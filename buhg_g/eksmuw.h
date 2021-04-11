/*$Id: eksmuw.h,v 1.1 2013/08/13 06:15:42 sasa Exp $*/
/*18.06.2013	18.06.2013	Белых А.И.	eksmuw.h
Класс для получения отчёта
*/

class eksmuw_rr
 {
  public:
  class iceb_u_str dataost;
  class iceb_u_str kodskl;
  class  iceb_u_str shetuh;

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
    
  eksmuw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    dataost.new_plus("");
    kodskl.new_plus("");
    shetuh.new_plus("");
   }
 };
