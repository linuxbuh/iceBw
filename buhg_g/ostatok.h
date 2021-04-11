/*$Id: ostatok.h,v 1.3 2011-01-13 13:49:52 sasa Exp $*/
/*19.10.2018	20.05.2004	Белых А.И.	ostatok.h
Структура остатков
*/

class ostatok
 {
  public:
  
  double ostg[4]; /*количественное движение с начала года до конца периода */
  double ostm[4];  /*количественное движение за период*/
  double ostgc[4]; /*денежное движение с начала года до конца периода*/
  double ostmc[4]; /*денежное движение за период*/
  double  kgg[4],kgmm[4];   /*Остаток в киллограммах*/
  //конструктор
  ostatok()
   {
    clear();
   }

  void clear(void)
   {
    memset(ostg,'\0',sizeof(ostg));
    memset(ostm,'\0',sizeof(ostm));
    memset(ostmc,'\0',sizeof(ostmc));
    memset(ostgc,'\0',sizeof(ostgc));
    memset(kgg,'\0',sizeof(kgg));
    memset(kgmm,'\0',sizeof(kgmm));
   }   
 };
