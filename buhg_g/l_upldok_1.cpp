/*$Id: l_upldok_1.c,v 1.3 2013/08/25 08:26:42 sasa Exp $*/
/*13.07.2016	11.03.2008	Белых А.И.	l_upldok_1.c
Работа с документом в подсистеме "Учёт путевых листов"
*/
#include "buhg_g.h"

enum
{
 FK2,
 SFK2,
 FK3,
 FK4,
 FK6,
 FK7,
 FK8,
 FK9,
 FK10,
 KOL_F_KL
};

enum
{
 COL_KOD_TOP,
 COL_KOLIH,
 COL_TIPZ,
 COL_KOMENT,
 COL_DATA_VREM,
 COL_KTO,  
 COL_NOMZAP_V_DOK,
 COL_NOMZAP,
 NUM_COLUMNS
};

class  upldok_1_data
 {
  public:
  GtkWidget *label_rek_dok;
  GtkWidget *sw;
  GtkWidget *treeview;
  GtkWidget *window;
  GtkWidget *knopka[KOL_F_KL];
  short     kl_shift; //0-отжата 1-нажата  
  int       snanomer;   //номер записи на которую надостать 
  int       kolzap;     //Количество записей
  int voz;
  
  //реквизиты документа с которым работаем
  class iceb_u_str data_dok; /*Дата документа в символьном виде*/
  class iceb_u_str nomd;     /*Номер документа*/
  class iceb_u_str kod_pod;  /*код подразделения*/
    
  class iceb_u_str kod_vod;
  class iceb_u_str kod_avt;
  class iceb_u_str nz;  /*Номер документа в один и тотже день*/
  
  class iceb_u_str nomzap_v_dok_v;
  class iceb_u_str nomzap_v_dok_tv;
  class iceb_u_str nomzap_v;  
  double zat_top_f; /*Затраты топлива фактические*/    
  /*заголовок меню*/
  class iceb_u_str hapka;
  class iceb_u_spisok kod_top_ost;    /*Список кодов топлива на остатке*/
  class iceb_u_double kolih_top_ost;  /*Список количества топлива на остатке*/
  class iceb_u_spisok kod_top_pol;    /*Список кодов топлива плученного*/
  class iceb_u_double kolih_top_pol;  /*Список количества топлива полученного*/
  double kolih_ost; /*Количество топлива на остатке на начало документа*/
  short tipz;  
  class iceb_u_str imaf_prot;
  //Конструктор
  upldok_1_data()
   {
    tipz=0;
    voz=0;
    snanomer=0;
    kl_shift=0;
    window=treeview=NULL;
    kolih_ost=0.;
    nomzap_v_dok_tv.plus("");
   }      

 };

gboolean   upldok_1_key_press(GtkWidget *widget,GdkEventKey *event,class upldok_1_data *data);
void upldok_1_vibor(GtkTreeSelection *selection,class upldok_1_data *data);
void upldok_1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,class upldok_1_data *data);
void  upldok_1_knopka(GtkWidget *widget,class upldok_1_data *data);
void upldok_1_add_columns (GtkTreeView *treeview);
void upldok_1_create_list(class upldok_1_data *data);

int l_upldok_1_read_rhd(class  upldok_1_data *data,GtkWidget *wpredok);
int l_upldok_1_vn(short dd,short md,short gd,char *nomdok,GtkWidget *wpredok);
int l_upldok_1_uz(class upldok_1_data *data);
int l_upldok_1_ud(class upldok_1_data *data);
void l_prov_ukr(char *data_dok,char *nomdok,GtkWidget *wpredok);
void ukrku(short gd,char *nomd,GtkWidget *wpredok);
int l_upldok_vzp(const char *data_dok,int kod_pod,const char *nomdok,int nomzap,int metka_r, GtkWidget *wpredok);
void l_upldok_1_uplast(short dd,short md,short gd,int nom_zap,const char *nomd,const char *podr,const char *kodavt,const char *kodvod,double sumasp,class iceb_u_spisok *KTC,class iceb_u_double *OST,class iceb_u_spisok *KTCPL,class iceb_u_double *OSTPL,GtkWidget *wpredok);
void l_upldok_1_potspv(short dd,short md,short gd,const char *podr,const char *nomd,const char *kodavt,const char *kodvod,int nom_zap,GtkWidget *wpredok);
int l_upldok_vzr(const char *data_dok,int kod_pod,const char *nomdok,int nomzap,int metka_r,GtkWidget *wpredok);
int l_upldok2a(const char *data_dok,const char *nom_dok,int kod_podr,GtkWidget *wpredok);
double uplztfw(const char *datadok,int podr,const char *nom_dok,GtkWidget *wpredok);

extern SQL_baza	bd;
extern short startgodupl; //Стартовый год для "Учета путевых листов"

int l_upldok_1(const char *data_dok, //Дата документа
const char *nomd, //Номер документа
const char *kod_pod, /*код подразделения*/
GtkWidget *wpredok)
{
class  upldok_1_data data;
iceb_u_str string;
char bros[512];
sprintf(bros,"ostvaw%d.lst",getpid());
data.imaf_prot.plus(bros);
data.data_dok.new_plus(data_dok);
data.nomd.plus(nomd);
data.kod_pod.plus(kod_pod);


/*Читаем реквизиты документа*/
l_upldok_1_read_rhd(&data,wpredok);

/*Читаем после чтения реквизитов документов*/
class iceb_fopen fil;
fil.start(data.imaf_prot.ravno(),"w",wpredok);

data.kolih_ost=ostvaw(data.data_dok.ravno(),data.kod_vod.ravno(),data.kod_avt.ravno(),data.nz.ravno_atoi(),&data.kod_top_ost,&data.kolih_top_ost,fil.ff,wpredok);
fil.end();

data.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);


sprintf(bros,"%s %s",iceb_get_namesystem(),gettext("Работа с документом"));

gtk_window_set_title (GTK_WINDOW (data.window),bros);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(upldok_1_key_press),&data);
g_signal_connect_after(data.window,"key_release_event",G_CALLBACK(iceb_key_release),&data.kl_shift);

GtkWidget *hbox = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox_hap = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 1);
gtk_box_set_homogeneous (GTK_BOX(hbox_hap),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_container_add (GTK_CONTAINER (data.window), hbox);

GtkWidget *vbox1 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox1),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *vbox2 = gtk_box_new (GTK_ORIENTATION_VERTICAL, 1);
gtk_box_set_homogeneous (GTK_BOX(vbox2),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет

gtk_box_pack_start (GTK_BOX (hbox), vbox1, FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE,1);
gtk_widget_show(hbox);


data.label_rek_dok=gtk_label_new (data.hapka.ravno());
//gtk_box_pack_start (GTK_BOX (vbox2),data.label_rek_dok,FALSE, FALSE,1);

gtk_box_pack_start (GTK_BOX (vbox2),hbox_hap,FALSE, FALSE,1);
gtk_box_pack_start (GTK_BOX (hbox_hap),data.label_rek_dok,FALSE, FALSE,1);

class iceb_u_str spis_top_ost;
spis_top_ost.plus(gettext("Остаток топлива"));
spis_top_ost.plus(":\n");

for(int ii=0; ii < data.kod_top_ost.kolih(); ii++)
 {
  sprintf(bros,"%6s:%10.3f\n",data.kod_top_ost.ravno(ii),data.kolih_top_ost.ravno(ii));
  spis_top_ost.plus(bros);
 }
GtkWidget *label=NULL;
label=gtk_label_new(spis_top_ost.ravno());

gtk_box_pack_end (GTK_BOX (hbox_hap),label,FALSE, FALSE,1);

gtk_widget_show_all(hbox_hap);


gtk_widget_show(vbox1);
gtk_widget_show(vbox2);

data.sw = gtk_scrolled_window_new (NULL, NULL);

gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (data.sw),GTK_SHADOW_ETCHED_IN);
gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (data.sw),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);
//gtk_box_pack_start (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);
gtk_box_pack_end (GTK_BOX (vbox2), data.sw, TRUE, TRUE,1);

//Кнопки

sprintf(bros,"F2 %s",gettext("Ввести"));
data.knopka[FK2]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK2], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK2],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Ввод новой записи на получение топлива"));
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_widget_show(data.knopka[FK2]);

sprintf(bros,"%sF2 %s",RFK,gettext("Корректировать"));
data.knopka[SFK2]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[SFK2], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[SFK2],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[SFK2],gettext("Корректировка выбранной записи"));
gtk_widget_set_name(data.knopka[SFK2],iceb_u_inttochar(SFK2));
gtk_widget_show(data.knopka[SFK2]);

sprintf(bros,"F3 %s",gettext("Удалить"));
data.knopka[FK3]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK3], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK3],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Удалить выбранную запись"));
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_widget_show(data.knopka[FK3]);

sprintf(bros,"F4 %s",gettext("Счета"));
data.knopka[FK4]=gtk_button_new_with_label(bros);
g_signal_connect(data.knopka[FK4], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK4],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK4],gettext("Переход в режим списания топлива по счетам"));
gtk_widget_set_name(data.knopka[FK4],iceb_u_inttochar(FK4));
gtk_widget_show(data.knopka[FK4]);

sprintf(bros,"F6 %s",gettext("Списание"));
data.knopka[FK6]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK6],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK6], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK6],gettext("Автоматическое списание топлива"));
gtk_widget_set_name(data.knopka[FK6],iceb_u_inttochar(FK6));
gtk_widget_show(data.knopka[FK6]);

sprintf(bros,"F7 %s",gettext("Перенос"));
data.knopka[FK7]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK7],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK7], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK7],gettext("Перенос остатка топлива с предыдушего водителя"));
gtk_widget_set_name(data.knopka[FK7],iceb_u_inttochar(FK7));
gtk_widget_show(data.knopka[FK7]);

sprintf(bros,"F8 %s",gettext("Списание"));
data.knopka[FK8]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK8],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK8], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK8],gettext("Ввод записи на списание топлива"));
gtk_widget_set_name(data.knopka[FK8],iceb_u_inttochar(FK8));
gtk_widget_show(data.knopka[FK8]);

sprintf(bros,"F9 %s",gettext("Протокол"));
data.knopka[FK9]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1), data.knopka[FK9],TRUE,TRUE,1);
g_signal_connect(data.knopka[FK9], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_widget_set_tooltip_text(data.knopka[FK9],gettext("Просмотр протокола расчёта остатка топлива"));
gtk_widget_set_name(data.knopka[FK9],iceb_u_inttochar(FK9));
gtk_widget_show(data.knopka[FK9]);

sprintf(bros,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(bros);
gtk_box_pack_start(GTK_BOX(vbox1),data.knopka[FK10],TRUE,TRUE,1);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10], "clicked",G_CALLBACK(upldok_1_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_widget_show(data.knopka[FK10]);

 
gtk_widget_realize(data.window);
gdk_window_set_cursor(gtk_widget_get_window(data.window),gdk_cursor_new_for_display(gtk_widget_get_display(data.window),ICEB_CURSOR));

gtk_widget_grab_focus(data.knopka[FK10]);

upldok_1_create_list(&data);
gtk_widget_show(data.window);

gtk_window_maximize(GTK_WINDOW(data.window));


gtk_main();


if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);
}
/*****************************/
/*Обработчик нажатия кнопок  */
/*****************************/
void  upldok_1_knopka(GtkWidget *widget,class upldok_1_data *data)
{
class spis_oth oth;
int knop=atoi(gtk_widget_get_name(widget));

data->kl_shift=0; //Сбрасываем нажатый шифт так как после запуска нового меню он не сбрасывается
short dd=0,md=0,gd=0;
iceb_u_rsdat(&dd,&md,&gd,data->data_dok.ravno(),1);

switch (knop)
 {
  case FK2:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_upldok_vzp(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),0,0,data->window) == 0)
      upldok_1_create_list(data);
    return;  

  case SFK2:
    if(data->kolzap == 0)
     return;

    if(data->tipz == 2) /*Расход*/
     if(l_upldok_vzr(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),data->nomzap_v_dok_tv.ravno_atoi(),1,data->window) == 0)
       upldok_1_create_list(data);

    if(data->tipz == 1) /*приход*/
     if(l_upldok_vzp(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),data->nomzap_v_dok_tv.ravno_atoi(),1,data->window) == 0)
       upldok_1_create_list(data);

    return;  

    return;  

  case FK3:
    if(data->kolzap == 0)
     return;
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    if(l_upldok_1_uz(data) == 0)
      upldok_1_create_list(data);

    return;  


  case FK4:
    if(data->kolzap == 0)
     return;
    l_upldok2a(data->data_dok.ravno(),data->nomd.ravno(),data->kod_pod.ravno_atoi(),data->window);
    return;  

  case FK6:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    l_upldok_1_uplast(dd,md,gd,data->nz.ravno_atoi(),data->nomd.ravno(),data->kod_pod.ravno(),data->kod_avt.ravno(),
    data->kod_vod.ravno(),data->zat_top_f,&data->kod_top_ost,&data->kolih_top_ost,
    &data->kod_top_pol,&data->kolih_top_pol,data->window);

    upldok_1_create_list(data);

    return;

  case FK7:
    if(l_upldok_pb(data->data_dok.ravno(),data->window) != 0)
     return;

    l_upldok_1_potspv(dd,md,gd,data->kod_pod.ravno(),data->nomd.ravno(),data->kod_avt.ravno(),data->kod_vod.ravno(),data->nz.ravno_atoi(),data->window);
    upldok_1_create_list(data);

    return;

  case FK8:
    if(l_upldok_vzr(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),0,0,data->window) == 0)
     upldok_1_create_list(data);
    return;

  case FK9:
    oth.spis_imaf.plus(data->imaf_prot.ravno());
    oth.spis_ms.plus(data->imaf_prot.ravno());
    oth.spis_naim.plus(gettext("Протокол расчёта остатка топлива"));
    iceb_rabfil(&oth,data->window);
    return;


  case FK10:
    unlink(data->imaf_prot.ravno());
    gtk_widget_destroy(data->window);
    return;


 }
}

/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   upldok_1_key_press(GtkWidget *widget,GdkEventKey *event,class upldok_1_data *data)
{

switch(event->keyval)
 {


  case GDK_KEY_plus: //это почемуто не работает Нажата клавиша плюс на основной клавиатуре
  case GDK_KEY_KP_Add: //Нажата клавиша "+" на дополнительной клавиатуре
  case GDK_KEY_F2:
    if(data->kl_shift == 0)
      g_signal_emit_by_name(data->knopka[FK2],"clicked");
    if(data->kl_shift == 1)
      g_signal_emit_by_name(data->knopka[SFK2],"clicked");
    return(TRUE);

  case GDK_KEY_minus: //Нажата клавиша минус на основной клавиатуре
  case GDK_KEY_KP_Subtract: //Нажата клавиша "-" на дополнительной клавиатуре
    return(TRUE);
  
  case GDK_KEY_F3:
    g_signal_emit_by_name(data->knopka[FK3],"clicked");
    return(TRUE);

  case GDK_KEY_F4:
    g_signal_emit_by_name(data->knopka[FK4],"clicked");
    return(TRUE);

  case GDK_KEY_F6:
    g_signal_emit_by_name(data->knopka[FK6],"clicked");
    return(TRUE);

  case GDK_KEY_F7:
    g_signal_emit_by_name(data->knopka[FK7],"clicked");
    return(TRUE);

  case GDK_KEY_F8:
    g_signal_emit_by_name(data->knopka[FK8],"clicked");
    return(TRUE);

  case GDK_KEY_F9:
    g_signal_emit_by_name(data->knopka[FK9],"clicked");
    return(TRUE);


  case ICEB_REG_L:
  case ICEB_REG_R:
    printf("upldok_1_key_press-Нажата клавиша Shift\n");

    data->kl_shift=1;

    return(TRUE);

  case GDK_KEY_Escape:
  case GDK_KEY_F10:
    g_signal_emit_by_name(data->knopka[FK10],"clicked");
    return(FALSE);

  default:
    printf("Не выбрана клавиша ! Код=%d\n",event->keyval);

    break;
 }

return(TRUE);
}
/*****************/
/*Создаем колонки*/
/*****************/

void upldok_1_add_columns(class upldok_1_data *data)
{
GtkCellRenderer *renderer;

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
"M", renderer,"text", COL_TIPZ,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Код топлива"), renderer,"text", COL_KOD_TOP,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Количество"), renderer,"text", COL_KOLIH,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Коментар"), renderer,"text", COL_KOMENT,NULL);



renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Дата и время записи"), renderer,
"text", COL_DATA_VREM,NULL);

renderer = gtk_cell_renderer_text_new ();
gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW (data->treeview),-1,
gettext("Кто записал"), renderer,
"text", COL_KTO,NULL);


}
/***********************************/
/*Создаем список для просмотра */
/***********************************/
void upldok_1_create_list (class upldok_1_data *data)
{
class iceb_gdite_data gdite;
iceb_gdite(&gdite,0,data->window);

GtkListStore *model=NULL;
GtkTreeIter iter;
SQLCURSOR cur;
SQLCURSOR cur1;
char strsql[512];
int  kolstr=0;
SQL_str row;

data->kl_shift=0; //0-отжата 1-нажата  

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR_GDITE));
iceb_refresh();

if(data->treeview != NULL)
  gtk_widget_destroy(data->treeview);

data->treeview = gtk_tree_view_new();


gtk_container_add (GTK_CONTAINER (data->sw), data->treeview);

g_signal_connect(data->treeview,"row_activated",G_CALLBACK(upldok_1_v_row),data);

GtkTreeSelection *selection=gtk_tree_view_get_selection(GTK_TREE_VIEW(data->treeview));
gtk_tree_selection_set_mode(selection,GTK_SELECTION_SINGLE);
g_signal_connect(selection,"changed",G_CALLBACK(upldok_1_vibor),data);

gtk_tree_selection_set_mode (gtk_tree_view_get_selection (GTK_TREE_VIEW (data->treeview)),GTK_SELECTION_SINGLE);


model = gtk_list_store_new (NUM_COLUMNS+1, 
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_STRING, 
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_STRING, 
G_TYPE_STRING,
G_TYPE_STRING,
G_TYPE_INT);

sprintf(strsql,"select nzap,tz,kodtp,kolih,kom,ktoz,vrem from Upldok1 where datd='%s' and kp=%d and nomd='%s' order by tz,kodtp asc",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

/*printf("strsql=%s\n",strsql);*/

if((kolstr=cur.make_cursor(&bd,strsql)) < 0)
 {
  iceb_msql_error(&bd,gettext("Ошибка создания курсора !"),strsql,data->window);
  return;
 }
//gtk_list_store_clear(model);

class iceb_u_str ss;

data->kolzap=0;

float kolstr1=0.;
double itogo_sp=0.;
double ost=data->kolih_ost;
double kolih=0.;
data->kod_top_pol.free_class();
data->kolih_top_pol.free_class();
int nomer_top=0;
while(cur.read_cursor(&row) != 0)
 {
/*  printf("%s %s %s %s %s %s %s\n",row[0],row[1],row[2],row[3],row[4],row[5],row[6]);*/
  
  iceb_pbar(gdite.bar,kolstr,++kolstr1);
  
  //Количество  
  kolih=atof(row[3]);

  //Тип записи
  if(row[1][0] == '1')
   {
    ss.new_plus("+");
    if((nomer_top=data->kod_top_pol.find(row[2])) < 0)
     data->kod_top_pol.plus(row[2]);
    data->kolih_top_pol.plus(kolih,nomer_top);    
   }
  else
   {
    ss.new_plus("-");
    itogo_sp+=atof(row[3]);
    kolih*=-1;
   }  
  
  ost+=kolih;

  gtk_list_store_append (model, &iter);

  gtk_list_store_set (model, &iter,
  COL_KOD_TOP,row[2],
  COL_KOLIH,row[3],
  COL_TIPZ,ss.ravno(),
  COL_KOMENT,row[4],
  COL_DATA_VREM,iceb_u_vremzap(row[6]),
  COL_KTO,iceb_kszap(row[5],data->window),
  COL_NOMZAP_V_DOK,row[0],
  NUM_COLUMNS,data->kolzap,
  -1);

  data->kolzap++;
 }
data->nomzap_v_dok_tv.new_plus("");

gtk_tree_view_set_model (GTK_TREE_VIEW(data-> treeview),GTK_TREE_MODEL (model));

g_object_unref (GTK_TREE_MODEL (model));

upldok_1_add_columns (data);

if(data->kolzap == 0)
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),FALSE);//Недоступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),FALSE);//Недоступна
 }
else
 {
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[SFK2]),TRUE);//Доступна
  gtk_widget_set_sensitive(GTK_WIDGET(data->knopka[FK3]),TRUE);//Доступна
 }
gtk_widget_show (data->treeview);
gtk_widget_show (data->sw);

iceb_snanomer(data->kolzap,&data->snanomer,data->treeview);

class iceb_u_str zagol;
zagol.plus(data->hapka.ravno());

if(data->kolzap > 0)
 {
  sprintf(strsql,"%s:%d ",
  gettext("Количество записей"),
  data->kolzap);

  zagol.ps_plus(strsql);
  
  sprintf(strsql,"%s:%.10g ",
  gettext("Списано топлива"),
  itogo_sp);

  zagol.plus(strsql);

  sprintf(strsql,"%s:%.10g",
  gettext("Остаток"),
  ost);
    
  zagol.plus(strsql);

 }
 
gtk_label_set_text(GTK_LABEL(data->label_rek_dok),zagol.ravno());

gdk_window_set_cursor(gtk_widget_get_window(data->window),gdk_cursor_new_for_display(gtk_widget_get_display(data->window),ICEB_CURSOR));
}
/****************************/
/*Чтение реквизитов стороки на которую установлена подсветка строки*/
/**********************/

void upldok_1_vibor(GtkTreeSelection *selection,class upldok_1_data *data)
{
GtkTreeModel *model;
GtkTreeIter  iter;


if(gtk_tree_selection_get_selected(selection,&model,&iter) != TRUE)
 return;

gchar *nomzap_v_dok;
gchar *tipz;
gint  nomer;

gtk_tree_model_get(model,&iter,
COL_NOMZAP_V_DOK,&nomzap_v_dok,
COL_TIPZ,&tipz,
NUM_COLUMNS,&nomer,-1);

data->nomzap_v_dok_v.new_plus(nomzap_v_dok);
if(tipz[0] == '+')
 data->tipz=1;
else
 data->tipz=2;
data->snanomer=nomer;

g_free(nomzap_v_dok);
g_free(tipz);

}
/****************************/
/*Выбор строки по двойному клику мышкой*/
/**********************/
void upldok_1_v_row(GtkTreeView *treeview,GtkTreePath *arg1,GtkTreeViewColumn *arg2,
class upldok_1_data *data)
{
//Корректировка записи
g_signal_emit_by_name(data->knopka[SFK2],"clicked");


}

/******************************/
/*Чтение реквизитов шапки документа*/
/************************************/
int l_upldok_1_read_rhd(class  upldok_1_data *data,GtkWidget *wpredok)
{
char strsql[512];
SQL_str row;
class SQLCURSOR cur;

sprintf(strsql,"select ka,kv,nz from Upldok where god=%d and kp=%d and \
nomd='%s'",data->data_dok.ravno_god(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
 {
  
  sprintf(strsql,"%s %s %s !",
  gettext("Не найден документ"),
  data->nomd.ravno(),
  data->data_dok.ravno());
  
  iceb_menu_soob(strsql,wpredok);
  return(1);
 }

data->kod_vod.new_plus(row[1]);
data->kod_avt.new_plus(row[0]);
data->nz.new_plus(row[2]);

//data->zat_top_f=atof(row[10])+atof(row[23])+atof(row[28])+atof(row[31]);
data->zat_top_f=uplztfw(data->data_dok.ravno(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),wpredok);

class iceb_u_str naim_pod("");
sprintf(strsql,"select naik from Uplpodr where kod=%d",data->kod_pod.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_pod.new_plus(row[0]);

class iceb_u_str naim_vod("");
sprintf(strsql,"select naik from Uplouot where kod=%d",data->kod_vod.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_vod.new_plus(row[0]);

class iceb_u_str naim_avt("");
sprintf(strsql,"select naik from Uplavt where kod=%d",data->kod_avt.ravno_atoi());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
 naim_avt.new_plus(row[0]);

sprintf(strsql,"%s %s %s:%s\n",
gettext("Дата документа"),
data->data_dok.ravno(),
gettext("Номер"), 
data->nomd.ravno());

data->hapka.new_plus(strsql);

sprintf(strsql,"%s:%s/%s\n",
gettext("Подразделение"),
data->kod_pod.ravno(),naim_pod.ravno());

data->hapka.plus(strsql);


sprintf(strsql,"%s:%d/%s\n",
gettext("Водитель"),
data->kod_vod.ravno_atoi(),naim_vod.ravno());

data->hapka.plus(strsql);

sprintf(strsql,"%s:%d/%s\n",
gettext("Автомобиль"),
data->kod_avt.ravno_atoi(),naim_avt.ravno());

data->hapka.plus(strsql);

sprintf(strsql,"%s:%.10g",
gettext("Затраты топлива"),
data->zat_top_f);

data->hapka.plus(strsql);


return(0);
}
/***********************/
/*Удаление записи в документе*/
/****************************/
int l_upldok_1_uz(class upldok_1_data *data)
{

if(iceb_menu_danet(gettext("Удалить запись ? Вы уверены ?"),2,data->window) == 2)
   return(1);    

char strsql[512];

sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%d and nomd='%s' and nzap=%d",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno(),data->nomzap_v_dok_v.ravno_atoi());

iceb_sql_zapis(strsql,1,0,data->window);

return(0);
}
/***************************/
/*Удаление всего документа*/
/**************************/
int l_upldok_1_ud(class upldok_1_data *data)
{


if(iceb_menu_danet(gettext("Удалить документ ? Вы уверены ?"),2,data->window) == 2)
  return(1);    

char strsql[512];

sprintf(strsql,"delete from Upldok2a where datd='%s' and kp=%d and nomd='%s'",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Upldok1 where datd='%s' and kp=%d and nomd='%s'",
data->data_dok.ravno_sqldata(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

sprintf(strsql,"delete from Upldok where god=%d and kp=%d and nomd='%s'",
data->data_dok.ravno_god(),data->kod_pod.ravno_atoi(),data->nomd.ravno());

if(iceb_sql_zapis(strsql,0,0,data->window) != 0)
  return(1);

 

return(0);
}
/*******************************/
/*Запись расходов из массива   */
/*******************************/

double	l_upldok_1_l_upldok_1_uplast1(short dd,short md,short gd, //Дата
int nom_zap, //порядковый номер документа в один и тотже день
const char *nomd, //Номер документа
const char *podr, //Подразделение
const char *kodavt, //Код автомобиля
const char *kodvod, //Код водителя
double sumasp, //Сумма которую надо списать
class iceb_u_spisok *KTC,class iceb_u_double *OST,
GtkWidget *wpredok)
{
double          kolih=0.;
double		kolihz=0.;
time_t		vrem;
class iceb_u_str kodtp("");

printf("%s\n",__FUNCTION__);

time(&vrem);

int kolzap=KTC->kolih();
int i;


for(i=0; i<kolzap;i++)
 {
  if(sumasp < 0.0009)
   return(0.);

  kodtp.new_plus(KTC->ravno(i));

  kolih=OST->ravno(i);
  
  if(kolih > sumasp)
   {
    kolihz=sumasp;
    sumasp=0.;
   }
  else
   {
    kolihz=kolih;
    sumasp-=kolih;
   }
//  printw("kolihz=%f sumasp=%f\n",kolihz,sumasp);
//OSTANOV();


  if(kolihz > 0.0009)
   {
    upl_zapvdokw(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),0,2,kodtp.ravno(),kolihz,
    "",0,nom_zap,wpredok);
  }
 }

return(sumasp);

}

/****************************************/
/*Автоматическое списание топлива       */
/****************************************/

void	l_upldok_1_uplast(short dd,short md,short gd, //Дата
int nom_zap, //порядковый номер документа в один и тотже день
const char *nomd, //Номер документа
const char *podr, //Подразделение
const char *kodavt, //Код автомобиля
const char *kodvod, //Код водителя
double sumasp, //Сумма которую надо списать
class iceb_u_spisok *KTC,class iceb_u_double *OST,
class iceb_u_spisok *KTCPL,class iceb_u_double *OSTPL,
GtkWidget *wpredok)
{
char		strsql[512];
double   spisano=0;

//Удаляем все расходы в документе
sprintf(strsql,"delete from Upldok1 where datd='%d-%d-%d' and kp=%d and nomd='%s' and tz=2",
gd,md,dd,atoi(podr),nomd);

if(iceb_sql_zapis(strsql,0,0,wpredok) != 0)
  return;

spisano=l_upldok_1_l_upldok_1_uplast1(dd,md,gd,nom_zap,nomd,podr,kodavt,kodvod,sumasp,KTC,OST,wpredok);
l_upldok_1_l_upldok_1_uplast1(dd,md,gd,nom_zap,nomd,podr,kodavt,kodvod,spisano,KTCPL,OSTPL,wpredok);

}

/*************************************************/
/*Перенос остатков топлива с предыдущего водителя*/
/*************************************************/

void l_upldok_1_potspv(short dd,short md,short gd,
const char *podr,
const char *nomd,
const char *kodavt,
const char *kodvod,
int nom_zap,
GtkWidget *wpredok)
{
char		strsql[512];
SQL_str		row;
SQLCURSOR curr;
class iceb_u_str kodvodp("");


//Определяем самый последний документ по данному автомобилю
if(nom_zap == 0)
 sprintf(strsql,"select kv from Upldok1 where \
datd < '%d-%d-%d' and ka=%s order by datd desc limit 1",
 gd,md,dd,kodavt);
else
 sprintf(strsql,"select kv from Upldok1 where \
datd = '%d-%d-%d' and ka=%s and nz < %d order by nz desc limit 1",
 gd,md,dd,kodavt,nom_zap);

if(iceb_sql_readkey(strsql,&row,&curr,wpredok) > 0)
 {
  kodvodp.new_plus(row[0]); 
 }
else
 {
  //Читаем может есть сальдо
  sprintf(strsql,"select kv from Uplsal where god=%d and ka=%s",startgodupl,kodavt);
  if(iceb_sql_readkey(strsql,&row,&curr,wpredok) == 1)
    kodvodp.new_plus(row[0]); 

 }
if(iceb_u_SRAV(kodvod,kodvodp.ravno(),0) == 0)
  return;
if(kodvodp.ravno()[0] == '\0')
  return;

//Создаем список остатков топлива
class iceb_u_spisok KTC;
class iceb_u_double OST;

ostvaw(dd,md,gd,kodvodp.ravno(),kodavt,nom_zap,&KTC,&OST,NULL,wpredok);

int kolsp=KTC.kolih();
if(kolsp == 0)
  return;
  
double kolih=0.;


for(int i=0; i < kolsp;i++)
 {


  kolih=OST.ravno(i);

  upl_zapvdokw(dd,md,gd,atoi(podr),nomd,atoi(kodavt),atoi(kodvod),kodvodp.ravno_atoi(),1,KTC.ravno(i),kolih,
  gettext("Перенос остатка"),atoi(kodavt),nom_zap,wpredok);

 }



}
