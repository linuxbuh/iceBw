/*$Id: l_zkartmat_v.c,v 1.26 2013/10/04 07:27:16 sasa Exp $*/
/*03.03.2016	25.05.2004	Белых А.И.	l_zkartmat_v.c
Корректировка реквизитов карточки материалла
*/
#include "buhg_g.h"

enum
{
  FK2,
  FK3,
  FK10,
  KOL_F_KL
};

enum
 {
  E_CENA,
  E_EI,
  E_NDS,
  E_KRATN,
  E_KODTAR,
  E_FAS,
  E_SHETU,
  E_CENAPR,
  E_NOMZAK,
  E_DATA_KON_IS,
  E_DATAN,
  E_INNOM,
  E_REGNOM,
  KOLENTER  
 };

class l_zkartmat_v_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет
  
  class rek_kartmat *rk; 
  iceb_u_str shetz; //Счёт до корректировки карточки
  double cenaz;     //Цена до корректировки карточки
  iceb_u_str ei;
      
  l_zkartmat_v_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
 };


void    l_zkartmat_v_v_vvod(GtkWidget *widget,class l_zkartmat_v_data *data);
void  l_zkartmat_v_v_knopka(GtkWidget *widget,class l_zkartmat_v_data *data);
void   l_zkartmat_v_rekviz(class l_zkartmat_v_data *data);
gboolean   l_zkartmat_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zkartmat_v_data *data);
void   zkartmat_v_rekviz(class l_zkartmat_v_data *data);
int zkartmat_zrk(class l_zkartmat_v_data *data);

void korcen(int skl,int nk,double cn,int mbz,GtkWidget *wpredok);

extern SQL_baza bd;
extern class iceb_u_str shrt;    /*Счета розничной торговли*/
extern double	okrcn;  /*Округление цены*/

int l_zkartmat_v(class rek_kartmat *data_kar,GtkWidget *wpredok)
{
class l_zkartmat_v_data data;
int gor=0,ver=0;
data.name_window.plus(__FUNCTION__);
data.rk=data_kar;
data.cenaz=data.rk->cena.ravno_atof();
data.shetz.plus(data.rk->shet.ravno());
data.ei.plus(data.rk->ei.ravno());

char strsql[512];

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка реквизитов карточки материалла"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }
 
g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(l_zkartmat_v_v_key_press),&data);

iceb_u_str zagolov;
zagolov.new_plus(gettext("Корректировка реквизитов карточки материалла"));



GtkWidget *label=gtk_label_new(zagolov.ravno());

GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

sprintf(strsql,"%s",gettext("Учётная цена"));
label=gtk_label_new(strsql);
data.entry[E_CENA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA]), data.entry[E_CENA],FALSE, TRUE, 0);
g_signal_connect(data.entry[E_CENA], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA]),data.rk->cena.ravno());
gtk_widget_set_name(data.entry[E_CENA],iceb_u_inttochar(E_CENA));

label=gtk_label_new(gettext("Единица измерения"));
data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI],FALSE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.rk->ei.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

label=gtk_label_new(gettext("НДС"));
data.entry[E_NDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NDS]),5);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.entry[E_NDS], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_NDS], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);

sprintf(strsql,"%s",data.rk->nds.ravno());
if(data.rk->mnds.ravno()[0] == '1')
  sprintf(strsql,"+%s",data.rk->nds.ravno());

gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS]),data.rk->nds.ravno());
gtk_widget_set_name(data.entry[E_NDS],iceb_u_inttochar(E_NDS));

label=gtk_label_new(gettext("Кратность"));
data.entry[E_KRATN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KRATN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KRATN]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KRATN]), data.entry[E_KRATN], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_KRATN], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KRATN]),data.rk->krat.ravno());
gtk_widget_set_name(data.entry[E_KRATN],iceb_u_inttochar(E_KRATN));

label=gtk_label_new(gettext("Код тары"));
data.entry[E_KODTAR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODTAR]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODTAR]), data.entry[E_KODTAR], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_KODTAR], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODTAR]),data.rk->kodtar.ravno());
gtk_widget_set_name(data.entry[E_KODTAR],iceb_u_inttochar(E_KODTAR));

label=gtk_label_new(gettext("Фасовка"));
data.entry[E_FAS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FAS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_FAS]), data.entry[E_FAS], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_FAS], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FAS]),data.rk->fas.ravno());
gtk_widget_set_name(data.entry[E_FAS],iceb_u_inttochar(E_FAS));

label=gtk_label_new(gettext("Счёт учёта"));
data.entry[E_SHETU] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHETU]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETU]), data.entry[E_SHETU], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_SHETU], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETU]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHETU],iceb_u_inttochar(E_SHETU));

label=gtk_label_new(gettext("Цена продажи"));
data.entry[E_CENAPR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENAPR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENAPR]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENAPR]), data.entry[E_CENAPR], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_CENAPR], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENAPR]),data.rk->cenapr.ravno());
gtk_widget_set_name(data.entry[E_CENAPR],iceb_u_inttochar(E_CENAPR));

label=gtk_label_new(gettext("Номер заказа"));
gtk_box_pack_start (GTK_BOX (hbox[E_NOMZAK]), label, TRUE, FALSE, 0);

data.entry[E_NOMZAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMZAK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMZAK]), data.entry[E_NOMZAK], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_NOMZAK], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMZAK]),data.rk->nomzak.ravno());
gtk_widget_set_name(data.entry[E_NOMZAK],iceb_u_inttochar(E_NOMZAK));

label=gtk_label_new(gettext("Конечная дата использования"));
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_KON_IS]), label, TRUE, FALSE, 0);

data.entry[E_DATA_KON_IS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_KON_IS]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_KON_IS]), data.entry[E_DATA_KON_IS], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_DATA_KON_IS], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_KON_IS]),data.rk->data_kon_is.ravno());
gtk_widget_set_name(data.entry[E_DATA_KON_IS],iceb_u_inttochar(E_DATA_KON_IS));

if(data.rk->rnd.getdlinna() <= 1)
  label=gtk_label_new(gettext("Дата начала эксплуатации"));
else
  label=gtk_label_new(gettext("Дата регистрации"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datv.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));


if(data.rk->rnd.getdlinna() <= 1)
  label=gtk_label_new(gettext("Инв-ный номер"));
else
  label=gtk_label_new(gettext("Серия мед.препарата"));

data.entry[E_INNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_INNOM]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_INNOM]), data.entry[E_INNOM], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_INNOM], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_INNOM]),data.rk->innom.ravno());
gtk_widget_set_name(data.entry[E_INNOM],iceb_u_inttochar(E_INNOM));

if(data.rk->rnd.getdlinna() <= 1)
label=gtk_label_new(gettext("Регистрационный номер"));
data.entry[E_REGNOM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_REGNOM]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_REGNOM]), label, TRUE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_REGNOM]), data.entry[E_REGNOM], FALSE, TRUE, 0);
g_signal_connect(data.entry[E_REGNOM], "activate",G_CALLBACK(l_zkartmat_v_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_REGNOM]),data.rk->rnd.ravno());
gtk_widget_set_name(data.entry[E_REGNOM],iceb_u_inttochar(E_REGNOM));


sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(l_zkartmat_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F3 %s",gettext("Реквизиты"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Просмотр нужных списков, для заполнения нужными кодами реквизитов меню"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(l_zkartmat_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(l_zkartmat_v_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

 
return(data.voz);

}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   l_zkartmat_v_v_key_press(GtkWidget *widget,GdkEventKey *event,class l_zkartmat_v_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  case ICEB_REG_L:
  case ICEB_REG_R:
//    printf("Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);
 }

return(TRUE);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  l_zkartmat_v_v_knopka(GtkWidget *widget,class l_zkartmat_v_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    if(zkartmat_zrk(data) != 0)
      return;

    gtk_widget_destroy(data->window);
    data->window=NULL;

    data->voz=0;
    return;  

  case FK3:
    zkartmat_v_rekviz(data);
    return;

    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    l_zkartmat_v_v_vvod(GtkWidget *widget,class l_zkartmat_v_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_CENA:
    data->rk->cena.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_EI:
    data->rk->ei.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NDS:
    data->rk->nds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KRATN:
    data->rk->krat.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODTAR:
    data->rk->kodtar.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_FAS:
    data->rk->fas.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHETU :
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_CENAPR :
    data->rk->cenapr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMZAK :
    data->rk->nomzak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATA_KON_IS :
    data->rk->data_kon_is.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAN :
    data->rk->datv.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_INNOM :
    data->rk->innom.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_REGNOM :
    data->rk->rnd.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************/
/*Работа с реквизитами*/
/***********************/
void   zkartmat_v_rekviz(class l_zkartmat_v_data *data)
{
short nomer=0;

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));

zagolovok.plus(gettext("Выберите нужное"));

punkt_m.plus(gettext("Просмотр списка счетов"));
punkt_m.plus(gettext("Просмотр списка единиц измерения"));

nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,nomer,data->window);



iceb_u_str kod;
iceb_u_str naikod;

switch (nomer)
 {
  case -1:
    return;
    
  case 0:
    iceb_vibrek(1,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETU]),data->rk->shet.ravno());
    break;

  case 1:
    iceb_vibrek(1,"Edizmer",&data->rk->ei,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->rk->ei.ravno());
    break;

 }

}
/****************************************/
/*Запись реквизитов карточки*/
/***************************/
int zkartmat_zrk(class l_zkartmat_v_data *data)
{
char strsql[512];

/*Проверяем не является ли счёт счетом розничной торговли*/
if(shrt.getdlinna() > 1)
 {
  if(iceb_u_proverka(shrt.ravno(),data->rk->shet.ravno(),0,1) != 0)
   if(data->rk->cenapr.ravno_atof() != 0.)
    {
     class iceb_u_spisok repl;
     
     sprintf(strsql,gettext("Счёт %s не является счетом розничной торговли !"),data->rk->shet.ravno());
     repl.plus(strsql);
     
     repl.plus(gettext("Цена розничная не вводится"));
     iceb_menu_soob(&repl,data->window);
     return(1);
    }

  if(iceb_u_proverka(shrt.ravno(),data->rk->shet.ravno(),0,1) == 0)
   if(data->rk->cenapr.ravno_atof() == 0.)
    {
     class iceb_u_spisok repl;
     
     sprintf(strsql,gettext("Счёт %s является счетом розничной торговли !"),data->rk->shet.ravno());
     repl.plus(strsql);
     
     repl.plus(gettext("Цена розничная должна быть введена"));
     iceb_menu_soob(&repl,data->window);
     return(1);
    }

 }    

short d,m,g;

prdppw(data->rk->skl.ravno_atoi(),data->rk->n_kart.ravno_atoi(),&d,&m,&g,data->window);

if(iceb_u_SRAV(data->shetz.ravno(),data->rk->shet.ravno(),0) != 0 || data->cenaz != data->rk->cena.ravno_atof())
if(iceb_pblok(m,g,ICEB_PS_MU,data->window) != 0)
 {
  iceb_u_spisok repl;
  
  sprintf(strsql,gettext("Карточка открыта %d.%d.%d, эта дата заблокирована"),d,m,g);
  repl.plus(strsql);
  repl.plus(gettext("Корректировка невозможна !"));


  iceb_menu_soob(&repl,data->window);
  return(1);
 }

/*Проверяем правильность введения единицы измерения*/
sprintf(strsql,"select naik from Edizmer where kod='%s'",data->rk->ei.ravno());

if(sql_readkey(&bd,strsql) != 1)
 {
  iceb_u_str repl;
  sprintf(strsql,"%s %s !",gettext("Не найдено единицу измерения"),data->rk->ei.ravno());
  repl.plus(strsql);
  iceb_menu_soob(&repl,data->window);
  return(1);
 }    

OPSHET shetv;
/*Проверяем правильность введения счета*/
if(iceb_prsh1(data->rk->shet.ravno(),&shetv,data->window) != 0)
  return(1);

if(data->rk->datv.getdlinna() > 1)
 if(iceb_u_rsdat(&d,&m,&g,data->rk->datv.ravno(),1) != 0)
  {
   iceb_u_str repl;
   repl.plus(gettext("Не верно введена дата !"));
   
   iceb_menu_soob(&repl,data->window);
   return(1);

  }

if(data->rk->data_kon_is.getdlinna() > 1)
 if(data->rk->data_kon_is.prov_dat() != 0)
  {
   iceb_menu_soob(gettext("Неправильно введена конечная дата использоваия !"),data->window);
   return(1);

  }

data->rk->mnds.new_plus("0");
if(data->rk->nds.ravno()[0] == '+')
  data->rk->mnds.new_plus("1");

if(zapkartm(1,data->rk,data->window) != 0)
 return(1);

if(iceb_u_SRAV(data->shetz.ravno(),data->rk->shet.ravno(),0) != 0 || data->cenaz != data->rk->cena.ravno_atof())
 {
  int i1=0;
  if( data->cenaz == data->rk->cena.ravno_atof())
   i1=1; /*Пометить только документы*/
  double cena=0.;
  cena=iceb_u_okrug(data->rk->cena.ravno_atof(),okrcn);

  korcen(data->rk->skl.ravno_atoi(),data->rk->n_kart.ravno_atoi(),cena,i1,data->window);
 }       

/*Исправляем единицу измерения в документах*/
if(iceb_u_SRAV(data->rk->ei.ravno(),data->ei.ravno(),0) != 0)
 {
  time_t vrem;
  time(&vrem);
  sprintf(strsql,"update Dokummat1 \
set \
ei='%s',\
ktoi=%d,\
vrem= %ld \
where sklad=%d and kodm=%d and nomkar=%d",
  data->rk->ei.ravno(),iceb_getuid(data->window),vrem,
  data->rk->skl.ravno_atoi(),data->rk->kodm.ravno_atoi(),data->rk->n_kart.ravno_atoi());

  if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
   return(1);
  
 }


return(0);

}

/*********************************************************/
/*Корректировка цены на приход в карточках и в накладных*/
/*********************************************************/
void		korcen(int skl,int nk,double cn,
int mbz, //0-сена изменилась 1-нет*/
GtkWidget *wpredok)
{
SQL_str         row,row1,row2;
time_t		vrem;
char		strsql[512];
long		kolstr;
short		d,m,g;
short		dp,mp,gp;

sprintf(strsql,"select nomd,datd,datdp,tipz from Zkart where sklad=%d and nomk=%d",
skl,nk);
SQLCURSOR cur;
SQLCURSOR cur1;
SQLCURSOR cur2;
if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,wpredok);

if(kolstr == 0)
  return;

time(&vrem);

while(cur.read_cursor(&row) != 0)
 {
  iceb_u_rsdat(&dp,&mp,&gp,row[2],2);
  iceb_u_rsdat(&d,&m,&g,row[1],2);
  sprintf(strsql,"select kodop,nomd,pro from Dokummat \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
  g,m,d,skl,row[0]);

  if(sql_readkey(&bd,strsql,&row1,&cur1) != 1)
   {
    continue;
   }


  /*Помечаем что проводки надо проверить*/
  if(row1[2][0] == '1' && iceb_u_SRAV(row1[1],"000",0) != 0)
   {
    /*Определяем тип операции*/
    if(row[3][0] == '1')
     {
      sprintf(strsql,"select vido,prov from Prihod where kod='%s'",
      row1[0]);
      if(sql_readkey(&bd,strsql,&row2,&cur2) != 1)
       {
        printf("Не найден код операции %s !\n",row1[1]);
       }


       if(row2[0][0] != '0' || row2[1][0] == '1')
        goto vp1;
      }

    /*Корректировка записи*/
    sprintf(strsql,"update Dokummat \
set \
pro=0,\
ktoi=%d,\
vrem= %ld \
where datd='%d-%02d-%02d' and sklad=%d and nomd='%s'",
    iceb_getuid(wpredok),vrem,
    g,m,d,skl,row[0]);

    if(sql_zap(&bd,strsql) != 0)
     {
      iceb_msql_error(&bd,"",strsql,wpredok);
     }
   }

vp1:;

  if(mbz == 1)
    continue;
    
  if(row[3][0] == '1')
   {
    
    sprintf(strsql,"update Zkart \
set \
cena=%.10g,\
ktoi=%d,\
vrem= %ld \
where sklad=%d and nomk=%d and nomd='%s' and datdp='%d-%02d-%02d'",
    cn,iceb_getuid(wpredok),vrem,
    skl,nk,row[0],gp,mp,dp);

    if(sql_zap(&bd,strsql) != 0)
     {
      iceb_msql_error(&bd,"",strsql,wpredok);
     }


    sprintf(strsql,"update Dokummat1 \
set \
cena=%.10g,\
ktoi=%d,\
vrem=%ld \
where sklad=%d and nomkar=%d and nomd='%s' and datd='%s'",
    cn,iceb_getuid(wpredok),vrem,
    skl,nk,row[0],row[1]);

    if(sql_zap(&bd,strsql) != 0)
     {
      iceb_msql_error(&bd,"",strsql,wpredok);
     }
  
   }

 }

}
