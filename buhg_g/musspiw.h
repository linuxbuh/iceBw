/*$Id:$*/
/*26.02.2017	26.02.2017	Белых А.И.	musspiw.h
Класс для получения отчёта 
*/

class musspiw_rr
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str sklad;
  class iceb_u_str grup_mat;
  class iceb_u_str kodmat;
  class iceb_u_str shetz;

    
  musspiw_rr()
   {
    clear_data();
   }

  void clear_data()
   {
    datan.new_plus("");
    datak.new_plus("");
    sklad.new_plus("");
    grup_mat.new_plus("");
    shetz.new_plus("");
    kodmat.new_plus("");
   }
 };
