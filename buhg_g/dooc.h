/*$Id: dooc.h,v 1.4 2012-09-06 07:10:02 sasa Exp $*/
/*05.09.2012	20.10.2004	Белых А.И.	dooc.h
Данные для дооценки документа
*/

class dooc_data
 {
  public:
  static iceb_u_str datad;
  static iceb_u_str sumadooc;
  static iceb_u_str osnov;
  static iceb_u_str kodmat;
  static class iceb_u_str kodop;
  //Реквизиты документа куда копируем
  short dd,md,gd;
  int    tipz; //1-приход 2-расход
  iceb_u_str nomdok_c;
  int    skl;
  
  dooc_data()
   {
    clear_data();
   }

  void clear_data()
   {
    datad.new_plus("");
    osnov.new_plus("");
    kodmat.new_plus("");
    sumadooc.new_plus("");
    kodop.new_plus("");
   }
 };
