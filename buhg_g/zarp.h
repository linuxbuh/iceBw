/*$Id: zarp.h,v 1.5 2013/08/13 06:10:03 sasa Exp $*/
/*23.05.2013	01.09.2006	Белых А.И.	zarp.h
Уникальные реквизиты записи начисления/удержания
*/

class ZARP
 {
  public:
  int tabnom; //Табельный номер
  short prn; //1-начисление 2-удержание
  short knu; //Код начисления/удержание
  short dz,mz,gz; //Дата записи
  short godn; //Год в счет котрого запись
  short mesn; //Месяц в счет которого запись
  short nomz; //Номер записи
  int   podr; //Подразделение
  char  shet[64]; //Счет
  ZARP()
   {
    tabnom=prn=knu=0;
    dz=mz=gz=godn=mesn=nomz=podr=0;
    memset(shet,'\0',sizeof(shet));
   }
 };
