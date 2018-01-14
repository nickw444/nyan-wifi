#include <ESP8266WiFi.h>

extern "C" {
  #include <user_interface.h>
}

int pkts_pos = 0;
int pkts[13][NUM_AVG];
int pkts_sums[13];

int max_avg[NUM_AVG];
int max_sum = 0;

int last_hop;
int wifi_ch = 0;

struct melody_item {
  uint16_t freqency;
  uint16_t duration;
} melody[MELODY_LENGTH];

#define N_COLORS 6
#define CPRFX "\033[48;5;"
static const char * colors[] = {
  CPRFX"33m", // Light Blue
  CPRFX"33m", // Light Blue
  CPRFX"118m", // Green
  CPRFX"226m", // Yellow
  CPRFX"214m", // Orange
  CPRFX"196m", // Red
};

void promisc_cb(unsigned char*, short unsigned int) {
  pkts[wifi_ch][pkts_pos]++;
}

void wifi_setup() {
  wifi_set_opmode(STATION_MODE);
  wifi_set_channel(1);
  wifi_set_promiscuous_rx_cb(promisc_cb);
  wifi_promiscuous_enable(1);
}

void clear_stats() {
  memset(pkts_sums, 0, sizeof(int) * 13);
  memset(pkts, 0, sizeof(int) * 13 * NUM_AVG);
  memset(max_avg, 0, sizeof(int) * NUM_AVG);
}

void load_melody() {
  spi_flash_read(MELODY_ADDR, (uint32_t*)melody, sizeof(melody));
}

void setup() {
  Serial.begin(115200);
  last_hop = millis();
  clear_stats();
  wifi_setup();
  load_melody();
}

void process_data() {
  int max = 0;
  int pkts_pos_next = (pkts_pos + 1) % NUM_AVG;
  for (int i = 0; i < 13; i++) {
    int v = pkts[i][pkts_pos];
    pkts_sums[i] += v - pkts[i][pkts_pos_next];
    pkts[i][pkts_pos_next] = 0;
    max = max > v ? max : v;
  }

  max_avg[pkts_pos] = max;
  max_sum += max - max_avg[pkts_pos_next];
  pkts_pos = pkts_pos_next;
}

int melody_pos = 0;
int next_tone_time = 0;

void melody_next() {
  int now = millis();
  if (now > next_tone_time) {
    struct melody_item entry = melody[melody_pos];
    int note_duration = (1000/entry.duration) * 1.2;
    int pause_between_duration = note_duration * .8;
    tone(AUDIO_PIN, entry.freqency, note_duration);
    melody_pos = (melody_pos + 1) % MELODY_LENGTH;
    next_tone_time = now + note_duration + pause_between_duration;
  }
}

int frame = 0;
char frame_data[CAT_ROWS][CAT_ROW_SIZE];

void output_stats2() {
  spi_flash_read(CAT_ADDR + (frame * CAT_ROW_SIZE * CAT_ROWS), (uint32_t*)frame_data, sizeof(frame_data));
  Serial.printf("\033[0;0H");
  for (int i = 0; i < CAT_ROWS; i++) {
    melody_next();
    if (i >= 3 && i < 16) {
      int v = pkts_sums[i - 3];
      v = v > max_sum ? max_sum : v;
      int idx = ceil(v / (float)max_sum * (N_COLORS - 1));
      Serial.printf("%s", colors[idx]);
    }
    Serial.printf("%30s%s\n", "", frame_data[i]);
  }
  frame = (frame + 1) % CAT_FRAMES;
}

void loop() {
  melody_next();
  int now = millis();
  if (last_hop + 40 < now) {
    wifi_ch = (wifi_ch + 1) % 13;
    wifi_set_channel(wifi_ch + 1);
    last_hop = now;
    if (wifi_ch == 0) {
      process_data();
    }
    if (wifi_ch % 4 == 0 && max_sum > 0) {
      output_stats2();
    }
  }
}
