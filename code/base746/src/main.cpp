#define ARDUINO

#ifdef ARDUINO

#include "lvglDrivers.h"
#include "images.h"
#include <chrono>
#include <Servo.h>

#define TIMECHRONO 5000

Servo barriere;

// pin des capteurs 
const int PIN_IRE = PA0;
const int PIN_IRS = PB9;

static lv_obj_t * label_parking = NULL;
static lv_obj_t * label_status = NULL;
static lv_obj_t * btn_ouvrir = NULL;
static lv_obj_t * btn_fermer = NULL;
static lv_obj_t * btn_ticket = NULL;
static lv_obj_t * swt_manuel = NULL;
static lv_obj_t * img_barriere = NULL;
static lv_obj_t * label_manuel = NULL;

extern bool lvglLock(TickType_t xBlockTime);
extern bool lvglUnlock();

// flags du fonctionnement du système 
bool ticket_ok = false;
bool etat_barriere = false;
bool voiture = false;
bool mode_manuel = false; 

using Clock = std::chrono::steady_clock;
static Clock::time_point barriere_open_start;


// gestion des événements des boutons et du switch
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * target = (lv_obj_t *) lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
      if (target == btn_ouvrir) {
        etat_barriere = true;
        lv_obj_add_state(btn_ouvrir, LV_STATE_DISABLED);
        lv_obj_clear_state(btn_fermer, LV_STATE_DISABLED);
        barriere.write(60);
        lv_image_set_src(img_barriere, &barriere_ouverte);
        lv_obj_add_state(swt_manuel, LV_STATE_DISABLED);
        barriere_open_start = Clock::now();
      }
      else if (target == btn_fermer) {
        etat_barriere = false;
        lv_obj_add_state(btn_fermer, LV_STATE_DISABLED);
        lv_obj_clear_state(btn_ouvrir, LV_STATE_DISABLED);
        barriere.write(140);
        lv_image_set_src(img_barriere, &barriere_fermee);
        lv_obj_clear_state(swt_manuel, LV_STATE_DISABLED);
      }
      else if (target == btn_ticket) {
        ticket_ok = true;
        etat_barriere = true;
        lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
      }
    }
    if (code == LV_EVENT_VALUE_CHANGED && target == swt_manuel) {
      if (lv_obj_has_state(swt_manuel, LV_STATE_CHECKED)) { // mode manuel 
          lv_label_set_text(label_manuel, "Mode manuel");
          mode_manuel = true;
          lv_obj_clear_state(btn_ouvrir, LV_STATE_DISABLED); 
      } else { // mode automatique
          mode_manuel = false;
          lv_label_set_text(label_manuel, "Mode auto");
          lv_obj_add_state(btn_ouvrir, LV_STATE_DISABLED); 
          lv_obj_add_state(btn_fermer, LV_STATE_DISABLED); 
      }
    }
}

// initialisation des capteurs, du servo et de la communication série
void init_system() {
  Serial.begin(115200);
  pinMode(PIN_IRE, INPUT);
  pinMode(PIN_IRS, INPUT);
  barriere.attach(PIN_A5);
}


// initialisation de l'affichage du lvgl
void init_affichage()
{
  barriere.write(140);

  lv_obj_set_style_bg_color(lv_screen_active(), lv_color_hex(0xE3C1C1), 0); // couleur de fond 
  lv_obj_set_style_bg_opa(lv_screen_active(), LV_OPA_COVER, 0);

  label_parking = lv_label_create(lv_screen_active());
  lv_label_set_text(label_parking, "Parking de l'IUT");
  lv_obj_align(label_parking, LV_ALIGN_TOP_MID, 1, 6);
  lv_obj_set_style_text_font(label_parking, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(label_parking, lv_color_hex(0x590C0C), 0);

  // image barriere fermee sans voiture au demarrage 
  img_barriere = lv_image_create(lv_screen_active());
  lv_image_set_src(img_barriere, &barriere_fermee);
  lv_obj_align(img_barriere, LV_ALIGN_BOTTOM_RIGHT, 0, 0);
  lv_obj_set_size(img_barriere, 200, 150);

  // bouton ouverture barriere 
  btn_ouvrir = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn_ouvrir, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(btn_ouvrir, 160, 50);
  lv_obj_align(btn_ouvrir, LV_ALIGN_BOTTOM_LEFT, 20, -80);
  lv_obj_remove_flag(btn_ouvrir, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(btn_ouvrir, lv_color_hex(0x612C2C), 0); // 
  lv_obj_set_style_radius(btn_ouvrir, LV_RADIUS_CIRCLE, 0);
  // lv_obj_set_style_border_color(btn_ouvrir, lv_color_hex(0x2B5D99), 0); // Bordure 
  // lv_obj_set_style_border_width(btn_ouvrir, 4, 0);                     // Épaisseur de 4 pixels
  // lv_obj_set_style_border_opa(btn_ouvrir, LV_OPA_80, 0);                // Opacité du contour
  lv_obj_set_style_shadow_width(btn_ouvrir, 15, 0);
  lv_obj_set_style_shadow_color(btn_ouvrir, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(btn_ouvrir, LV_OPA_30, 0);
  lv_obj_set_style_shadow_offset_y(btn_ouvrir, 6, 4); // Décale l'ombre vers le bas
  lv_obj_t * labelbutton1 = lv_label_create(btn_ouvrir);
  lv_label_set_text(labelbutton1, "Ouverture barriere");
  lv_obj_center(labelbutton1);
  lv_obj_set_style_text_color(labelbutton1, lv_color_hex(0xFFFFFF), 0);
  lv_obj_add_state(btn_ouvrir, LV_STATE_DISABLED); // desactive au demarrage 

  // bouton fermeture barriere 
  btn_fermer = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn_fermer, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(btn_fermer, 160, 50);
  lv_obj_align(btn_fermer, LV_ALIGN_BOTTOM_LEFT, 20, -20);
  lv_obj_remove_flag(btn_fermer, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(btn_fermer, lv_color_hex(0x612C2C), 0); // 
  lv_obj_set_style_radius(btn_fermer, LV_RADIUS_CIRCLE, 0);
  // lv_obj_set_style_border_color(btn_fermer, lv_color_hex(0x2B5D99), 0); // Bordure 
  // lv_obj_set_style_border_width(btn_fermer, 4, 0);                     // Épaisseur de 4 pixels
  // lv_obj_set_style_border_opa(btn_fermer, LV_OPA_80, 0);                // Opacité du contour
  lv_obj_set_style_shadow_width(btn_fermer, 15, 0);
  lv_obj_set_style_shadow_color(btn_fermer, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(btn_fermer, LV_OPA_30, 0);
  lv_obj_set_style_shadow_offset_y(btn_fermer, 6, 4); // Décale l'ombre vers le bas
  lv_obj_t * labelbutton2 = lv_label_create(btn_fermer);
  lv_label_set_text(labelbutton2, "Fermeture barriere");
  lv_obj_center(labelbutton2);
  lv_obj_set_style_text_color(labelbutton2, lv_color_hex(0xFFFFFF), 0);
  lv_obj_add_state(btn_fermer, LV_STATE_DISABLED); // desactive au demarrage 

  // bouton du ticket 
  btn_ticket = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn_ticket, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(btn_ticket, 160, 70);
  lv_obj_align(btn_ticket, LV_ALIGN_TOP_RIGHT, -20, 50);
  lv_obj_remove_flag(btn_ticket, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(btn_ticket, lv_color_hex(0x7A1414), 0); // rouge
  lv_obj_set_style_radius(btn_ticket, LV_RADIUS_CIRCLE, 0);
  // lv_obj_set_style_border_color(btn_ticket, lv_color_hex(0x915F27), 0); // Bordure 
  // lv_obj_set_style_border_width(btn_ticket, 4, 0);                     // Épaisseur de 4 pixels
  // lv_obj_set_style_border_opa(btn_ticket, LV_OPA_80, 0);                // Opacité du contour
  lv_obj_set_style_shadow_width(btn_ticket, 15, 0);
  lv_obj_set_style_shadow_color(btn_ticket, lv_color_hex(0x000000), 0);
  lv_obj_set_style_shadow_opa(btn_ticket, LV_OPA_30, 0);
  lv_obj_set_style_shadow_offset_y(btn_ticket, 6, 4); // Décale l'ombre vers le bas
  lv_obj_t * labelbutton3 = lv_label_create(btn_ticket);
  lv_label_set_text(labelbutton3, "Prendre\nun ticket");
  lv_obj_set_style_text_align(labelbutton3, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(labelbutton3);
  lv_obj_set_style_text_color(labelbutton3, lv_color_hex(0xFFFFFF), 0);
  lv_obj_add_state(btn_ticket, LV_STATE_DISABLED); // desactive au demarrage 
  
  // bouton pour le passage au mode manuel 
  swt_manuel = lv_switch_create(lv_screen_active());
  lv_obj_add_event_cb(swt_manuel, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(swt_manuel, 140, 50);
  lv_obj_align(swt_manuel, LV_ALIGN_TOP_LEFT, 30, 80);
  lv_obj_remove_flag(swt_manuel, LV_OBJ_FLAG_PRESS_LOCK);
  // switch mode auto
  lv_obj_set_style_bg_color(swt_manuel, lv_color_hex(0x614141), LV_PART_MAIN | LV_STATE_DEFAULT); 
  // switch mode manuel
   lv_obj_set_style_bg_color(swt_manuel, lv_color_hex(0x994646), LV_PART_INDICATOR | LV_STATE_CHECKED); 
  lv_obj_set_style_radius(swt_manuel, LV_RADIUS_CIRCLE, LV_PART_INDICATOR);
  // curseur
  lv_obj_set_style_bg_color(swt_manuel, lv_color_hex(0xDEC1C1), LV_PART_KNOB | LV_STATE_DEFAULT); // Curseur blanc
  lv_obj_set_style_radius(swt_manuel, LV_RADIUS_CIRCLE, LV_PART_KNOB);
  lv_obj_set_style_pad_all(swt_manuel, 0, LV_PART_MAIN | LV_STATE_CHECKED);

  label_manuel = lv_label_create(lv_screen_active());
  lv_label_set_text(label_manuel, "Mode auto");
  lv_obj_align_to(label_manuel, swt_manuel, LV_ALIGN_OUT_TOP_MID, -10, 0);
  lv_obj_set_style_text_color(label_manuel, lv_color_hex(0x590C0C), 0);
}


// gestion du chrono pour la fermeture automatique de la barrière après 5 secondes 
void gestion_chrono() {
  if (etat_barriere) {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - barriere_open_start).count();

    // apres 5 secondes : fermeture barrière,affichage correct des boutons et changement des flags 
    if (elapsed >= TIMECHRONO) {
      barriere.write(140);
      if (mode_manuel == false) {
        ticket_ok = false;
        lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
        etat_barriere = false;
      } else {
        lv_obj_clear_state(btn_ouvrir, LV_STATE_DISABLED);
        lv_obj_clear_state(swt_manuel, LV_STATE_DISABLED);
        lv_obj_add_state(btn_fermer, LV_STATE_DISABLED);
        lv_image_set_src(img_barriere, &barriere_fermee);
        etat_barriere = false;
      }
    }
  }
}

// affichage de l'image (4 images différentes) en fonction de l'état de la barrière et de la présence d'une voiture
void gestion_image() {
  if (voiture) { //&& !mode_manuel) {
    if(etat_barriere) lv_image_set_src(img_barriere, &barriere_ouverte_voiture);
    else lv_image_set_src(img_barriere, &barriere_fermee_voiture);
  }
  else if (!voiture) { //} && !mode_manuel) {
    if(etat_barriere) lv_image_set_src(img_barriere, &barriere_ouverte);
    else lv_image_set_src(img_barriere, &barriere_fermee);
  }
}

// lecture des capteurs de présence (infrarouges)
int gestion_capteurs() {
  int valE = digitalRead(PIN_IRE);
  int valS = digitalRead(PIN_IRS);

  // voiture passant la barrière    
  if (valE == LOW && valS == LOW) {
    if (!mode_manuel) {
      etat_barriere = true;
      voiture = true;
    } 
    voiture = true;
    return 1;
  }
  // voiture en entrée sans ticket 
  else if (valE == LOW && ticket_ok == false) {
    if (!mode_manuel) {
      etat_barriere = false;
      lv_obj_clear_state(btn_ticket, LV_STATE_DISABLED);
    } 
    voiture = true;
    return 1;
  }
  // voiture en entrée avec ticket 
  else if (valE == LOW && ticket_ok == true) {
    if (!mode_manuel) {
      etat_barriere = true;
      barriere.write(60);
      barriere_open_start = Clock::now();
    }
    voiture = true;
    return 1;
  }
  // voiture en sortie avec ticket 
  else if (valS == LOW && ticket_ok == true) {
    if (!mode_manuel) {
      etat_barriere = false;
      ticket_ok = false;
      delay(500);
      barriere.write(140);
      lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
    }
    voiture = true;
    return 1;
  }
  // voiture en sortie en mode manuel 
  else if (valS == LOW && ticket_ok == false) {
    voiture = true;
    return 1;
  }
  // voiture sous la barrière avec ticket 
  else if (valE == HIGH && valS == HIGH && ticket_ok == true) {
    if (!mode_manuel) {
      etat_barriere = true;
      lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
    }
    voiture = false;
    return 1;
  }
  else {
    if (!mode_manuel) {
      etat_barriere = false;
      lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
    }
    voiture = false;
    return 0;
  }
  return 0;
}

// tâche principale 
void mySetup()
{
  init_system();
  init_affichage();
}

void loop() {}


// tâche qui gère les capteurs, l'affichage et le chrono
void myTask(void *pvParameters)
{
  TickType_t xLastWakeTime = xTaskGetTickCount();

  while (1)
  {
    gestion_capteurs();

    if (lvglLock(pdMS_TO_TICKS(100))) {
      gestion_image();
      gestion_chrono();
      lvglUnlock();
    }

    vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(200));
  }
}

#else

extern "C" {
#include "lvgl.h"
}
#include "app_hal.h"
#include <cstdio>

int main(void)
{
  printf("LVGL Simulator\n");
  fflush(stdout);

  lv_init();
  hal_setup();
  testLvgl();
  hal_loop();
  return 0;
}

#endif