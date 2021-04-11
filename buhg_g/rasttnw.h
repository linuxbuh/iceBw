/*$Id: rasttnw.h,v 1.1 2013/10/04 07:27:16 sasa Exp $*/
/*03.10.2013	30.09.2013	Белых А.И.
*/
class rasttn_rek
 {
  public:
   class iceb_u_str marka_avt;
   class iceb_u_str gos_nomer;
   class iceb_u_str nom_put_list;
   class iceb_u_str fio_vod;
   class iceb_u_str avtopredp;
   class iceb_u_str punkt_zagr;
   class iceb_u_str punkt_razg;
   class iceb_u_str zakazchik;
   class iceb_u_str gos_nomer_pric;   
   short dd,md,gd;
   class iceb_u_str nomdok;
   int sklad;
   
   rasttn_rek()
    {
     clear_rek();
     dd=md=gd=0;
     nomdok.plus("");
     sklad=0;
    }
   void clear_rek()
    {
     marka_avt.new_plus("");
     gos_nomer.new_plus("");
     gos_nomer_pric.new_plus("");
     nom_put_list.new_plus("");
     fio_vod.new_plus("");
     avtopredp.new_plus("");
     punkt_zagr.new_plus("");
     punkt_razg.new_plus("");
     zakazchik.new_plus("");
    }
 };
