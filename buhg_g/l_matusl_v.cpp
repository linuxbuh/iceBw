/*$Id: l_matusl_v.c,v 1.21 2013/09/26 09:46:50 sasa Exp $*/
/*29.02.2016	03.11.2004	Белых А.И.	l_matusl_v.c
ввод и корректировка услуг в документ материального учёта
*/
#include        "buhg_g.h"



enum
{
  FK2,
  FK3,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NAIM,
  E_KOLIH,
  E_CENA_SUMA,
  E_EI,
  E_PDV,
  E_KODUSL,
  KOLENTER  
 };

class matusl_v_m
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-начать расчёт 1 нет

  //Реквизиты документа
  
  short dd,md,gd;
  iceb_u_str nomdok;
  int skl;
  int nomer_zap;
  short metka_zap; //0-ввод новой записи 1- корректировка старой
  
  //Реквизиты введенные
  iceb_u_str naim_usl_v;
  iceb_u_str ei_v;
  iceb_u_str kolih_v;
  iceb_u_str cena_suma_v;
  iceb_u_str pdv_v;
  iceb_u_str kodusl;
    
  matusl_v_m() //Конструктор
   {
    clear_data();
    kl_shift=0;
    voz=1;
   }

  void read_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear_data();
   }
  void clear_data()
   {
    kodusl.new_plus("");
    naim_usl_v.new_plus("");
    ei_v.new_plus("");
    kolih_v.new_plus("");
    cena_suma_v.new_plus("");
    pdv_v.new_plus("");
   }
 };



gboolean   matusl_v_key_press(GtkWidget *widget,GdkEventKey *event,class matusl_v_m *data);
void    matusl_v_vvod(GtkWidget *widget,class matusl_v_m *data);
void  matusl_v_knopka(GtkWidget *widget,class matusl_v_m *data);
void  l_matusl_v_v_e_knopka(GtkWidget *widget,class matusl_v_m *data);


int matusl_zap(class matusl_v_m *data);

extern SQL_baza bd;


int l_matusl_v(int metka, //0- ввод новой записи 1- корректировка старой
short dd,short md,short gd,const char *nomdok,int skl,
float pnds,
int nomer_zap,
GtkWidget *wpredok)
{
char strsql[512];
class matusl_v_m data;
int gor=0,ver=0;
data.metka_zap=metka;
data.dd=dd;
data.md=md;
data.gd=gd;
data.nomdok.new_plus(nomdok);
data.skl=skl;
data.nomer_zap=nomer_zap;
data.name_window.plus(__FUNCTION__);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

if(metka == 0)
 {
  sprintf(strsql,"%.10g",pnds);
  data.pdv_v.new_plus(strsql);
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод услуги"));
 }
if(metka == 1)
 {
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Корректировка услуги"));
  SQL_str row;
  class SQLCURSOR cur;
  sprintf(strsql,"select * from Dokummat3 where nz=%d",nomer_zap);
  printf("strsql=%s\n",strsql);
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   {
    data.ei_v.new_plus(row[3]);
    data.kolih_v.new_plus(row[4]);
    data.cena_suma_v.new_plus(row[5]);
    data.pdv_v.new_plus(row[6]);
    data.naim_usl_v.new_plus(row[7]);
    data.kodusl.new_plus(row[10]);
    sprintf(strsql,"select naius from Uslugi where kodus=%s",data.kodusl.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
     {
      data.kodusl.plus(" ");
      data.kodusl.plus(row[0]);
     }
   }
 }
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(matusl_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=NULL;
if(metka == 0)
  label=gtk_label_new(gettext("Ввод услуги"));
if(metka == 1)
  label=gtk_label_new(gettext("Корректировка услуги"));

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


sprintf(strsql,"%s",gettext("Наименование услуги"));
label=gtk_label_new(strsql);
data.entry[E_NAIM] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NAIM]),99);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NAIM]), data.entry[E_NAIM], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NAIM], "activate",G_CALLBACK(matusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NAIM]),data.naim_usl_v.ravno());
gtk_widget_set_name(data.entry[E_NAIM],iceb_u_inttochar(E_NAIM));

sprintf(strsql,"%s",gettext("Количество"));
label=gtk_label_new(strsql);
data.entry[E_KOLIH] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLIH]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLIH]), data.entry[E_KOLIH], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLIH], "activate",G_CALLBACK(matusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLIH]),data.kolih_v.ravno());
gtk_widget_set_name(data.entry[E_KOLIH],iceb_u_inttochar(E_KOLIH));

sprintf(strsql,"%s",gettext("Цена/сумма"));
label=gtk_label_new(strsql);
data.entry[E_CENA_SUMA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_CENA_SUMA]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_SUMA]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_CENA_SUMA]), data.entry[E_CENA_SUMA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_CENA_SUMA], "activate",G_CALLBACK(matusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_CENA_SUMA]),data.cena_suma_v.ravno());
gtk_widget_set_name(data.entry[E_CENA_SUMA],iceb_u_inttochar(E_CENA_SUMA));

sprintf(strsql,"%s",gettext("Единица измерения"));
data.knopka_enter[E_EI]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.knopka_enter[E_EI], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_EI],"clicked",G_CALLBACK(l_matusl_v_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_EI],iceb_u_inttochar(E_EI));
gtk_widget_set_tooltip_text(data.knopka_enter[E_EI],gettext("Выбор единицы измерения"));

data.entry[E_EI] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_EI]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_EI]), data.entry[E_EI], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_EI], "activate",G_CALLBACK(matusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_EI]),data.ei_v.ravno());
gtk_widget_set_name(data.entry[E_EI],iceb_u_inttochar(E_EI));

sprintf(strsql,"%s",gettext("НДС"));
label=gtk_label_new(strsql);
data.entry[E_PDV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PDV]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PDV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_PDV]), data.entry[E_PDV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_PDV], "activate",G_CALLBACK(matusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PDV]),data.pdv_v.ravno());
gtk_widget_set_name(data.entry[E_PDV],iceb_u_inttochar(E_PDV));


sprintf(strsql,"%s",gettext("Код услуги"));
data.knopka_enter[E_KODUSL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODUSL]), data.knopka_enter[E_KODUSL], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODUSL],"clicked",G_CALLBACK(l_matusl_v_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODUSL],iceb_u_inttochar(E_KODUSL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODUSL],gettext("Выбор услуги"));

data.entry[E_KODUSL] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODUSL]), data.entry[E_KODUSL], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODUSL], "activate",G_CALLBACK(matusl_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODUSL]),data.kodusl.ravno());
gtk_widget_set_name(data.entry[E_KODUSL],iceb_u_inttochar(E_KODUSL));




sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введенной в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(matusl_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(matusl_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(matusl_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

return(data.voz);


}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  l_matusl_v_v_e_knopka(GtkWidget *widget,class matusl_v_m *data)
{
iceb_u_str kod;
iceb_u_str naim;
kod.new_plus("");
naim.new_plus("");

int knop=atoi(gtk_widget_get_name(widget));
//g_print("l_matusl_v_v_e_knopka knop=%d\n",knop);

switch (knop)
 {
   


  case E_KODUSL:
    if(l_uslugi(1,&kod,&naim,data->window) == 0)
     {
      data->kodusl.new_plus(kod.ravno());
      data->kodusl.plus(" ");
      data->kodusl.plus(naim.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODUSL]),data->kodusl.ravno());
     }
    return;  

  case E_EI:

    iceb_vibrek(1,"Edizmer",&data->ei_v,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_EI]),data->ei_v.ravno());

    return;  

 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   matusl_v_key_press(GtkWidget *widget,GdkEventKey *event,class matusl_v_m *data)
{

//printf("matusl_v_key_press\n");
switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
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
void  matusl_v_knopka(GtkWidget *widget,class matusl_v_m *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню


    matusl_zap(data);
        
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  

  case FK3:

    return;

  case FK4:
    data->clear_rek();
    return;
    
  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->read_rek(); //Читаем реквизиты меню
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    matusl_v_vvod(GtkWidget *widget,class matusl_v_m *data)
{

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {

  case E_NAIM:
    data->naim_usl_v.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOLIH:
    data->kolih_v.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_CENA_SUMA:
    data->cena_suma_v.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_EI:
    data->ei_v.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PDV:
    data->pdv_v.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/********************************/
/*запись информации*/
/************************/

int matusl_zap(class matusl_v_m *data)
{


if(data->naim_usl_v.getdlinna() <= 1 && data->kodusl.ravno_atoi() <= 0)
 {
  iceb_menu_soob(gettext("Не введено наименование услуги !"),data->window);
  return(1);
 }

char strsql[1024];

if(data->kodusl.ravno_atoi() > 0)
 {
  sprintf(strsql,"select kodus from Uslugi where kodus=%d",data->kodusl.ravno_atoi());
  if(iceb_sql_readkey(strsql,data->window) != 1)
   {
    sprintf(strsql,"%s %d !",gettext("Не найден код услуги"),data->kodusl.ravno_atoi());
    iceb_menu_soob(strsql,data->window);
    return(1);
   }
 }

time_t vrem;
time(&vrem);

if(data->metka_zap == 0)
 sprintf(strsql,"insert into Dokummat3 (datd,sklad,nomd,ei,kolih,cena,nds,naimu,ktoi,vrem,ku) \
values ('%d-%02d-%02d',%d,'%s','%s',%.10g,%.10g,%.10g,'%s',%d,%ld,%d)",
 data->gd,data->md,data->dd,data->skl,data->nomdok.ravno(),
 data->ei_v.ravno(),
 data->kolih_v.ravno_atof(),
 data->cena_suma_v.ravno_atof(),
 data->pdv_v.ravno_atof(),
 data->naim_usl_v.ravno(),
 iceb_getuid(data->window),vrem,
 data->kodusl.ravno_atoi());

if(data->metka_zap == 1)
  sprintf(strsql,"update Dokummat3 \
set \
ei='%s',\
kolih=%.10g,\
cena=%.10g,\
nds=%.10g,\
naimu='%s',\
ktoi=%d,\
vrem=%ld,\
ku=%d \
where nz=%d",
 data->ei_v.ravno(),
 data->kolih_v.ravno_atof(),
 data->cena_suma_v.ravno_atof(),
 data->pdv_v.ravno_atof(),
 data->naim_usl_v.ravno(),
 iceb_getuid(data->window),
 vrem,
 data->kodusl.ravno_atoi(),
 data->nomer_zap);


if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
 return(1);
 

return(0);

}
