

#define un

#define BITMASK_A 0b10000
#define BITMASK_B 0b1000
#define BITMASK_C 0b100
#define BITMASK_D 0b10
#define BITMASK_E 0b1

RT_EVENT event;
int main(){
    rt_event_create(&event, "mes_beaux_drapeaux", 0, EV_PRIO);

    // lancement des threads

    rt_event_wait(&event, BITMASK_A, 0, EV_ALL);
    rt_event_delete(&event, )... ou join
}

void A(){
    #ifdef un
    rt_event_wait(&event, BITMASK_B | BITMASK_C, 0, EV_ALL);
    #else
    rt_event_wait(&event, BITMASK_B | BITMASK_C, 0, EV_ANY);
    #endif
    // traitement
    rt_event_signal(&event, BITMASK_A);
}
void B(){
    rt_event_wait(&event, BITMASK_D, 0, EV_ALL);
    // traitement
    rt_event_signal(&event, BITMASK_B);
}
void C(){
    rt_event_wait(&event, BITMASK_D |  BITMASK_E, 0, EV_ALL);
    // traitement
    rt_event_signal(&event, BITMASK_C);

}
void D(){
    // Traitement
    rt_event_signal(&event, BITMASK_D);
}
void E(){
    // Traitement
    rt_event_signal(&event, BITMASK_E);

}