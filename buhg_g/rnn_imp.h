/*$Id: rnn_imp.h,v 1.3 2013-01-02 10:39:35 sasa Exp $*/
/*26.12.2012	06.05.2008	Белых А.И.	rnn_imp.h
Реквизиты иморта налоговых накладных
*/

class rnn_imp_data
 {
  public:
   class iceb_u_str datan;
   class iceb_u_str datak;
   class iceb_u_str inn; /*Индивидуальный налоговый номер*/
   class iceb_u_str nnn; /*Номер налоговой наклданой*/   
   rnn_imp_data()
    {
     clear();
    }   

   void clear()
    {
     datan.new_plus("");
     datak.new_plus("");
     inn.new_plus("");
     nnn.new_plus("");
    }

 };
