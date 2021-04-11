/*$Id:$*/
/*23.10.2019	23.10.2019	Белых А.И.	zvb_menu_lst.h
данные которые вводятся в меню
*/


class zvb_menu_lst_r
 {
  public:
  short d_pp,m_pp,g_pp; //Дата платёжки
  short mz,gz;
  class iceb_u_str nomer_pp;
  class iceb_u_str vid_nah;

  zvb_menu_lst_r()
   {
    d_pp=m_pp=g_pp=0;
    mz=gz=0;
    nomer_pp.plus("");
    vid_nah.plus("Заработная плата и авансы");
   }
};
 
