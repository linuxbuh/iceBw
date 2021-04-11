/*$Id: xdkbanksw.h,v 1.1 2013/08/13 06:23:37 sasa Exp $*/
/*18.07.2013	18.07.2013	Белых А.И.	xdkbanksw.h

*/

class xdkbanksw_rr
 {
  public:
  class iceb_u_str shet;
  class iceb_u_str datas;
  class iceb_u_str kodkontr;
  class iceb_u_str kodgrkontr;
  class iceb_u_str shetk;
  class iceb_u_str datap;

  int metka; /*0-кредитовое 1-дебетовое*/    

  /*реквизиты платёжки*/
  short dd,md,gd;
  class iceb_u_str nomdok;
  class iceb_u_str kodop;
  class iceb_u_str kodor;  
  class iceb_u_str tabl;
  xdkbanksw_rr()
   {
    clear_data();
    dd=md=gd=0;
    nomdok.plus("");
    kodop.plus("");
    kodor.plus("");
    tabl.plus("");
   }

  void clear_data()
   {
    shet.new_plus("");
    datas.new_plus("");
    kodkontr.new_plus("");
    kodgrkontr.new_plus("");
    shetk.new_plus("");
    datap.new_plus("");
    metka=0;
   }
 };
