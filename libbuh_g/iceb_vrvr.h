/*$Id:$*/
/*24.11.2016	22.11.2016	Белых А.И.	iceb_vrvr.h
вставка реквизитов в распечатку
перечень реквизитов и строк и позиций вставки
*/
class iceb_vrvr
 {
  public:
   class iceb_u_int ns; /*номера строк в которые надо вставить*/
   class iceb_u_int begin; /*позиция начала встваки*/
   class iceb_u_int end; /*позиция конца встваки*/
   class iceb_u_int hag;  /*шаг вставки*/
   class iceb_u_spisok str;         /*то что нужно вставить*/

   void free_class()
    {
     ns.free_class();
     begin.free_class();
     end.free_class();
     hag.free_class();
     str.free_class();
    }
 };
 