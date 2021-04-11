/*$Id: vpld.c,v 1.26 2013/08/25 08:26:59 sasa Exp $*/
/*21.06.2015	24.03.2006	Белых А.И.	vpld.c
Ввод нового платёжного поручения
*/
#include "buhg_g.h"
#include "dok4w.h"
enum
{
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_NOMDOK,
  E_KODOP,
  E_SUMAP,
  E_DATAD,
  E_NDS,
  E_KOLKOP,
  E_KOD_NE_REZ,
  E_KOD_KONTR,
  KOLENTER  
 };

class vpld_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *window;
  GtkWidget *label_naim_kodop;
  GtkWidget *label_naim_kontr;
  GtkWidget *knopka_enter[KOLENTER];
  GtkTextBuffer *buffer;
  GtkWidget *ramka;
  class iceb_u_str name_window;
  short kl_shift;
  short voz;      //0-ввели 1 нет

  //Реквизиты шапки документа
  class iceb_u_str datad;
  class iceb_u_str nomdok;
  class iceb_u_str kodop;
  class iceb_u_str sumap;
  class iceb_u_str nds;
  class iceb_u_str kolkop;
  class iceb_u_str kodnr;
  class iceb_u_str kod_kontr;
  class iceb_u_str naz_plat;  

  class iceb_u_str tablica;
    
  vpld_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    clear_data();
   }

  void read_rek()
   {
    nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    datad.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATAD])));
    kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KODOP])));
    sumap.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SUMAP])));
    nds.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NDS])));
    kolkop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOLKOP])));
    kodnr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_NE_REZ])));
    kod_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KOD_KONTR])));

    xdkvnp_rt(&naz_plat,buffer); /*чтение назначения платежа*/

   }
  void clear_data()
   {
    kod_kontr.new_plus("");
    datad.new_plus("");
    nomdok.new_plus("");
    kodop.new_plus("");
    sumap.new_plus("");
    nds.new_plus("");
    kolkop.new_plus("");
    kodnr.new_plus("");
    naz_plat.new_plus("");
   }
  void clear_rek()
   {
    for(int i=0; i < KOLENTER-1; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
    clear_data();
   }
 };

gboolean   vpld_v_key_press(GtkWidget *widget,GdkEventKey *event,class vpld_data *data);
void    vpld_v_vvod(GtkWidget *widget,class vpld_data *data);
void  vpld_v_knopka(GtkWidget *widget,class vpld_data *data);
void vpld_kkvt(char *skod,char *nkod,GtkWidget *wpredok);
int vpld_pk(char *kod,GtkWidget *wpredok);
void  vpld_v_e_knopka(GtkWidget *widget,class vpld_data *data);
int vpld_f2(class vpld_data *data);


extern SQL_baza bd;
extern short kp; /*Количество копий платежки*/
//extern double   nds1; /*Н.Д.С.*/
extern double   okrg1; /*Округление*/
class REC rec;


int vpld(class iceb_u_str *datad, //дата документа
class iceb_u_str *nomdok,
const char *tablica,
GtkWidget *wpredok)
{
int gor=0;
int ver=0;
class vpld_data data;
char strsql[512];
short dd,md,gd;

rec.clear_data(); /*обязательно очистить*/

data.name_window.plus(__FUNCTION__);

data.tablica.new_plus(tablica);
if(datad->getdlinna() <= 1)
 {
  iceb_u_poltekdat(&dd,&md,&gd);
  char mescc[56];
  memset(mescc,'\0',sizeof(mescc));
  iceb_mesc(md,1,mescc);
  sprintf(strsql,"%02d %s %d%s",dd,mescc,gd,gettext("г."));
  data.datad.new_plus(strsql);
 }
else
 {
  data.datad.new_plus(datad->ravno());
  iceb_u_rsdat(&dd,&md,&gd,data.datad.ravno(),0);
 }

data.nomdok.new_plus(iceb_nomdok(gd,tablica,wpredok));

if(kp != 0)
 data.kolkop.new_plus(kp);
  
data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);
if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод нового платёжного поручения"));
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Ввод нового платёжного требования"));

gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(vpld_v_key_press),&data);

if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

GtkWidget *label=NULL;
if(iceb_u_SRAV(tablica,"Pltp",0) == 0)
  label=gtk_label_new(gettext("Ввод нового платёжного поручения"));
if(iceb_u_SRAV(tablica,"Pltt",0) == 0)
  label=gtk_label_new(gettext("Ввод нового платёжного требования"));


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
gtk_box_pack_start (GTK_BOX (vbox), label, FALSE, FALSE, 0);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox), hbox[i], FALSE, FALSE, 0);

/*назначение платежа*/
data.ramka=xkdvnp("",&data.buffer);
gtk_box_pack_start (GTK_BOX (vbox), data.ramka, FALSE, FALSE, 5);



gtk_box_pack_start (GTK_BOX (vbox), hboxknop, FALSE, FALSE, 0);


sprintf(strsql,"%s",gettext("Номер документа"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));



sprintf(strsql,"%s",gettext("Код операции"));
data.knopka_enter[E_KODOP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.knopka_enter[E_KODOP], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KODOP],"clicked",G_CALLBACK(vpld_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KODOP],iceb_u_inttochar(E_KODOP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KODOP],gettext("Выбор операции"));

data.entry[E_KODOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KODOP]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));

data.label_naim_kodop=gtk_label_new("   ");
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.label_naim_kodop, TRUE, TRUE, 0);

sprintf(strsql,"%s",gettext("Сумма"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMAP]), label, FALSE, FALSE, 0);

data.entry[E_SUMAP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SUMAP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_SUMAP]), data.entry[E_SUMAP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SUMAP], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SUMAP]),data.sumap.ravno());
gtk_widget_set_name(data.entry[E_SUMAP],iceb_u_inttochar(E_SUMAP));

sprintf(strsql,"%s",gettext("Дата"));
data.knopka_enter[E_DATAD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.knopka_enter[E_DATAD], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAD],"clicked",G_CALLBACK(vpld_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAD],iceb_u_inttochar(E_DATAD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAD],gettext("Выбор даты"));

data.entry[E_DATAD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAD]),30);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAD]), data.entry[E_DATAD], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAD], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAD]),data.datad.ravno());
gtk_widget_set_name(data.entry[E_DATAD],iceb_u_inttochar(E_DATAD));

sprintf(strsql,"%s",gettext("НДС"));
data.knopka_enter[E_NDS]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.knopka_enter[E_NDS], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_NDS],"clicked",G_CALLBACK(vpld_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NDS],iceb_u_inttochar(E_NDS));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NDS],gettext("Расчёт НДС"));

data.entry[E_NDS] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NDS]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.entry[E_NDS], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NDS], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NDS]),data.nds.ravno());
gtk_widget_set_name(data.entry[E_NDS],iceb_u_inttochar(E_NDS));

sprintf(strsql,"%s",gettext("Количество копий"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLKOP]), label, FALSE, FALSE, 0);

data.entry[E_KOLKOP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOLKOP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOLKOP]), data.entry[E_KOLKOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOLKOP], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOLKOP]),data.kolkop.ravno());
gtk_widget_set_name(data.entry[E_KOLKOP],iceb_u_inttochar(E_KOLKOP));

sprintf(strsql,"%s",gettext("Код не резидента"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NE_REZ]), label, FALSE, FALSE, 0);

data.entry[E_KOD_NE_REZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_NE_REZ]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_NE_REZ]), data.entry[E_KOD_NE_REZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_NE_REZ], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_NE_REZ]),data.kodnr.ravno());
gtk_widget_set_name(data.entry[E_KOD_NE_REZ],iceb_u_inttochar(E_KOD_NE_REZ));

sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KOD_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.knopka_enter[E_KOD_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KOD_KONTR],"clicked",G_CALLBACK(vpld_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KOD_KONTR],gettext("Выбор контрагента"));

data.entry[E_KOD_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KOD_KONTR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.entry[E_KOD_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOD_KONTR], "activate",G_CALLBACK(vpld_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOD_KONTR]),data.kod_kontr.ravno());
gtk_widget_set_name(data.entry[E_KOD_KONTR],iceb_u_inttochar(E_KOD_KONTR));

data.label_naim_kontr=gtk_label_new("   ");
gtk_box_pack_start (GTK_BOX (hbox[E_KOD_KONTR]), data.label_naim_kontr, TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Записать"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Записать введенную в меню информацию"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(vpld_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(vpld_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(vpld_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);


gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));

if(data.voz == 0)
 {
  datad->new_plus(data.datad.ravno());
  nomdok->new_plus(data.nomdok.ravno());
 }



return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  vpld_v_e_knopka(GtkWidget *widget,class vpld_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

class iceb_u_str kod("");
class iceb_u_str naim("");
double nds1=0.;

switch (knop)
 {
  case E_DATAD:

    if(iceb_calendar(&data->datad,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAD]),data->datad.ravno());
    return;  

  case E_KODOP:

    if(l_xdkop(1,&kod,&naim,data->tablica.ravno(),data->window) == 0)
     { 
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KODOP]),kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),naim.ravno());

      char bros[512];
      sprintf(bros,"Назначение платежа для операции %s",kod.ravno());
      
      if(iceb_poldan(bros,&naim,"nastdok.alx",1,data->window) == 0)
       {
        GtkTextIter iter;
        gtk_text_buffer_get_end_iter (data->buffer,&iter);        
        gtk_text_buffer_insert (data->buffer,&iter,naim.ravno(),-1);
       }
     }
    return;  

  case E_KOD_KONTR:

    if(iceb_l_kontr(1,&kod,&naim,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KOD_KONTR]),kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),naim.ravno());
     }
    return;  

  case E_NDS:
    data->read_rek(); //Читаем реквизиты меню
//    nds1=iceb_pnds(rec.dd,rec.md,rec.gd,data->window);
    nds1=iceb_pnds(data->datad.ravno(),data->window);
    rec.nds=data->sumap.ravno_atof()*nds1/(nds1+100);
    rec.nds=iceb_u_okrug(rec.nds,okrg1);
    if(rec.nds != 0.)
     {
      char bros[512];
      sprintf(bros,"%.2f",rec.nds);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NDS]),bros);

      sprintf(bros,"\n%s-%.2f %s",
      gettext("НДС"),
      rec.nds,
      gettext("грн."));
      
      GtkTextIter iter;
      gtk_text_buffer_get_end_iter (data->buffer,&iter);        
      gtk_text_buffer_insert (data->buffer,&iter,bros,-1);
     }
    return;  
      

 }
}
/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   vpld_v_key_press(GtkWidget *widget,GdkEventKey *event,class vpld_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);

  case GDK_KEY_F3:
//    g_signal_emit_by_name(data->knopka[FK3],"clicked");
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
void  vpld_v_knopka(GtkWidget *widget,class vpld_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(vpld_f2(data) != 0)
      return;
    
    gtk_widget_destroy(data->window);
    data->window=NULL;
    data->voz=0;
    return;  


  case FK4:
    data->clear_rek();
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

void    vpld_v_vvod(GtkWidget *widget,class vpld_data *data)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {

  case E_DATAD:
    data->datad.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
//    if(vpld_prov_datad(data) != 0)
//      return;
    break;

  case E_NOMDOK:
    data->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
//    if(vpld_prov_nomdok(data) != 0)
//      return;
    break;

  case E_KODOP:
    data->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->kodop.getdlinna() > 1)
     {
      if(iceb_u_SRAV(data->tablica.ravno(),"Pltp",0) == 0 || iceb_u_SRAV(data->tablica.ravno(),"Tpltp",0) == 0)
       sprintf(strsql,"select naik from Opltp where kod='%s'",data->kodop.ravno());

      if(iceb_u_SRAV(data->tablica.ravno(),"Pltt",0) == 0 || iceb_u_SRAV(data->tablica.ravno(),"Tpltt",0) == 0)
       sprintf(strsql,"select naik from Opltp where kod='%s'",data->kodop.ravno());

      if(iceb_sql_readkey(strsql,&row,&cur,data->window) != 1)
       {
        sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->kodop.ravno());
        iceb_menu_soob(strsql,data->window);
       }
      else
       {
        gtk_label_set_text(GTK_LABEL(data->label_naim_kodop),row[0]);
        class iceb_u_str naznach("");
        sprintf(strsql,"Назначение платежа для операции %s",data->kodop.ravno());
                 
        if(iceb_poldan(strsql,&naznach,"nastdok.alx",1,data->window) == 0)
         {
          GtkTextIter iter;
          gtk_text_buffer_get_end_iter (data->buffer,&iter);        
          gtk_text_buffer_insert (data->buffer,&iter,naznach.ravno(),-1);
         }

       }
     }
    break;

  case E_SUMAP:
    data->sumap.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NDS:
    data->nds.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOLKOP:
    data->kolkop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_NE_REZ:
    data->kodnr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOD_KONTR:
    data->kod_kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    sprintf(strsql,"select naikon,kod,mfo,nomsh from Kontragent where kodkon='%s'",data->kod_kontr.ravno());
    if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
     {
      gtk_label_set_text(GTK_LABEL(data->label_naim_kontr),row[0]);
     }
    break;


 }

enter+=1;
  
if(enter >= KOLENTER-1)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
/**************************************/
/*Ввод информации*/
/******************************/
int vpld_f2(class vpld_data *data)
{
char strsql[512];

if(data->nomdok.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён номер документа !"),data->window);
  return(1);
 }

/*Проверяем код операции*/
if(data->kodop.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введена операция !"),data->window);
  return(1);
 }

/*Проверяем код контрагента*/
if(data->kod_kontr.getdlinna() <= 1)
 {
  iceb_menu_soob(gettext("Не введён код контрагента!"),data->window);
  return(1);
 }

if(iceb_u_SRAV(data->tablica.ravno(),"Pltp",0) == 0)
 sprintf(strsql,"select naik from Opltp where kod='%s'",data->kodop.ravno());    

if(iceb_u_SRAV(data->tablica.ravno(),"Pltt",0) == 0)
 sprintf(strsql,"select naik from Opltt where kod='%s'",data->kodop.ravno());

if(iceb_sql_readkey(strsql,data->window) != 1)
 {
  sprintf(strsql,"%s %s !",gettext("Не найден код операции"),data->kodop.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

if(iceb_u_rsdat(&rec.dd,&rec.md,&rec.gd,data->datad.ravno(),0) != 0)
 {
  iceb_menu_soob(gettext("Неправильно введена дата !"),data->window);
  return(1);
 }


/***********************
if(VV.VVOD_SPISOK_return_data(6)[0] != '\0')
if(iceb_u_rsdat(&rec.dv,&rec.mv,&rec.gv,VV.VVOD_SPISOK_return_data(6),0) != 0)
 {
  SOOB.VVOD_delete();
  SOOB.VVOD_SPISOK_add_MD(gettext("Не верно введена дата валютирования !"));
  soobshw(&SOOB,stdscr,-1,-1,0,1);
  par=3;
  goto naz;
 }
***********************/


if(data->sumap.ravno_atof() == 0.)
 {
  iceb_menu_soob(gettext("Не введена сумма платежа !"),data->window);
  return(1);
 }

kp=data->kolkop.ravno_atoi();
if(kp == 0)
 {
  iceb_menu_soob(gettext("Не введено количество копий !"),data->window);
  return(1);
 }

/*Проверяем номер платежного поручения*/

sprintf(strsql,"select nomd from %s where datd >= '%04d-01-01' and \
datd <= '%04d-12-31' and nomd='%s'",data->tablica.ravno(),rec.gd,rec.gd,data->nomdok.ravno());

if(iceb_sql_readkey(strsql,data->window) >= 1)
 {
  sprintf(strsql,gettext("Документ с номером %s уже есть !"),data->nomdok.ravno());
  iceb_menu_soob(strsql,data->window);
  return(1);
 }

//memset(rec.nomdk,'\0',sizeof(rec.nomdk));
//strncpy(rec.nomdk,VV.VVOD_SPISOK_return_data(0),sizeof(rec.nomdk)-1);
//strncpy(rec.kodnr,VV.VVOD_SPISOK_return_data(7),sizeof(rec.kodnr)-1);

rec.nomdk.new_plus(data->nomdok.ravno());

rec.nds=data->nds.ravno_atof();
rec.sumd=data->sumap.ravno_atof();
if(rec.nds != 0.)
  rec.nds=iceb_u_okrug(rec.nds,okrg1);
rec.sumd=iceb_u_okrug(rec.sumd,okrg1);

rec.kodnr.new_plus(data->kodnr.ravno());
rec.naz_plat.new_plus(data->naz_plat.ravno());
rec.kodop.new_plus(data->kodop.ravno());
/*Берем реквизиты организации из базы*/
if(vzrkbzw(data->kod_kontr.ravno(),data->window) != 0)
 return(1);

return(0);
}
