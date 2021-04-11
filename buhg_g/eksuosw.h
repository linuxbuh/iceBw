/*$Id: eksuosw.h,v 1.1 2013/08/13 06:15:42 sasa Exp $*/
/*18.06.2013	18.06.2013	Белых А.И.	eksuosw.h
Класс для получения отчёта
*/

class eksuosw_rr
 {
  public:
  class iceb_u_str dataost;
  class iceb_u_str kodpdr;
  class iceb_u_str kodmot;
  class iceb_u_str shetuh;

  class iceb_u_spisok imaf;
  class iceb_u_spisok naimf;
    
  eksuosw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    dataost.new_plus("");
    kodpdr.new_plus("");
    kodmot.new_plus("");
    shetuh.new_plus("");
   }
 };
