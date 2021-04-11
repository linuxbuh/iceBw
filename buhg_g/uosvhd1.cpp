/*$Id: uosvhd1.c,v 1.23 2013/09/26 09:46:57 sasa Exp $*/
/*13.07.2015	19.10.2007	Белых А.И.	uosvhd1.c
Ввод шапки документа для прихода (операция внешняя)
Если вернули 0-шапка записана
             1-шапка не записана
*/
#include "buhg_g.h"
#include "uosvhd.h"
enum
{
  FK2,
  FK3,
  FK10,
  KOL_F_KL
};

enum
 {
  /*Обязательные реквизиты для заполнения*/
  E_DATADOK,
  E_NOMDOK,
  E_PODR,
  E_MAT_OT,
  E_KONTR,
  /*Необязательные реквизиты для заполнения*/
  E_NOMDOK_POST,
  E_OSNOV,
  E_DOVER,
  E_DATA_VD,
  E_SHEREZ,
  E_NOMNALNAK,
  E_DATA_VNN,
  E_DATA_OPL,
  E_FOROP,
  E_DVNN,
  E_VID_DOG,
 /**********/
  E_NDS,
  KOLENTER  
 };

class uosvhd1_m_data
 {
  public:
    
  GtkWidget *entry[KOLENTER];
  GtkWidget *knopka[KOL_F_KL];
  GtkWidget *knopka_enter[KOLENTER];
  GtkWidget *opt;
  GtkWidget *window;
  GtkWidget *label_kontr;
  GtkWidget *label_podr;
  GtkWidget *label_kod_op;
  GtkWidget *label_forop;
  GtkWidget *label_mat_ot;
  short kl_shift;
  short voz;      //0-шапка записана расчёт 1 нет

  /*Реквизиты шапки документа*/
  class uosvhd_data rk;
  short god_nd; /*год нумерации документов*/
  uosvhd1_m_data() //Конструктор
   {
    kl_shift=0;
    voz=1;
    god_nd=0;
   }

  void read_rek()
   {
    rk.datadok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATADOK])));
    rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK])));
    rk.podr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_PODR])));
    rk.mat_ot.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_MAT_OT])));
    rk.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_KONTR])));
    rk.nomdok_post.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMDOK_POST])));
    rk.osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_OSNOV])));
    rk.dover.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DOVER])));
    rk.data_vd.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VD])));
    rk.sherez.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_SHEREZ])));
    rk.nomnalnak.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_NOMNALNAK])));
    rk.data_vnn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_VNN])));
    rk.data_opl.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DATA_OPL])));
    rk.forop.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_FOROP])));
    rk.dvnn.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_DVNN])));
    rk.vid_dog.new_plus(gtk_entry_get_text(GTK_ENTRY(entry[E_VID_DOG])));

    rk.lnds=gtk_combo_box_get_active (GTK_COMBO_BOX(opt));

   }

 };



gboolean   uosvhd1_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvhd1_m_data *data);
void    uosvhd1_m_v_vvod(GtkWidget *widget,class uosvhd1_m_data *data);
void  uosvhd1_m_v_knopka(GtkWidget *widget,class uosvhd1_m_data *data);
void   uosvhd1_m_rekviz(class uosvhd1_m_data *data);
void  uosvhd1_v_e_knopka(GtkWidget *widget,class uosvhd1_m_data *data);


extern SQL_baza bd;

int uosvhd1(class iceb_u_str *datadok,
class iceb_u_str *nomdok,
const char *kod_op, /*Код операции для нового документа*/
GtkWidget *wpredok)
{
SQL_str row;
class SQLCURSOR cur;
class uosvhd1_m_data data;
char strsql[512];
class iceb_u_str naim_kontr("");
class iceb_u_str naim_podr("");
class iceb_u_str naim_mat_ot("");
class iceb_u_str naim_kod_op("");
class iceb_u_str naim_for_opl("");

data.rk.datadok_z.new_plus(datadok->ravno());
data.rk.datadok.new_plus(datadok->ravno());
data.rk.nomdok.new_plus(nomdok->ravno());
data.rk.nomdok_z.new_plus(nomdok->ravno());
data.rk.kod_op.new_plus(kod_op);
data.rk.tipz=1;
data.god_nd=datadok->ravno_god();

if(data.rk.datadok.getdlinna() > 1)
 {
  sprintf(strsql,"select tipz,kodop,kontr,podr,kodol,nomdv,osnov,dover,datvd,forop,datpnn,nomnn,sherez,\
datop,nds,pn,dvnn,vd  from Uosdok where datd='%s' and nomd='%s'",data.rk.datadok.ravno_sqldata(),
  data.rk.nomdok.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) != 1)
   {
    iceb_u_str repl;
    repl.plus(gettext("Не найден документ"));
    repl.plus(" !!!");

    sprintf(strsql,"%s:%s %s:%s",
    gettext("Дата"),
    data.rk.datadok.ravno(),
    gettext("Номер документа"),
    data.rk.nomdok.ravno());
    
    repl.ps_plus(strsql);
    iceb_menu_soob(&repl,wpredok);  
    
    return(1);
   } 

  if(atoi(row[0]) != 1)
   {
    iceb_menu_soob("Это не приходный документ !",wpredok);
    return(1);
   }
  data.rk.kod_op.new_plus(row[1]);

  data.rk.kontr.new_plus(row[2]);
  data.rk.kontr_z.new_plus(row[2]);
  
  data.rk.podr.new_plus(row[3]);
  data.rk.podr_z.new_plus(row[3]);
  
  data.rk.mat_ot.new_plus(row[4]);
  data.rk.mat_ot_z.new_plus(row[4]);
  
  data.rk.nomdok_post.new_plus(row[5]);
  data.rk.nomdok_post_z.new_plus(row[5]);
  
  data.rk.osnov.new_plus(row[6]);
  data.rk.dover.new_plus(row[7]);
  data.rk.data_vd.new_plus_sqldata(row[8]);
  data.rk.forop.new_plus(row[9]);
  data.rk.data_vnn.new_plus_sqldata(row[10]);
  data.rk.nomnalnak.new_plus(row[11]);
  data.rk.sherez.new_plus(row[12]);
  data.rk.data_opl.new_plus_sqldata(row[13]);
  data.rk.lnds=atoi(row[14]);
  data.rk.pnds=atof(row[15]);  
  data.rk.dvnn.new_plus(iceb_u_datzap(row[16]));
  data.rk.vid_dog.new_plus(row[17]);  
  sprintf(strsql,"select naikon from Kontragent where kodkon='%s'",data.rk.kontr.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_kontr.new_plus(row[0]);

  sprintf(strsql,"select naik from Uospod where kod=%d",data.rk.podr.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
   naim_podr.new_plus(row[0]);

  sprintf(strsql,"select naik from Uosol where kod=%d",data.rk.mat_ot.ravno_atoi());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    naim_mat_ot.new_plus(row[0]);

  sprintf(strsql,"select naik from Foroplat where kod='%s'",data.rk.forop.ravno());
  if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
    naim_for_opl.new_plus(row[0]);

   
 }

sprintf(strsql,"select naik from Uospri where kod='%s'",data.rk.kod_op.ravno());
if(iceb_sql_readkey(strsql,&row,&cur,wpredok) == 1)
  naim_kod_op.new_plus(row[0]);


data.window=gtk_window_new(GTK_WINDOW_TOPLEVEL);
gtk_window_set_position( GTK_WINDOW(data.window),ICEB_POS_CENTER);
gtk_window_set_modal(GTK_WINDOW(data.window),TRUE);

sprintf(strsql,"%s %s",iceb_get_namesystem(),gettext("Работа с шапкой документа"));
gtk_window_set_title (GTK_WINDOW (data.window),strsql);
gtk_container_set_border_width (GTK_CONTAINER (data.window), 5);

g_signal_connect(data.window,"delete_event",G_CALLBACK(gtk_widget_destroy),NULL);
g_signal_connect(data.window,"destroy",G_CALLBACK(gtk_main_quit),NULL);

g_signal_connect_after(data.window,"key_press_event",G_CALLBACK(uosvhd1_m_v_key_press),&data);


if(wpredok != NULL)
 {
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR_GDITE));
  //Удерживать окно над породившем его окном всегда
  gtk_window_set_transient_for(GTK_WINDOW(data.window),GTK_WINDOW(wpredok));
  //Закрыть окно если окно предок удалено
  gtk_window_set_destroy_with_parent(GTK_WINDOW(data.window),TRUE);
 }




GtkWidget *vbox = gtk_box_new (GTK_ORIENTATION_VERTICAL, 0);
gtk_box_set_homogeneous (GTK_BOX(vbox),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
GtkWidget *hbox[KOLENTER];
for(int i=0; i < KOLENTER; i++)
 {
  hbox[i] = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(  hbox[i]),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
 }



gtk_container_add (GTK_CONTAINER (data.window), vbox);
//gtk_container_add (GTK_CONTAINER (vbox), label);


GtkWidget *label=NULL;
label=gtk_label_new(gettext("Приход"));
gtk_box_pack_start (GTK_BOX (vbox),label, FALSE, FALSE,1);

sprintf(strsql,"%s:%s %s (%s)",gettext("Операция"),
data.rk.kod_op.ravno(),naim_kod_op.ravno(),gettext("Внешняя"));

data.label_kod_op=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (vbox),data.label_kod_op, FALSE, FALSE,1);

for(int i=0; i < KOLENTER; i++)
  gtk_box_pack_start (GTK_BOX (vbox),hbox[i], FALSE, FALSE,1);
//  gtk_container_add (GTK_CONTAINER (vbox), hbox[i]);

//gtk_container_add (GTK_CONTAINER (vbox), hboxknop);

GtkWidget *hboxknop = gtk_box_new (GTK_ORIENTATION_HORIZONTAL, 0);
gtk_box_set_homogeneous (GTK_BOX(hboxknop),FALSE); //Устанавливает одинакоый ли размер будут иметь упакованные виджеты-TRUE-одинаковые FALSE-нет
gtk_box_pack_start (GTK_BOX (vbox),hboxknop, FALSE, FALSE,1);


sprintf(strsql,"%s (%s)",gettext("Дата"),gettext("д.м.г"));
data.knopka_enter[E_DATADOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATADOK]), data.knopka_enter[E_DATADOK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATADOK],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATADOK],iceb_u_inttochar(E_DATADOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATADOK],gettext("Выбор даты"));

data.entry[E_DATADOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATADOK]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATADOK]), data.entry[E_DATADOK], FALSE, FALSE,1);
g_signal_connect(data.entry[E_DATADOK], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATADOK]),data.rk.datadok.ravno());
gtk_widget_set_name(data.entry[E_DATADOK],iceb_u_inttochar(E_DATADOK));

sprintf(strsql,"%s",gettext("Номер документа"));
data.knopka_enter[E_NOMDOK]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.knopka_enter[E_NOMDOK], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_NOMDOK],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));
gtk_widget_set_tooltip_text(data.knopka_enter[E_NOMDOK],gettext("Получение номера документа"));

data.entry[E_NOMDOK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK]), data.entry[E_NOMDOK], FALSE, FALSE,1);
g_signal_connect(data.entry[E_NOMDOK], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK]),data.rk.nomdok.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK],iceb_u_inttochar(E_NOMDOK));


sprintf(strsql,"%s",gettext("Код подразделения"));
data.knopka_enter[E_PODR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.knopka_enter[E_PODR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_PODR],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_PODR],iceb_u_inttochar(E_PODR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_PODR],gettext("Выбор подразделения"));

data.entry[E_PODR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_PODR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.entry[E_PODR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_PODR], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_PODR]),data.rk.podr.ravno());
gtk_widget_set_name(data.entry[E_PODR],iceb_u_inttochar(E_PODR));

data.label_podr=gtk_label_new(naim_podr.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_PODR]), data.label_podr, TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Код материально-ответственного"));
data.knopka_enter[E_MAT_OT]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.knopka_enter[E_MAT_OT], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_MAT_OT],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));
gtk_widget_set_tooltip_text(data.knopka_enter[E_MAT_OT],gettext("Выбор материально-ответственного"));

data.entry[E_MAT_OT] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_MAT_OT]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.entry[E_MAT_OT], TRUE, TRUE,1);
g_signal_connect(data.entry[E_MAT_OT], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_MAT_OT]),data.rk.mat_ot.ravno());
gtk_widget_set_name(data.entry[E_MAT_OT],iceb_u_inttochar(E_MAT_OT));

data.label_mat_ot=gtk_label_new(naim_mat_ot.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_MAT_OT]), data.label_mat_ot, TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Код контрагента"));
data.knopka_enter[E_KONTR]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.knopka_enter[E_KONTR], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_KONTR],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_KONTR],iceb_u_inttochar(E_KONTR));
gtk_widget_set_tooltip_text(data.knopka_enter[E_KONTR],gettext("Выбор контрагента"));

data.entry[E_KONTR] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_KONTR]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.entry[E_KONTR], TRUE, TRUE,1);
g_signal_connect(data.entry[E_KONTR], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_KONTR]),data.rk.kontr.ravno());
gtk_widget_set_name(data.entry[E_KONTR],iceb_u_inttochar(E_KONTR));

data.label_kontr=gtk_label_new(naim_kontr.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_KONTR]), data.label_kontr, TRUE, TRUE,1);


sprintf(strsql,"%s",gettext("Номер документа поставщика"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_POST]), label, FALSE, FALSE, 0);

data.entry[E_NOMDOK_POST] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMDOK_POST]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMDOK_POST]), data.entry[E_NOMDOK_POST], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMDOK_POST], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMDOK_POST]),data.rk.nomdok_post.ravno());
gtk_widget_set_name(data.entry[E_NOMDOK_POST],iceb_u_inttochar(E_NOMDOK_POST));


sprintf(strsql,"%s",gettext("Основание"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), label, FALSE, FALSE, 0);

data.entry[E_OSNOV] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_OSNOV]),59);
gtk_box_pack_start (GTK_BOX (hbox[E_OSNOV]), data.entry[E_OSNOV], TRUE, TRUE,1);
g_signal_connect(data.entry[E_OSNOV], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_OSNOV]),data.rk.osnov.ravno());
gtk_widget_set_name(data.entry[E_OSNOV],iceb_u_inttochar(E_OSNOV));


sprintf(strsql,"%s",gettext("Доверенность"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), label, FALSE, FALSE, 0);

data.entry[E_DOVER] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DOVER]),29);
gtk_box_pack_start (GTK_BOX (hbox[E_DOVER]), data.entry[E_DOVER], TRUE, TRUE,1);
g_signal_connect(data.entry[E_DOVER], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DOVER]),data.rk.dover.ravno());
gtk_widget_set_name(data.entry[E_DOVER],iceb_u_inttochar(E_DOVER));


sprintf(strsql,"%s",gettext("Через кого"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_SHEREZ]), label, FALSE, FALSE, 0);

data.entry[E_SHEREZ] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_SHEREZ]),39);
gtk_box_pack_start (GTK_BOX (hbox[E_SHEREZ]), data.entry[E_SHEREZ], TRUE, TRUE,1);
g_signal_connect(data.entry[E_SHEREZ], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_SHEREZ]),data.rk.sherez.ravno());
gtk_widget_set_name(data.entry[E_SHEREZ],iceb_u_inttochar(E_SHEREZ));


sprintf(strsql,"%s (%s)",gettext("Дата видачи доверенности"),gettext("д.м.г"));
data.knopka_enter[E_DATA_VD]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VD]), data.knopka_enter[E_DATA_VD], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA_VD],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_VD],iceb_u_inttochar(E_DATA_VD));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_VD],gettext("Выбор даты"));

data.entry[E_DATA_VD] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_VD]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VD]), data.entry[E_DATA_VD], FALSE, FALSE,1);
g_signal_connect(data.entry[E_DATA_VD], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VD]),data.rk.data_vd.ravno());
gtk_widget_set_name(data.entry[E_DATA_VD],iceb_u_inttochar(E_DATA_VD));


sprintf(strsql,"%s",gettext("Номер налоговой накладной"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), label, FALSE, FALSE, 0);

data.entry[E_NOMNALNAK] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_NOMNALNAK]),19);
gtk_box_pack_start (GTK_BOX (hbox[E_NOMNALNAK]), data.entry[E_NOMNALNAK], TRUE, TRUE,1);
g_signal_connect(data.entry[E_NOMNALNAK], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_NOMNALNAK]),data.rk.nomnalnak.ravno());
gtk_widget_set_name(data.entry[E_NOMNALNAK],iceb_u_inttochar(E_NOMNALNAK));


sprintf(strsql,"%s (%s)",gettext("Дата получения налоговой накладной"),gettext("д.м.г"));
data.knopka_enter[E_DATA_VNN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VNN]), data.knopka_enter[E_DATA_VNN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA_VNN],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_VNN],iceb_u_inttochar(E_DATA_VNN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_VNN],gettext("Выбор даты"));

data.entry[E_DATA_VNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_VNN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_VNN]), data.entry[E_DATA_VNN], FALSE, FALSE,1);
g_signal_connect(data.entry[E_DATA_VNN], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_VNN]),data.rk.data_vnn.ravno());
gtk_widget_set_name(data.entry[E_DATA_VNN],iceb_u_inttochar(E_DATA_VNN));


sprintf(strsql,"%s (%s)",gettext("Дата оплаты"),gettext("д.м.г"));
data.knopka_enter[E_DATA_OPL]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OPL]), data.knopka_enter[E_DATA_OPL], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DATA_OPL],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DATA_OPL],iceb_u_inttochar(E_DATA_OPL));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DATA_OPL],gettext("Выбор даты"));

data.entry[E_DATA_OPL] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DATA_OPL]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DATA_OPL]), data.entry[E_DATA_OPL], FALSE, FALSE,1);
g_signal_connect(data.entry[E_DATA_OPL], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DATA_OPL]),data.rk.data_opl.ravno());
gtk_widget_set_name(data.entry[E_DATA_OPL],iceb_u_inttochar(E_DATA_OPL));


sprintf(strsql,"%s",gettext("Код формы оплаты"));
data.knopka_enter[E_FOROP]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_FOROP]), data.knopka_enter[E_FOROP], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_FOROP],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_FOROP],iceb_u_inttochar(E_FOROP));
gtk_widget_set_tooltip_text(data.knopka_enter[E_FOROP],gettext("Выбор формы оплаты"));

data.entry[E_FOROP] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_FOROP]),20);
gtk_box_pack_start (GTK_BOX (hbox[E_FOROP]), data.entry[E_FOROP], TRUE, TRUE,1);
g_signal_connect(data.entry[E_FOROP], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_FOROP]),data.rk.forop.ravno());
gtk_widget_set_name(data.entry[E_FOROP],iceb_u_inttochar(E_FOROP));

data.label_forop=gtk_label_new(naim_kontr.ravno(20));
gtk_box_pack_start (GTK_BOX (hbox[E_FOROP]), data.label_forop, TRUE, TRUE,1);


sprintf(strsql,"%s (%s)",gettext("Дата виписки налоговой накладной"),gettext("д.м.г"));
data.knopka_enter[E_DVNN]=gtk_button_new_with_label(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_DVNN]), data.knopka_enter[E_DVNN], FALSE, FALSE,1);
g_signal_connect(data.knopka_enter[E_DVNN],"clicked",G_CALLBACK(uosvhd1_v_e_knopka),&data);
gtk_widget_set_name(data.knopka_enter[E_DVNN],iceb_u_inttochar(E_DVNN));
gtk_widget_set_tooltip_text(data.knopka_enter[E_DVNN],gettext("Выбор даты"));

data.entry[E_DVNN] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_DVNN]),10);
gtk_box_pack_start (GTK_BOX (hbox[E_DVNN]), data.entry[E_DVNN], FALSE, FALSE,1);
g_signal_connect(data.entry[E_DVNN], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_DVNN]),data.rk.dvnn.ravno());
gtk_widget_set_name(data.entry[E_DVNN],iceb_u_inttochar(E_DVNN));

sprintf(strsql,"%s",gettext("Вид договора"));
label=gtk_label_new(strsql);
gtk_box_pack_start (GTK_BOX (hbox[E_VID_DOG]), label, FALSE, FALSE, 0);

data.entry[E_VID_DOG] = gtk_entry_new ();
gtk_entry_set_max_length(GTK_ENTRY(data.entry[E_VID_DOG]),255);
gtk_box_pack_start (GTK_BOX (hbox[E_VID_DOG]), data.entry[E_VID_DOG], TRUE, TRUE,1);
g_signal_connect(data.entry[E_VID_DOG], "activate",G_CALLBACK(uosvhd1_m_v_vvod),&data);
gtk_entry_set_text(GTK_ENTRY(data.entry[E_VID_DOG]),data.rk.vid_dog.ravno());
gtk_widget_set_name(data.entry[E_VID_DOG],iceb_u_inttochar(E_VID_DOG));


label=gtk_label_new(gettext("НДС"));
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), label, FALSE, FALSE, 0);

iceb_pm_nds(data.rk.datadok.ravno(),&data.opt,data.rk.lnds);
gtk_box_pack_start (GTK_BOX (hbox[E_NDS]), data.opt, FALSE, FALSE, 0);



sprintf(strsql,"F2 %s",gettext("Запись"));
data.knopka[FK2]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK2],gettext("Запись введённой в меню информации"));
g_signal_connect(data.knopka[FK2],"clicked",G_CALLBACK(uosvhd1_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK2],iceb_u_inttochar(FK2));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK2], TRUE, TRUE,1);

sprintf(strsql,"F3 %s",gettext("Операция"));
data.knopka[FK3]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK3],gettext("Выбор операции"));
g_signal_connect(data.knopka[FK3],"clicked",G_CALLBACK(uosvhd1_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK3],iceb_u_inttochar(FK3));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK3], TRUE, TRUE,1);


sprintf(strsql,"F10 %s",gettext("Выход"));
data.knopka[FK10]=gtk_button_new_with_label(strsql);
gtk_widget_set_tooltip_text(data.knopka[FK10],gettext("Завершение работы в этом окне"));
g_signal_connect(data.knopka[FK10],"clicked",G_CALLBACK(uosvhd1_m_v_knopka),&data);
gtk_widget_set_name(data.knopka[FK10],iceb_u_inttochar(FK10));
gtk_box_pack_start(GTK_BOX(hboxknop), data.knopka[FK10], TRUE, TRUE,1);

gtk_widget_grab_focus(data.entry[0]);

gtk_widget_show_all (data.window);


gtk_main();

if(data.voz == 0)
 {
  datadok->new_plus(data.rk.datadok.ravno());
  nomdok->new_plus(data.rk.nomdok.ravno());
 }

if(wpredok != NULL)
  gdk_window_set_cursor(gtk_widget_get_window(wpredok),gdk_cursor_new_for_display(gtk_widget_get_display(wpredok),ICEB_CURSOR));
return(data.voz);

}
/**********************************/
/*обработка ввода даты*/
/********************************/
void uosvhd1_od(class uosvhd1_m_data *data)
{
char strsql[512];
if(iceb_pbpds(data->rk.datadok.ravno(),data->window) != 0)
 {
  sprintf(strsql,gettext("Дата %s заблокирована !"),data->rk.datadok.ravno());
  iceb_menu_soob(strsql,data->window);
  gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATADOK]),data->rk.datadok_z.ravno());
  return;
 }

if(data->rk.datadok_z.getdlinna() > 1)
  if(iceb_pbpds(data->rk.datadok_z.ravno(),data->window) != 0)
   {
    sprintf(strsql,gettext("Дата %s заблокирована !"),data->rk.datadok_z.ravno());
    iceb_menu_soob(strsql,data->window);
    gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATADOK]),data->rk.datadok_z.ravno());
    return;
   }

    if(data->rk.datadok.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
      return;
     }

    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_NOMDOK])));
    if(data->rk.nomdok.getdlinna() <= 1 || data->god_nd != data->rk.datadok.ravno_god())
     {
      uosgetnd(data->rk.datadok.ravno_god(),data->rk.tipz,&data->rk.nomdok,data->window);
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
      data->god_nd=data->rk.datadok.ravno_god();
     }
}
/*****************************/
/*Обработчик нажатия enter кнопок  */
/*****************************/
void  uosvhd1_v_e_knopka(GtkWidget *widget,class uosvhd1_m_data *data)
{
class iceb_u_str kod("");
class iceb_u_str naim("");

int knop=atoi(gtk_widget_get_name(widget));
switch (knop)
 {
  case E_DATADOK:

    if(iceb_calendar(&data->rk.datadok,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATADOK]),data->rk.datadok.ravno());
      g_signal_emit_by_name(data->entry[E_DATADOK],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }      
    return;  

  case E_DVNN:

    if(iceb_calendar(&data->rk.dvnn,data->window) == 0)
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_DVNN]),data->rk.dvnn.ravno());
      
    return;  
  case E_DATA_VD:

    if(iceb_calendar(&data->rk.data_vd,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VD]),data->rk.data_vd.ravno());
      g_signal_emit_by_name(data->entry[E_DATA_VD],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }      
    return;  

  case E_DATA_VNN:

    if(iceb_calendar(&data->rk.data_vnn,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_VNN]),data->rk.data_vnn.ravno());
      g_signal_emit_by_name(data->entry[E_DATA_VNN],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }
    return;  

  case E_DATA_OPL:

    if(iceb_calendar(&data->rk.data_opl,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_DATA_OPL]),data->rk.data_opl.ravno());
      g_signal_emit_by_name(data->entry[E_DATA_OPL],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }      
    return;  

  case E_NOMDOK:

     data->rk.datadok.new_plus(gtk_entry_get_text(GTK_ENTRY(data->entry[E_DATADOK])));
     if(data->rk.datadok.ravno_god() == 0)
       {
        iceb_menu_soob(gettext("Не введена дата документа !"),data->window);
        return;
       }
     uosgetnd(data->rk.datadok.ravno_god(),data->rk.tipz,&data->rk.nomdok,data->window);
     gtk_entry_set_text(GTK_ENTRY(data->entry[E_NOMDOK]),data->rk.nomdok.ravno());
      
    return;  

  case E_PODR:
    if(l_uospodr(1,&kod,&naim,data->window) == 0)
     {
      data->rk.podr.new_plus(kod.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_podr),naim.ravno(20));
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_PODR]),data->rk.podr.ravno());
      g_signal_emit_by_name(data->entry[E_PODR],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }
      
    return;  

  case E_MAT_OT:
    if(l_uosmo(1,&kod,&naim,data->window) == 0)
     {
      data->rk.mat_ot.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_MAT_OT]),data->rk.mat_ot.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_mat_ot),naim.ravno(20));
      g_signal_emit_by_name(data->entry[E_MAT_OT],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }      
    return;  

  case E_KONTR:
    if(iceb_poi_kontr(&data->rk.kontr,&naim,data->window) == 0)
     {
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_KONTR]),data->rk.kontr.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_kontr),naim.ravno(20));
      g_signal_emit_by_name(data->entry[E_KONTR],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }
    return;  

  case E_FOROP:
    if(iceb_l_forop(1,&kod,&naim,data->window) == 0)
     {
      data->rk.forop.new_plus(kod.ravno());
      gtk_entry_set_text(GTK_ENTRY(data->entry[E_FOROP]),data->rk.forop.ravno());
      gtk_label_set_text(GTK_LABEL(data->label_forop),naim.ravno(20));
      g_signal_emit_by_name(data->entry[E_FOROP],"activate"); /*чтобы произошёл ввод и были отработаны все проверки*/
     }      
    return;  


 }
}


/*********************************/
/*Обработка нажатия клавиш       */
/*********************************/

gboolean   uosvhd1_m_v_key_press(GtkWidget *widget,GdkEventKey *event,class uosvhd1_m_data *data)
{

//printf("uosvhd1_m_v_key_press\n");
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
void  uosvhd1_m_v_knopka(GtkWidget *widget,class uosvhd1_m_data *data)
{

int knop=atoi(gtk_widget_get_name(widget));
//g_print("uosvhd1_m_v_knopka knop=%d\n",knop);
class iceb_u_str kod("");
class iceb_u_str naim("");

switch (knop)
 {
  case FK2:
    data->read_rek(); //Читаем реквизиты меню
    if(uosprhd(&data->rk,data->window) != 0)
     return;

    if(uoszaphd(&data->rk,data->window) != 0)
     return;
     
    data->voz=0;
    gtk_widget_destroy(data->window);
    return;  

  case FK3:
    if(data->rk.tipz == 1)
     {
      char strsql[512];
      //Узнаём тип операции
      SQL_str row;
      class SQLCURSOR cur;
      int vido1=0;
      int vido2=0;
      sprintf(strsql,"select vido from Uospri where kod='%s'",data->rk.kod_op.ravno());
      if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
       vido1=atoi(row[0]);

      if(l_uosopp(1,&kod,&naim,data->window) == 0)
       {
        sprintf(strsql,"select vido from Uospri where kod='%s'",kod.ravno());
        if(iceb_sql_readkey(strsql,&row,&cur,data->window) == 1)
         vido2=atoi(row[0]);
       
        if(vido1 != vido2)
         {
          iceb_menu_soob(gettext("Такая замена невозможна !"),data->window);
          return;
         }
        sprintf(strsql,"%s:%s %s (%s)",
        gettext("Операция"),
        kod.ravno(),
        naim.ravno(),
        gettext("Внешняя"));
        data->rk.kod_op.new_plus(kod.ravno());
        gtk_label_set_text(GTK_LABEL(data->label_kod_op),strsql);
         
       }
     }
    return;
    
  case FK10:
    gtk_widget_destroy(data->window);
    data->voz=1;
    return;
 }
}
/********************************/
/*Перевод чтение текста и перевод фокуса на следующюю строку ввода*/
/******************************************/

void    uosvhd1_m_v_vvod(GtkWidget *widget,class uosvhd1_m_data *data)
{

int enter=atoi(gtk_widget_get_name(widget));
//g_print("uosvhd1_m_v_vvod enter=%d\n",enter);


switch (enter)
 {
  case E_DATADOK:
    data->rk.datadok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    uosvhd1_od(data);
    break;

  case E_NOMDOK:
    data->rk.nomdok.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_PODR:
    data->rk.podr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_MAT_OT:
    data->rk.mat_ot.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_KONTR:
    data->rk.kontr.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_FOROP:
    data->rk.forop.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMDOK_POST:
    data->rk.nomdok_post.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_NOMNALNAK:
    data->rk.nomnalnak.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));

    data->read_rek();

    iceb_pnnnp(data->rk.data_vnn.ravno(),data->rk.dvnn.ravno(),data->rk.kontr.ravno(), data->rk.nomnalnak.ravno(),
    data->rk.datadok.ravno(),data->rk.nomdok.ravno(),3,data->window);

    break;

  case E_OSNOV:
    data->rk.osnov.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_DOVER:
    data->rk.dover.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;

  case E_SHEREZ:
    data->rk.sherez.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    break;
  
  case E_DATA_VD:
    data->rk.data_vd.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->rk.data_vd.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
      return;
     }
    break;
  
  case E_DATA_VNN:
    data->rk.data_vnn.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->rk.data_vnn.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
      return;
     }
    data->read_rek();

    iceb_pnnnp(data->rk.data_vnn.ravno(),data->rk.dvnn.ravno(),data->rk.kontr.ravno(), data->rk.nomnalnak.ravno(),
    data->rk.datadok.ravno(),data->rk.nomdok.ravno(),3,data->window);
    break;
  
  case E_DATA_OPL:
    data->rk.data_opl.new_plus(gtk_entry_get_text(GTK_ENTRY(widget)));
    if(data->rk.data_opl.prov_dat() != 0)
     {
      iceb_menu_soob(gettext("Неправильно введена дата!"),data->window);
      return;
     }
    break;


 }

enter+=1;
  
if(enter >= KOLENTER)
enter=0;

gtk_widget_grab_focus(data->entry[enter]);
 
}
