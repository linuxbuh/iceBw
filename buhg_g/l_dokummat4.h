/*$Id:$*/
/*25.06.2017	25.02.2017	Белых А.И.	l_dokummat4.h

*/
class l_dokummat4_rek
 {
  public:

   class iceb_u_str naim_kd;
   class iceb_u_str naim_km;
   class iceb_u_str kod_mat;
   class iceb_u_str kod_det;
   short metka_poi;
   
   l_dokummat4_rek()
    {
     clear_data();
    }
  void clear_data()
   {
    naim_kd.plus("");
    naim_km.plus("");
    kod_mat.plus("");
    kod_det.plus("");
    metka_poi=0; 
   }
 
 };
