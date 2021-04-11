/*$Id: m_poiprov.c,v 1.37 2013/09/26 09:46:51 sasa Exp $*/
/*03.03.2016	06.01.2004	Белых А.И.	m_poiprov.c
Меню для ввода реквизитов поиска проводок
*/

#include  "buhg_g.h"
#include  "prov_poi.h"

enum
{
  FK1,
  FK2,
  FK4,
  FK10,
  KOL_F_KL
};

enum
 {
  E_SHET,
  E_SHETK,
  E_KONTR,
  E_KOMENT,
  E_DEBET,
  E_KREDIT,
  E_DATAN,
  E_DATAK,
  E_METKA,
  E_NOMDOK,
  E_KODOP,
  E_GRUPAK,
  E_KEKV,
  E_DATANZ,
  E_DATAKZ,
  KOLENTER  
 };

class poiprov_data
 {
  public:
  
  class prov_poi_data *rk;
  
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *window;
  GtkWidget *label_link;
  class iceb_u_str name_window;
  short kl_shift;
  int voz;
  //Конструктор
  poiprov_data()
   {
    voz=1;
    kl_shift=0;
    window=NULL;
   }

  void activate()
   {
    for(int i=0; i < KOLENTER; i++)
      g_signal_emit_by_name(entry[i],"activate");
   }

  void clear_zero()
   {
    rk->clear_zero();
    for(int i=0; i < KOLENTER; i++)
      gtk_entry_set_text(GTK_ENTRY(entry[i]),"");
   }

 };

void    poiprov_v_vvod(GtkWidget *widget,class poiprov_data *data);
void  poiprov_v_knopka(GtkWidget *widget,class poiprov_data *data);
gboolean   poiprov_v_key_press(GtkWidget *widget,GdkEventKey *event,class poiprov_data *data);
void      poiprov_rek_metka(class poiprov_data *data);
void  m_poiprov_v_e_knopka(GtkWidget *widget,class poiprov_data *data);

extern SQL_baza	bd;



int m_poiprov(class prov_poi_data *rkp)
{


class iceb_u_str stroka;
char strsql[512];
class poiprov_data data;
int gor=0,ver=0;

//Делаем ссылку на структуру в которой остались реквизиты от предыдущей работы
data.rk=rkp;
data.name_window.plus(__FUNCTION__);

//printf("m_poiprov\n");

data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

if(iceb_sizwr(data.name_window.ravno(),&gor,&ver) == 0)
   gtk_window_set_default_size (GTK_WINDOW  (data.window),gor,ver);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Поиск"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(poiprov_v_key_press),&data);

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

stroka.new_plus(gettext("Поиск проводок"));


stroka.ps_plus(gettext("По невведенным реквизитам меню поиск не выполняется"));

GtkWidget *label=gtk_label_new(stroka.ravno());

gtk_container_add (GTK_CONTAINER (data.window), vbox);
gtk_container_add (GTK_CONTAINER (vbox), label);

for(int i=0; i < KOLENTER; i++)
  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

gtk_container_add (GTK_CONTAINER (vbox), hboxknop);


sprintf(strsql,"%s (,,)",gettext("Счёт"));
data.knopka_enter[E_SHET]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.knopka_enter[E_SHET], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHET],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHET],iceb_u_inttochar(E_SHET));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHET],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHET] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHET]), data.entry[E_SHET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHET], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHET]),data.rk->shet.ravno());
gtk_widget_set_name(data.entry[E_SHET],iceb_u_inttochar(E_SHET));

sprintf(strsql,"%s (,,)",gettext("Счёт корреспондент"));
data.knopka_enter[E_SHETK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.knopka_enter[E_SHETK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_SHETK],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_SHETK],iceb_u_inttochar(E_SHETK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_SHETK],gettext("Выбор счёта в плане счетов"));

data.entry[E_SHETK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_SHETK]), data.entry[E_SHETK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_SHETK], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHETK]),data.rk->shetk.ravno());
gtk_widget_set_name(data.entry[E_SHETK],iceb_u_inttochar(E_SHETK));

sprintf(strsql,"%s (,,)",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk->kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

label=gtk_label_new(gettext("Коментарий"));
data.entry[E_KOMENT] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KOMENT]), data.entry[E_KOMENT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KOMENT], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KOMENT]),data.rk->koment.ravno());
gtk_widget_set_name(data.entry[E_KOMENT],iceb_u_inttochar(E_KOMENT));

label=gtk_label_new(gettext("Дебет"));
data.entry[E_DEBET] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DEBET]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_DEBET]), data.entry[E_DEBET], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DEBET], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DEBET]),data.rk->debet.ravno());
gtk_widget_set_name(data.entry[E_DEBET],iceb_u_inttochar(E_DEBET));


label=gtk_label_new(gettext("Кредит"));
data.entry[E_KREDIT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KREDIT]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KREDIT]), data.entry[E_KREDIT], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KREDIT], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KREDIT]),data.rk->kredit.ravno());
gtk_widget_set_name(data.entry[E_KREDIT],iceb_u_inttochar(E_KREDIT));


sprintf(strsql,"%s (%s)(%s)",gettext("Дата начала поиска"),gettext("по дате проводки"),gettext("д.м.г"));
data.knopka_enter[E_DATAN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.knopka_enter[E_DATAN], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAN],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAN],iceb_u_inttochar(E_DATAN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAN],gettext("Выбор даты"));

data.entry[E_DATAN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAN]), data.entry[E_DATAN], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAN], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAN]),data.rk->datan.ravno());
gtk_widget_set_name(data.entry[E_DATAN],iceb_u_inttochar(E_DATAN));

sprintf(strsql,"%s (%s)(%s)",gettext("Дата конца"),gettext("по дате проводки"),gettext("д.м.г"));
data.knopka_enter[E_DATAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.knopka_enter[E_DATAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAK],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAK],iceb_u_inttochar(E_DATAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAK],gettext("Выбор даты"));

data.entry[E_DATAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAK]), data.entry[E_DATAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAK], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAK]),data.rk->datak.ravno());
gtk_widget_set_name(data.entry[E_DATAK],iceb_u_inttochar(E_DATAK));

sprintf(strsql,"%s (,,)",gettext("Метка проводки"));
data.knopka_enter[E_METKA]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA]), data.knopka_enter[E_METKA], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_METKA],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_METKA],iceb_u_inttochar(E_METKA));
gtk_widget_set_tooltip_text(data.knopka_enter[E_METKA],gettext("Выбор метки проводки"));

data.entry[E_METKA] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_METKA]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_METKA]), data.entry[E_METKA], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_METKA], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_METKA]),data.rk->metka.ravno());
gtk_widget_set_name(data.entry[E_METKA],iceb_u_inttochar(E_METKA));

label=gtk_label_new(gettext("Номер документа"));
data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk->nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));

stroka.new_plus(gettext("Код операции"));
stroka.plus(" (,,)");
label=gtk_label_new(stroka.ravno());
data.entry[E_KODOP] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KODOP]), data.entry[E_KODOP], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KODOP], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KODOP]),data.rk->kodop.ravno());
gtk_widget_set_name(data.entry[E_KODOP],iceb_u_inttochar(E_KODOP));


sprintf(strsql,"%s (,,)",gettext("Код группы контрагента"));
data.knopka_enter[E_GRUPAK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPAK]), data.knopka_enter[E_GRUPAK], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_GRUPAK],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_GRUPAK],iceb_u_inttochar(E_GRUPAK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_GRUPAK],gettext("Выбор группы контрагента"));

data.entry[E_GRUPAK] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_GRUPAK]), data.entry[E_GRUPAK], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_GRUPAK], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_GRUPAK]),data.rk->grupak.ravno());
gtk_widget_set_name(data.entry[E_GRUPAK],iceb_u_inttochar(E_GRUPAK));

sprintf(strsql,"%s (,,)",gettext("КЭКЗ"));
label=gtk_label_new(strsql);
data.entry[E_KEKV] = gtk_entry_new();
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), label, FALSE, FALSE, 0);
gtk_box_pack_start (GTK_BOX (hbox[E_KEKV]), data.entry[E_KEKV], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_KEKV], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KEKV]),data.rk->kredit.ravno());
gtk_widget_set_name(data.entry[E_KEKV],iceb_u_inttochar(E_KEKV));





sprintf(strsql,"%s (%s)(%s)",gettext("Дата начала поиска"),gettext("по дате записи"),gettext("д.м.г"));
data.knopka_enter[E_DATANZ]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATANZ]), data.knopka_enter[E_DATANZ], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATANZ],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATANZ],iceb_u_inttochar(E_DATANZ));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATANZ],gettext("Выбор даты"));

data.entry[E_DATANZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATANZ]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATANZ]), data.entry[E_DATANZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATANZ], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATANZ]),data.rk->datanz.ravno());
gtk_widget_set_name(data.entry[E_DATANZ],iceb_u_inttochar(E_DATANZ));

sprintf(strsql,"%s (%s)(%s)",gettext("Дата конца"),gettext("по дате записи"),gettext("д.м.г"));
data.knopka_enter[E_DATAKZ]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAKZ]), data.knopka_enter[E_DATAKZ], FALSE, FALSE, 0);
g_signal_connect(data.knopka_enter[E_DATAKZ],"clicked",G_CALLBACK(m_poiprov_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATAKZ],iceb_u_inttochar(E_DATAKZ));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATAKZ],gettext("Выбор даты"));

data.entry[E_DATAKZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATAKZ]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATAKZ]), data.entry[E_DATAKZ], TRUE, TRUE, 0);
g_signal_connect(data.entry[E_DATAKZ], "activate",G_CALLBACK(poiprov_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATAKZ]),data.rk->datakz.ravno());
gtk_widget_set_name(data.entry[E_DATAKZ],iceb_u_inttochar(E_DATAKZ));









sprintf(strsql,"<a href=\"%s/%s.html\" title=\"%s\">F1 %s</a>",ICEB_PUT_NA_HTML,__FUNCTION__,gettext("Просмотр документации"),gettext("Помощь"));
data.label_link=gtk_label_new(strsql);
gtk_label_set_use_markup (GTK_LABEL (data.label_link), TRUE);
g_signal_connect(data.label_link, "activate-current-link", G_CALLBACK (iceb_activate_link), NULL);

data.knopka[FK1]=gtk_button_new();
gtk_container_add (GTK_CONTAINER (data.knopka[FK1]), data.label_link);
g_signal_connect(data.knopka[FK1],"clicked",G_CALLBACK(poiprov_v_knopka),&data);
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK1], TRUE, TRUE, 0);


sprintf(strsql,"F2 %s",gettext("Поиск"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Начать поиск нужных записей"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(poiprov_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE, 0);

sprintf(strsql,"F4 %s",gettext("Очистить"));
data.knopka[FK4]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Очистить меню от введенной информации"));
g_signal_connect(data.knopka[FK4],"clicked",G_CALLBACK(poiprov_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK4], TRUE, TRUE, 0);

sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(poiprov_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE, 0);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);

gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));


gtk_main();

return(data.voz);
}

/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  m_poiprov_v_e_knopka(GtkWidget *widget,class poiprov_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case E_DATAN:

    if(iceb_calendar(&data->rk->datan,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAN]),data->rk->datan.ravno());
      
    return;  

  case E_DATANZ:

    if(iceb_calendar(&data->rk->datanz,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATANZ]),data->rk->datanz.ravno());
      
    return;  

  case E_DATAK:

    if(iceb_calendar(&data->rk->datak,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAK]),data->rk->datak.ravno());
      
    return;  


  case E_DATAKZ:

    if(iceb_calendar(&data->rk->datakz,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATAKZ]),data->rk->datakz.ravno());
      
    return;  

  case E_SHET:
    iceb_vibrek(0,"Plansh",&data->rk->shet,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHET]),data->rk->shet.ravno());
    return;

  case E_SHETK:
    iceb_vibrek(0,"Plansh",&data->rk->shetk,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_SHETK]),data->rk->shetk.ravno());
    return;

  case E_KONTR:
    iceb_vibrek(0,"Kontragent",&data->rk->kontr,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk->kontr.ravno());
    return;

  case E_GRUPAK:
    iceb_vibrek(0,"Gkont",&data->rk->grupak,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_GRUPAK]),data->rk->grupak.ravno());
    return;

  case E_METKA:
    poiprov_rek_metka(data);
    return;

 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   poiprov_v_key_press(GtkWidget *widget,GdkEventKey *event,class poiprov_data *data)
{

switch(event->keyval)
 {

  case GDK_KEY_F1:
    g_signal_emit_by_name(data->knopka[FK1],"clicked");
    return(TRUE);

  case GDK_KEY_F2:
    g_signal_emit_by_name(data->knopka[FK2],"clicked");
    return(TRUE);


  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(TRUE);

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
void  poiprov_v_knopka(GtkWidget *widget,class poiprov_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));

switch (knop)
 {
  case FK1:
    g_signal_emit_by_name(data->label_link,"activate-current-link");
    return;

  case FK2:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->activate();
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK4:
    data->clear_zero();
    return;  

  case FK10:
    iceb_sizww(data->name_window.ravno(),data->window);
    data->voz=1;
    gtk_widget_destroy(data->window);
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    poiprov_v_vvod(GtkWidget *widget,class poiprov_data *data)
{
int enter=atoi(gtk_widget_get_name(widget));

switch (enter)
 {
  case E_SHET:
    data->rk->shet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHETK:
    data->rk->shetk.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rk->kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KOMENT:
    data->rk->koment.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DEBET:
    data->rk->debet.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KREDIT:
    data->rk->kredit.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KEKV:
    data->rk->kekv.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAN:
    data->rk->datan.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  case E_DATANZ:
    data->rk->datanz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAK:
    data->rk->datak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DATAKZ:
    data->rk->datakz.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_METKA:
    data->rk->metka.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOK:
    data->rk->nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KODOP:
    data->rk->kodop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_GRUPAK:
    data->rk->grupak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}

/**********************************/
/*Меню выбора меток проводок      */
/**********************************/
void      poiprov_rek_metka(class poiprov_data *data)
{
char bros[512];

iceb_u_str titl;
iceb_u_str zagolovok;
iceb_u_spisok punkt_m;

titl.plus(gettext("Реквизиты"));
zagolovok.plus(gettext("Выберите нужное"));

sprintf(bros,"%s %s",ICEB_MP_MATU,gettext("Материальный учет"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",ICEB_MP_ZARP,gettext("Зароботная плата"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",ICEB_MP_UOS,gettext("Учет основных средств"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",ICEB_MP_PPOR,gettext("Платежные поручения"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",ICEB_MP_PTRE,gettext("Платежные требования"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",ICEB_MP_USLUGI,gettext("Учет услуг"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",ICEB_MP_KASA,gettext("Учет кассовых ордеров"));
punkt_m.plus(bros);

sprintf(bros,"%s %s",ICEB_MP_UKR,gettext("Учет командировочных расходов"));
punkt_m.plus(bros);

int nomer=iceb_menu_mv(&titl,&zagolovok,&punkt_m,0,data->window);

switch(nomer)
 {
  case -1:
   return;

  case 0:
   sprintf(bros,"%s",ICEB_MP_MATU);
   break;

  case 1:
   sprintf(bros,"%s",ICEB_MP_ZARP);
   break;

  case 2:
   sprintf(bros,"%s",ICEB_MP_UOS);
   break;

  case 3:
   sprintf(bros,"%s",ICEB_MP_PPOR);
   break;

  case 4:
   sprintf(bros,"%s",ICEB_MP_PTRE);
   break;

  case 5:
   sprintf(bros,"%s",ICEB_MP_USLUGI);
   break;

  case 6:
   sprintf(bros,"%s",ICEB_MP_KASA);
   break;

  case 7:
   sprintf(bros,"%s",ICEB_MP_UKR);
   break;
   
 }

if(data->rk->metka.getdlinna() > 1)
  data->rk->metka.plus(",");
data->rk->metka.plus(bros);

gtk_entry_set_text(GTK_ENTRY(data->entry[E_METKA]),data->rk->metka.ravno());

}
