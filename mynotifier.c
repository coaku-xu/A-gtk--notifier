/* A timer to notify at the point you set 
 *
 * 2012-2-8 	Coaku
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

void notify(int signo);
void errpop();
void create_main_window();
void timer_insert(GtkWidget*,gpointer);
void fresh_list();
void popok();
void set_alarm();
void delete_first_timer();
static int cond1 = 0;

struct notify_node
{
	long int t_set_utc;
	char *todo;
	struct notify_node *next;
};

static GtkWidget *window;
static gint hour,min,sec;
static GtkWidget *hourspin,*minspin,*secspin;
static GtkTreeStore *store;
static const char *textbuf;
static struct notify_node *list_head;
static struct notify_node *list_tail;

enum {
	COLUMN_TIMELINE,
	COLUMN_TODOS,
//	COLUMN_STATUS,
	N_COLUMNS
};



int main(int argc,char *argv[])
{
	
	signal(SIGALRM,notify);
	list_head = list_tail = NULL;

	/* Initialise GTK */
	gtk_init (&argc, &argv);

	create_main_window();
	gtk_widget_show_all(window);

	/* Enter the event loop */
	gtk_main ();
	    
	return 0;
}

void create_main_window()
{
	GtkWidget *frame;
	GtkWidget *hbox;
	GtkWidget *hbox2;
	GtkWidget *main_vbox;
	GtkWidget *main_hbox;
	GtkWidget *vbox;
	GtkWidget *vbox2;
	GtkWidget *vbox3;
	GtkWidget *button;
	GtkWidget *label;
	GtkAdjustment *adj;
	GtkWidget *entry;
	GtkWidget *view;
	GtkCellRenderer *renderer;
	gint viewheight;

	struct tm *timen;
	time_t	timenow;
	static gint hour_now,min_now,sec_now;

	time(&timenow);
	timen = localtime(&timenow);

	hour_now = (gint)timen->tm_hour;
	min_now = (gint)timen->tm_min;
	sec_now = (gint)timen->tm_sec;

	window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	gtk_window_set_position(GTK_WINDOW(window),GTK_WIN_POS_CENTER);
	
	g_signal_connect (window, "destroy", G_CALLBACK (gtk_main_quit), NULL);
	
	gtk_window_set_title (GTK_WINDOW (window), "Coaku's Notifier");

	/* main_hbox -> window */
	main_hbox = gtk_hbox_new(FALSE, 5);
	gtk_container_set_border_width (GTK_CONTAINER (main_hbox), 10);
	gtk_container_add (GTK_CONTAINER (window), main_hbox);
	
	/* main_vbox -> main_hbox */
	main_vbox = gtk_vbox_new (FALSE, 5);
	gtk_container_set_border_width(GTK_CONTAINER (main_vbox), 5);
	gtk_box_pack_start (GTK_BOX (main_hbox), main_vbox, TRUE, TRUE, 0);

	/* vbox3 -> main_hbox */
	vbox3 = gtk_hbox_new(FALSE,0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox3), 5);
	gtk_box_pack_start (GTK_BOX (main_hbox), vbox3, TRUE, TRUE, 0);

	/* view -> vbox3 */
	store = gtk_tree_store_new(N_COLUMNS, G_TYPE_STRING,  G_TYPE_STRING);
	view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (store));
	gtk_widget_set_size_request(view,350,-1);
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view),
			COLUMN_TIMELINE,"   Timeline   ", renderer,"text", COLUMN_TIMELINE,NULL);
	gtk_tree_view_insert_column_with_attributes (GTK_TREE_VIEW(view),
			COLUMN_TODOS,"   To do", renderer,"text", COLUMN_TODOS,NULL);
	gtk_box_pack_start (GTK_BOX (vbox3), view, TRUE, TRUE, 5);

	/* frame -> main_vbox & vbox -> frame */
	frame = gtk_frame_new ("Set Notifier Time");
	gtk_box_pack_start (GTK_BOX (main_vbox), frame, TRUE, TRUE, 0);
	  
	vbox = gtk_vbox_new (FALSE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 5);
	gtk_container_add (GTK_CONTAINER (frame), vbox);
	  
	/* hbox -> vbox & vbox2 -> hbox */  
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 5);
	  
	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 5);

	 
	/* label -> vbox2 */
	label = gtk_label_new ("Hour :");
	gtk_misc_set_alignment (GTK_MISC(label), 0, 0.5);
	gtk_box_pack_start (GTK_BOX(vbox2), label, FALSE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new (hour_now, 0.0, 23.0, 1.0, 1.0, 0.0);
	hourspin = gtk_spin_button_new (adj, 0, 0);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (hourspin), TRUE);
	gtk_box_pack_start (GTK_BOX (vbox2), hourspin, FALSE, TRUE, 0);


	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 5);
	label = gtk_label_new ("Min :");
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
	gtk_box_pack_start (GTK_BOX (vbox2), label, FALSE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new (min_now, 0.0, 59.0, 1.0, 1.0, 0.0);
	minspin = gtk_spin_button_new (adj, 0, 0);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (minspin), TRUE);
	gtk_box_pack_start (GTK_BOX (vbox2), minspin, FALSE, TRUE, 0);

	  
	vbox2 = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (hbox), vbox2, TRUE, TRUE, 5);
	label = gtk_label_new ("Sec :");
	gtk_misc_set_alignment (GTK_MISC (label), 0, 0.5);
	gtk_box_pack_start (GTK_BOX (vbox2), label, FALSE, TRUE, 0);
	adj = (GtkAdjustment *) gtk_adjustment_new (sec_now, 0.0, 59.0, 1.0, 1.0, 0.0);
	secspin = gtk_spin_button_new (adj, 0, 0);
	gtk_spin_button_set_wrap (GTK_SPIN_BUTTON (secspin), FALSE);
	gtk_box_pack_start (GTK_BOX (vbox2), secspin, FALSE, TRUE, 0);

	/* GtkEntry */
	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);

	label = gtk_label_new ("Put notes here:");
	gtk_misc_set_alignment (GTK_MISC(label), 0, 1);
	gtk_box_pack_start (GTK_BOX(hbox), label, TRUE, TRUE, 5);

	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, TRUE, TRUE, 5);
	entry = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(hbox),entry,TRUE,TRUE,5);


	hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX (vbox), hbox, FALSE, TRUE, 5);
	button = gtk_button_new_with_label ("Cancel");
	g_signal_connect_swapped (button, "clicked", G_CALLBACK (gtk_widget_destroy),	window);
	gtk_box_pack_start (GTK_BOX(hbox), button, TRUE, TRUE, 5);

	button = gtk_button_new_with_label("Add to timeline");
	g_signal_connect(button,"clicked",G_CALLBACK(timer_insert),entry);
	gtk_box_pack_start(GTK_BOX(hbox),button,TRUE,TRUE,5);
}

void notify(int signo)
{
	GtkWidget *dialog;
	gint result;
	struct tm *tmp;
	char *time_string = (char *)malloc(15);

	tmp = localtime((time_t *)(&(list_head->t_set_utc)));
	sprintf(time_string,"%02d:%02d's message",tmp->tm_hour,tmp->tm_min);

	dialog = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Ding Ding Ding...");
	gtk_window_set_title(GTK_WINDOW(dialog),time_string);
	gtk_message_dialog_format_secondary_text((GtkMessageDialog *)dialog,"It's time to %s",list_head->todo);
	g_signal_connect(dialog,"response",G_CALLBACK(gtk_widget_destroy),dialog);
	result = gtk_dialog_run(GTK_DIALOG(dialog));

	free(time_string);
	signal(SIGALRM,notify);

	/* Attention: fresh_list must be perior than delete_first_timer !!*/
	fresh_list();
	delete_first_timer();
	set_alarm();
	gtk_widget_show_all(window);
}

void errpop()
{
	GtkWidget *dialog;
	gint result;

	dialog = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_ERROR,GTK_BUTTONS_CANCEL,"Sorry...");
	gtk_message_dialog_format_secondary_text((GtkMessageDialog *)dialog,"Set a time BEFORE NOW is not allowed :(");
	g_signal_connect(dialog,"response",G_CALLBACK(gtk_widget_destroy),dialog);
	result = gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_show_all(window);
}

void timer_insert(GtkWidget *widget,gpointer data)
{
	struct tm *tmp;
	time_t future,t_now;
	long int left;
	int dayset,daynow;
	struct notify_node *p;
	char *todo;
		
	p = (struct notify_node *)malloc((size_t)sizeof(struct notify_node));

	textbuf = gtk_entry_get_text(GTK_ENTRY(data));
       	todo = (char *)malloc(strlen(textbuf)+1);
	strcpy(todo,textbuf);
	gtk_entry_set_text(GTK_ENTRY(data),"");

	hour = gtk_spin_button_get_value_as_int((GtkSpinButton *)hourspin);
	min = gtk_spin_button_get_value_as_int((GtkSpinButton *)minspin);
	sec = gtk_spin_button_get_value_as_int((GtkSpinButton *)secspin);

	time(&t_now);
	tmp = localtime(&t_now);

	dayset = hour*3600+min*60+sec;
	daynow = (tmp->tm_hour)*3600+(tmp->tm_min)*60+tmp->tm_sec;

	if (dayset <= daynow )
	{
		errpop();
		return;
	}
	tmp->tm_hour = (int)hour;
	tmp->tm_min = (int)min;
	tmp->tm_sec = (int)sec;

	future = mktime(tmp);

	p->t_set_utc = (long int)future;
	p->todo = todo;
	p->next = NULL;

	if (list_tail != NULL)
	{
		list_tail->next = p;
		list_tail = p;
	}
	else
		list_head = list_tail = p;

	fresh_list();
}

void fresh_list()
{
	struct notify_node *p = list_head;
	struct tm *tmp;
	GtkTreeIter child_iter;
	char *time_string = (char *)malloc(5);

	if (cond1 == 0)
	{
		set_alarm();
		cond1 = 1;
	}

	gtk_tree_store_clear(store);

	for(;;)
	{
		if(p == NULL)
			break;
		tmp = localtime((time_t *)(&(p->t_set_utc)));
		sprintf(time_string,"%2d:%2d",tmp->tm_hour,tmp->tm_min);
		gtk_tree_store_append(store, &child_iter, NULL);
		gtk_tree_store_set(store, &child_iter, COLUMN_TIMELINE,time_string,COLUMN_TODOS,p->todo,-1);
		gtk_widget_show_all(window);
		p = p->next;
	}
}

void set_alarm()
{
	int left;
	time_t t_now;
	
	if (list_head == NULL)
		return;

	time(&t_now);

	left = (long int)(list_head->t_set_utc)-(long int)t_now;

	if (left <= 0)	return;
	alarm((unsigned int)left);
}

#ifdef A
void popok()
{
	GtkWidget *dialog;
	gint result;

	dialog = gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"Add OK !");
	gtk_message_dialog_format_secondary_text((GtkMessageDialog *)dialog,"Timer is added successfully.");
	g_signal_connect(dialog,"response",G_CALLBACK(gtk_widget_destroy),dialog);
	result = gtk_dialog_run(GTK_DIALOG(dialog));

	gtk_widget_show_all(window);
}
#endif

void delete_first_timer()
{
	struct notify_node *p = list_head;
	
	if (list_head == list_tail)
	{
		list_head = list_tail = NULL;
		cond1 = 0;
		free(p->todo);
		free(p);
	}
	else
	{
		list_head = list_head->next;
		free(p->todo);
		free(p);
	}
}
	
