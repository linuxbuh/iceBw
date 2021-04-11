/*$Id:$*/
/*03.10.2015	03.10.2015	Белых А.И.	zarrsovw.h
Структура хранения данных по отработанному времини за период
*/


class iceb_zarrsov /*свод по отработанному времени*/
 {
  public:
   class iceb_u_int kod; /*коды табеля*/
   class iceb_u_spisok kod_mes_god; /*список код|месяц-год*/
   class iceb_u_int kolrd;   /*количество отработанных рабочих дней*/
   class iceb_u_int kolkd;   /*количество отработанных календарных дней*/
   class iceb_u_double kolhs;   /*количество отработанных часов*/

 };
