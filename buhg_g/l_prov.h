/*$Id: l_prov.h,v 1.17 2012-01-12 12:35:36 sasa Exp $*/
/*13.02.2016	07.01.2004	Белых А.И.	l_prov.h
*/
#include "prov_poi.h"
#include "prov_rek_data.h"
enum
{
  FK2,
  FK3,
  FK5,
  FK6,
  FK10,
  SFK3,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_SHETK,
  E_DATA,
  E_KOMENT,
  E_DEBET,
  E_KREDIT,
  E_KONTR,
  KOLENTER  
 };

class  prov_data
 {
  public:

  class prov_poi_data *rek_poi;
  class prov_rek_data rek_vibr;
  
  GtkWidget *label_kolstr;
  GtkWidget *sw;
  GtkWidget *entry[KOLENTER];
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *vwindow;
  GtkWidget *knopka[KOL_F_KL];
  int       kolzap;
  int       snanomer;   //номер записи на которую надостать
  short     kl_shift; //0-отжата 1-нажата  

  //Конструктор
  prov_data()
   {
    kolzap=0;
    snanomer=0;
    kl_shift=0;
    vwindow=window=treeview=NULL;
   }      


 };

extern SQL_baza	bd;

void prov_create_list (class prov_data *data);
