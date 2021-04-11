/*$Id:*/
/*30.12.2010	24.09.2010	п▒п╣п╩я▀я┘ п░.п≤.	iceb_print.c

п÷я─п╬пЁя─п╟п╪п╪п╟ пЄп╩я▐ п╬я┌п╩п╟пЄп╨п╦ п╡п╦пЄпІп╣я┌п╟ п©п╣я┤п╟я┌п╦
*/
#include <errno.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <stdlib.h>
#include        <locale.h>
#include <iceb_libbuh.h>
#include <nl_types.h>
#include <libintl.h>
#include "../xpm/iceBw_icon.xpm"

int iceb_print_operation(GtkPrintOperationAction operation_action,const char *filname,GtkWidget *wpredok);




const char		*name_system={"iceb_print"};
const char            *version={"0.0"};

int main(int argc,char **argv)
{
setlocale(LC_ALL,"");/*Устанавливаются переменные локали из окружающей среды*/

gtk_init( &argc, &argv );

//Делаем обязятельно после gtk_ini
(void)setlocale(LC_NUMERIC,"C"); //Переменная среды определяющая чем разделяется целая и дробная часть числа




GdkPixbuf *iconca=gdk_pixbuf_new_from_xpm_data(iceBw_icon_xpm);  
gtk_window_set_default_icon(iconca);


class iceb_u_str titl(gettext("Введите имя файла"));
class iceb_u_str imaf("");
if(iceb_menu_vvod1(&titl,&imaf,40,"",NULL) != 0)
   return(1);

iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG,imaf.ravno(),NULL);
//iceb_print_operation(GTK_PRINT_OPERATION_ACTION_PREVIEW,imaf.ravno(),NULL);

/*************
  cairo_surface_t * s = cairo_image_surface_create(CAIRO_FORMAT_ARGB32, 1280, 800);
  cairo_t * c = cairo_create (s);
  cairo_scale (c, 10, 10);
  PangoLayout * l = pango_cairo_create_layout (c);
  pango_layout_set_text (l, "00", 2);
  PangoAttrList * attrs = pango_attr_list_new ();
  pango_attr_list_insert (attrs, pango_attr_underline_new(PANGO_UNDERLINE_DOUBLE));
  pango_layout_set_attributes (l, attrs);
  cairo_move_to (c, 1, 1);
  pango_cairo_show_layout (c, l);
  cairo_surface_write_to_png (s, "test.png");
*************/

return(0);
}
