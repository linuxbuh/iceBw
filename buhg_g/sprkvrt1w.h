/*$Id: sprkvrt1w.h,v 1.2 2012-10-30 09:24:46 sasa Exp $*/
/*06.08.2015	31.04.2005	Белых А.И.	sprkvrt1w.h
Настройки для расчёта формы 1ДФ
*/

class sprkvrt1_nast
 {
  public:

  class iceb_u_spisok sheta;               //Настройки пар счетов для поиска нужных проводок
  
  class iceb_u_int kodi_pd; //коды признаков доходов
  class iceb_u_spisok kodi_nah;       // коды начислений к признакам доходов
  class iceb_u_int metka_podoh; /*0-берётся подоходный для кода признака дохода 1- не берётся*/
  
  class iceb_u_str gosstrah; //коды фондов на 133 признак

  short		TYP; /*Метка 0-юридическая особа 1-физическая особа*/
  short    *kodnvf8dr;  /*Коды не входящие в форму 8ДР*/
  short    *kodud;  /*Коды удержаний не входящие в форму 8ДР*/
  
  class iceb_u_int kodi_pd_prov; //коды признаков доходов для которых данные берём из главной книги
  
  class iceb_u_int kodkr; /*коды командировочных расходов*/
  class iceb_u_spisok kodd_for_kodkr; /*Коды доходов для кодов командировочных расходов*/

  sprkvrt1_nast() //Конструктор
   {
    gosstrah.plus("");
    TYP=0;    
    kodnvf8dr=NULL;
    kodud=NULL;
   }

  ~sprkvrt1_nast() //Деструкток
   {
    if(kodnvf8dr != NULL)
      delete(kodnvf8dr);
    if(kodud != NULL)
      delete(kodud);
   }

 };

