#define ARDUINO

#ifdef ARDUINO

#include "lvglDrivers.h"
#include "images.h"
#include <chrono>
#include <Servo.h>

#define TIMECHRONO 5000

Servo barriere;

const int PIN_IRE = PA0;       // Broche analogique entree du capteur IR
const int PIN_IRS = PB9;       // Broche analogique sortie du capteur IR

// Labels persistants utilisés par le système Arduino
static lv_obj_t * label_parking = NULL;
static lv_obj_t * label_status = NULL;
static lv_obj_t * btn_ouvrir = NULL;
static lv_obj_t * btn_fermer = NULL;
static lv_obj_t * btn_ticket = NULL;
static lv_obj_t * img_barriere = NULL;

bool ticket_ok = false; // false = fermée, true = ouverte

// Variables pour le chrono de fermeture automatique
using Clock = std::chrono::steady_clock;
static Clock::time_point barriere_open_start;
static bool barriere_ouverte_flag = false;

// Evenement lors d'appui sur boutons
static void event_handler(lv_event_t * e)
{
    lv_event_code_t code = lv_event_get_code(e);
    lv_obj_t * target = (lv_obj_t *) lv_event_get_target(e);

    if(code == LV_EVENT_CLICKED) {
      if (target == btn_ouvrir) {
        lv_obj_add_state(btn_ouvrir, LV_STATE_DISABLED);
        lv_obj_clear_state(btn_fermer, LV_STATE_DISABLED);
        barriere.write(60); // Ouvrir la barrière
        lv_image_set_src(img_barriere, &barriere_ouverte);  // Afficher image ouverte
        // Démarrer le chrono de fermeture automatique
        barriere_ouverte_flag = true;
        barriere_open_start = Clock::now();
        Serial.println("btn_ouvrir");
      }

      else if (target == btn_fermer) {
        lv_obj_add_state(btn_fermer, LV_STATE_DISABLED);
        lv_obj_clear_state(btn_ouvrir, LV_STATE_DISABLED);
        barriere.write(140); // Fermer la barrière
        lv_image_set_src(img_barriere, &barriere_fermee);   // Afficher image fermée
        // Annuler le chrono si fermeture manuelle
        barriere_ouverte_flag = false;
        Serial.println("btn_fermer");
      }

      else if (target == btn_ticket) {
        ticket_ok = true;
        lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
        Serial.println("btn_ticket");
      }

    }
    else if(code == LV_EVENT_VALUE_CHANGED) {
        LV_LOG_USER("Toggled");
    }
}

// Initialisation des pins et du terminal
void init_system() {
  Serial.begin(115200);
  pinMode(PIN_IRE, INPUT);
  pinMode(PIN_IRS, INPUT);
  barriere.attach(PIN_A5);
}

// Initialisation de l'affichage du lvgl
void init_affichage()
{
  barriere.write(140);

  // label fixe en haut : nom du parking
  label_parking = lv_label_create(lv_screen_active());
  lv_label_set_text(label_parking, "Parking de l'IUT");
  lv_obj_align(label_parking, LV_ALIGN_TOP_MID, 1, 6);
  lv_obj_set_style_text_font(label_parking, &lv_font_montserrat_20, 0);
  lv_obj_set_style_text_color(label_parking, lv_color_hex(0x1A73E8), 0);

  // Image de la barrière
  img_barriere = lv_image_create(lv_screen_active());
  lv_image_set_src(img_barriere, &barriere_fermee);
  lv_obj_align(img_barriere, LV_ALIGN_CENTER, 0, -50);
  lv_obj_set_size(img_barriere, 200, 150);

  // COLONNE GAUCHE : Ouvrir / Fermer la barrière
  btn_ouvrir = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn_ouvrir, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(btn_ouvrir, 160, 50);
  lv_obj_align(btn_ouvrir, LV_ALIGN_BOTTOM_LEFT, 20, -80);
  lv_obj_remove_flag(btn_ouvrir, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(btn_ouvrir, lv_color_hex(0xB3E137), 0); // vert kaki

  lv_obj_t * labelbutton1 = lv_label_create(btn_ouvrir);
  lv_label_set_text(labelbutton1, "Ouverture barriere");
  lv_obj_center(labelbutton1);

  btn_fermer = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn_fermer, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(btn_fermer, 160, 50);
  lv_obj_align(btn_fermer, LV_ALIGN_BOTTOM_LEFT, 20, -20);
  lv_obj_remove_flag(btn_fermer, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(btn_fermer, lv_color_hex(0xB3E137), 0); // vert kaki

  lv_obj_t * labelbutton2 = lv_label_create(btn_fermer);
  lv_label_set_text(labelbutton2, "Fermeture barriere");
  lv_obj_center(labelbutton2);

  lv_obj_add_state(btn_fermer, LV_STATE_DISABLED); // bouton inutilisable au début

  // COLONNE DROITE : Prendre le ticket
  btn_ticket = lv_button_create(lv_screen_active());
  lv_obj_add_event_cb(btn_ticket, event_handler, LV_EVENT_ALL, NULL);
  lv_obj_set_size(btn_ticket, 160, 110);
  lv_obj_align(btn_ticket, LV_ALIGN_BOTTOM_RIGHT, -20, -20);
  lv_obj_remove_flag(btn_ticket, LV_OBJ_FLAG_PRESS_LOCK);
  lv_obj_set_style_bg_color(btn_ticket, lv_color_hex(0xE16A5B), 0); // rose saumon

  lv_obj_t * labelbutton3 = lv_label_create(btn_ticket);
  lv_label_set_text(labelbutton3, "Prendre\nun ticket");
  lv_obj_set_style_text_align(labelbutton3, LV_TEXT_ALIGN_CENTER, 0);
  lv_obj_center(labelbutton3);

  lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
}

// Fermeture automatique de la barrière après TIMECHRONO ms
void gestion_chrono() {
  if (barriere_ouverte_flag) {
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
        Clock::now() - barriere_open_start).count();

    if (elapsed >= TIMECHRONO) {
      barriere.write(140); // Fermer la barrière
      lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
      lv_image_set_src(img_barriere, &barriere_fermee);
      lv_obj_add_state(btn_fermer, LV_STATE_DISABLED);
      lv_obj_clear_state(btn_ouvrir, LV_STATE_DISABLED);
      barriere_ouverte_flag = false;
      Serial.println("Fermeture automatique apres chrono");
    }
  }
}

// Lecture des capteurs et gestion du parking (image + barrière)
// capteurs actifs à 0L
int gestion_capteurs() {
  int valE = digitalRead(PIN_IRE);
  int valS = digitalRead(PIN_IRS);

  Serial.printf("Entree: %d, Sortie: %d\n", valE, valS);

  if (valE == LOW && valS == LOW) {
    // Les deux capteurs détectent en même temps, on ne fait rien
    return 1;
  }
  else if (valE == LOW && ticket_ok == false) {
    // Véhicule à l'entrée sans ticket : activer le bouton ticket
    lv_obj_clear_state(btn_ticket, LV_STATE_DISABLED);
    return 1;
  }
  else if (valE == LOW && ticket_ok == true) {
    // Véhicule à l'entrée avec ticket valide : ouvrir la barrière
    lv_image_set_src(img_barriere, &barriere_ouverte);
    ticket_ok = false; // Réinitialiser le ticket
    barriere.write(60); // Ouvrir la barrière
    // Démarrer le chrono de fermeture automatique
    barriere_ouverte_flag = true;
    barriere_open_start = Clock::now();
    Serial.println("Barriere ouverte - chrono demarre");
    return 1;
  }
  else if (valS == LOW) {
    // Véhicule à la sortie : fermer la barrière et annuler le chrono
    lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
    barriere.write(140); // Fermer la barrière
    lv_image_set_src(img_barriere, &barriere_fermee);
    barriere_ouverte_flag = false; // Annuler le chrono
    Serial.println("Sortie detectee - barriere fermee");
    return 1;
  }
  else {
    lv_obj_add_state(btn_ticket, LV_STATE_DISABLED);
    return 0;
  }
}

void mySetup()
{
  init_system();
  init_affichage();

  xTaskCreate(myTask, "gestion_capteurs", 1024, NULL, 1, NULL);
}

void loop()
{
  // Inactif (pour mise en veille du processeur)
}

void myTask(void *pvParameters)
{
  TickType_t xLastWakeTime;
  xLastWakeTime = xTaskGetTickCount();

  while (1)
  {
    gestion_capteurs();
    gestion_chrono(); // Vérifier si la barrière doit se fermer automatiquement

    // Tâche exécutée toutes les 200ms
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