/*$Id: rek_kartmat.h,v 1.6 2011-01-13 13:49:52 sasa Exp $*/
/*04.11.2014	07.06.2004	Белых А.И.	rek_kartmat.h
Реквизиты карточки материалла
*/
class rek_kartmat
 {
  public:
  
  //Реквизиты которые не меняются
  class iceb_u_str skl;
  class iceb_u_str n_kart;
  class iceb_u_str kodm;
  
  //Реквизиты которые могут меняться
  class iceb_u_str mnds;  //0-цена без НДС 1-цена с НДС
  class iceb_u_str ei;
  class iceb_u_str shet;
  class iceb_u_str cena;
  class iceb_u_str cenapr;
  class iceb_u_str krat;
  class iceb_u_str nds;
  class iceb_u_str fas;
  class iceb_u_str kodtar;
  class iceb_u_str datv;
  class iceb_u_str innom;
  class iceb_u_str rnd;
  class iceb_u_str nomzak;
  class iceb_u_str data_kon_is; //Дата конечного использования
  
  //Не относится к реквизитам карточки
  class iceb_u_str naimat;
  class iceb_u_str naimskl;
  class iceb_u_str kodgr;
  class iceb_u_str naimgr;
  
 };
