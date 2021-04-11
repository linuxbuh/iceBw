/*$Id:$*/
/*07.08.2015	07.08.2015	Белых А.И.	xukropp.h
Реквизиты поиска
*/
class xukropp_poi
 {
  public:
  class iceb_u_str datan;
  class iceb_u_str datak;
  class iceb_u_str kodkr;
  int metka_fop; /*0-все 1-только физицеские лица пердприниматели*/
  
  xukropp_poi()
   {
    clean();
   }

  void clean()
   {
    metka_fop=0;
    datan.new_plus("");
    datak.new_plus("");
    kodkr.new_plus("");
   }
 };
