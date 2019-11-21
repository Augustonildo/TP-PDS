#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_primitives.h>
#include <stdlib.h>
#include <windows.h>
#include <math.h>


#define SCREEN_W 480
#define SCREEN_H 640
#define FPS 60

#define N_COLS 6
#define N_LINHAS 8

#define N_TYPES 4

const int COL_W = SCREEN_W/N_COLS;
const int LIN_W = SCREEN_H/N_LINHAS;

typedef struct Candy {
	int type;
	int active;
	ALLEGRO_COLOR color;
} Candy;

Candy M[N_LINHAS][N_COLS];
int Seq[N_LINHAS][N_COLS];

void initCandies(){
	int i,j;
	
	for(i = 0; i < N_LINHAS; i++){
		for(j = 0; j < N_COLS; j++){
			M[i][j].type = 1 + rand()%N_TYPES;
			M[i][j].active = 1;
			M[i][j].color = al_map_rgb(M[i][j].type * 63,M[i][j].type * 63, M[i][j].type * 63);
			printf("%d ", M[i][j].type);
		}
		printf("\n");
	}
}

int getXCoord(int col){
	return col * COL_W; 
}

int getYCoord(int lin){
	return lin * LIN_W;	
}

void desenhaCandy(int lin, int col){
	int x = getXCoord(col);
	int y = getYCoord(lin);
	
	if(M[lin][col].type == 1){
		al_draw_filled_rectangle(x,y,x+COL_W, y+LIN_W, M[lin][col].color);
	}else if (M[lin][col].type == 2){
		al_draw_filled_rounded_rectangle(x,y,x+COL_W, y+LIN_W, COL_W/3, LIN_W/3, M[lin][col].color);
	}else if (M[lin][col].type == 3){
		al_draw_filled_ellipse(x+COL_W/2, y+LIN_W/2, COL_W/3, LIN_W/3, M[lin][col].color);
	}else if (M[lin][col].type == 4){
		al_draw_filled_triangle(x+COL_W/2, y, x, y+LIN_W, x+COL_W, y+LIN_W, M[lin][col].color);
	}
}



void draw_scenario(ALLEGRO_DISPLAY *display) {
 	
	
	ALLEGRO_COLOR BKG_COLOR = al_map_rgb(0,0,0); 
	al_set_target_bitmap(al_get_backbuffer(display));
	al_clear_to_color(BKG_COLOR);   
	
	int i,j;
	
	for(i = 0; i < N_LINHAS; i++){
		for(j = 0; j < N_COLS; j++){
			desenhaCandy(i, j);
		}
	}
	
}

int verificaSequencia(){
    int i, j;
    int existe = 0;
    
    for(i = 0; i < N_LINHAS; i++){
        for(j = 0; j < N_COLS; j++){
            Seq[i][j] = 0;
        }
    }
    
    for(i = 0; i < N_LINHAS; i++){
        for(j = 0; j < N_COLS-2; j++){
            if(M[i][j].type != 0 && M[i][j].type == M[i][j+1].type && M[i][j].type == M[i][j+2].type){
                Seq[i][j] = 1;
                Seq[i][j+1] = 1;
                Seq[i][j+2] = 1;
                existe = 1;
            }
        }
    }
    
    for(i = 0; i < N_LINHAS-2; i++){
        for(j = 0; j < N_COLS; j++){
            if(M[i][j].type != 0 && M[i][j].type == M[i+1][j].type && M[i][j].type == M[i+2][j].type){
                Seq[i][j] = 1;
                Seq[i+1][j] = 1;
                Seq[i+2][j] = 1;
                existe = 1;
            }
        }
    }
    return existe;
}

void destroiCandies(){
	int i,j;
    int contador = 0;
	
    for(i = 0; i < N_LINHAS; i++){
        for(j = 0; j < N_COLS; j++){
            if(Seq[i][j]){
                M[i][j].type = 0;
                M[i][j].active = 0;
            }
            if(M[i][j].active == 0){
                contador++;
            }
        }
    }
    printf("Zeros: %d\n", contador);
}

void sobeZeros(ALLEGRO_DISPLAY *display){
	int i,j;
    int flag = 0;
	Candy aux;
    
    for(i = 0; i < N_LINHAS-1; i++){
        for(j = 0; j < N_COLS; j++){
            if(M[i][j].type != 0 && M[i+1][j].type == 0){
                aux = M[i+1][j];
				M[i+1][j] = M[i][j];
                M[i][j] = aux;
                flag = 1;
            }
        }
        if(flag){
            i = -1;
            flag = 0;
			draw_scenario(display);
			Sleep(1);
        }
    }
}

void getCell(int x, int y, int* lin, int* col){
	*lin = y/LIN_W;
	*col = x/COL_W;
}

void swap(int lin_src, int col_src, int lin_dst, int col_dst){
	Candy aux;
	aux = M[lin_src][col_src];
	M[lin_src][col_src] = M[lin_dst][col_dst];
	M[lin_dst][col_dst] = aux;
}

int main(int argc, char **argv){

	ALLEGRO_DISPLAY *display = NULL;
	ALLEGRO_EVENT_QUEUE *event_queue = NULL;
	ALLEGRO_TIMER *timer = NULL;


	//----------------------- rotinas de inicializacao ---------------------------------------
	if(!al_init()) {
		fprintf(stderr, "failed to initialize allegro!\n");
		return -1;
	}

	if(!al_init_primitives_addon()){
		fprintf(stderr, "failed to initialize primitives!\n");
		return -1;
	}

	timer = al_create_timer(1.0 / FPS);
	if(!timer) {
		fprintf(stderr, "failed to create timer!\n");
		return -1;
	}

	display = al_create_display(SCREEN_W, SCREEN_H);
	if(!display) {
		fprintf(stderr, "failed to create display!\n");
		al_destroy_timer(timer);
		return -1;
	}

	if(!al_install_mouse())
		fprintf(stderr, "failed to initialize mouse!\n");   


	event_queue = al_create_event_queue();
	if(!event_queue) {
		fprintf(stderr, "failed to create event_queue!\n");
		al_destroy_display(display);
		al_destroy_timer(timer);
		return -1;
	}
	al_install_keyboard();
   //registra na fila de eventos que eu quero identificar quando a tela foi alterada
	al_register_event_source(event_queue, al_get_display_event_source(display));
   //registra na fila de eventos que eu quero identificar quando o tempo alterou de t para t+1
	al_register_event_source(event_queue, al_get_timer_event_source(timer));
	//registra o teclado na fila de eventos:
	al_register_event_source(event_queue, al_get_keyboard_event_source());   
	//registra mouse na fila de eventos:
	al_register_event_source(event_queue, al_get_mouse_event_source());    
   //inicia o temporizador
	al_start_timer(timer);
	
	// Inicializa a Matriz de Candies
	do{
		initCandies();
	}while(verificaSequencia());

	int playing = 1;
	//enquanto playing for verdadeiro, faca:
	
	int lin_src, col_src, lin_dst, col_dst;
	while(playing) {
		ALLEGRO_EVENT ev;
	  //espera por um evento e o armazena na variavel de evento ev
		al_wait_for_event(event_queue, &ev);

		if(ev.type == ALLEGRO_EVENT_KEY_DOWN) {
			if(ev.keyboard.keycode == ALLEGRO_KEY_ESCAPE) {
				playing = 0;
			}

		}
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			printf("\nclicou em (%d, %d)", ev.mouse.x, ev.mouse.y);
			getCell(ev.mouse.x, ev.mouse.y, &lin_src, &col_src);
		}
		else if(ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			printf("\nsoltou em (%d, %d)", ev.mouse.x, ev.mouse.y);
			getCell(ev.mouse.x, ev.mouse.y, &lin_dst, &col_dst);
			swap(lin_src, col_src, lin_dst, col_dst);
		}		
	    //se o tipo de evento for um evento do temporizador, ou seja, se o tempo passou de t para t+1
		else if(ev.type == ALLEGRO_EVENT_TIMER) {
		    draw_scenario(display);
			al_flip_display();		
		}
	    //se o tipo de evento for o fechamento da tela (clique no x da janela)
		else if(ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			playing = 0;
		}
		
		while(verificaSequencia()){
			destroiCandies();
			sobeZeros(display);
		}
	} 

	al_rest(1);

	al_destroy_timer(timer);
	al_destroy_display(display);
	al_destroy_event_queue(event_queue);

	return 0;
}
