/*$Id: vibshet.c,v 1.13 2013/09/26 09:46:58 sasa Exp $*/
/*21.01.2004	05.01.2004	Белых А.И.	vibshet.c
Выбор счета для выполнения проводок
*/
#include  "buhg_g.h"

void m_vprov_bs(const char*);
void m_vprov_bs_gr(const char*);
void m_vprov_nbs(const char*);
void m_vprov_nbs_gr(const char*);


void vibshet(int metka_gr) //0-обычные проводки 1- по гуппам контрагентов
{
class iceb_u_str shet("");
iceb_u_str repl;


if(metka_gr == 0)
  repl.plus(gettext("Введите номер счета (субсчета)"));
else
  repl.plus(gettext("Введите номер счета с развернутым сальдо"));

naz:;

if(iceb_menu_vvod1(repl.ravno(),&shet,20,"",NULL) != 0)
 return;


iceb_u_str shetv;
iceb_u_str naimv;

if(shet.ravno()[0] == '\0')
 {
 if(iceb_l_plansh(1,&shetv,&naimv,NULL) != 0)
   return;
 else
  shet.new_plus(shetv.ravno());
 }
OPSHET rekshet;
if(iceb_prsh1(shet.ravno(),&rekshet,NULL) != 0)
  goto naz;


if(metka_gr == 1)
 if(rekshet.saldo == 0)
  {
   iceb_menu_soob(gettext("Счёт имеет свернутое сальдо !"),NULL);
   goto naz;  
  }

if(metka_gr == 0)
 {
  if(rekshet.stat == 0)
   m_vprov_bs(shet.ravno());
  else
   m_vprov_nbs(shet.ravno());
 }
if(metka_gr == 1)
 {
  if(rekshet.stat == 0)
   m_vprov_bs_gr(shet.ravno());
  else
   m_vprov_nbs_gr(shet.ravno());
 }
}
