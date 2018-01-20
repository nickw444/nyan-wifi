#include <Arduino.h>
extern "C" {
  #include <user_interface.h>
}
#define A "\033[48;5;"
#define SPF Serial.printf

int pp = 0,
    ps[13][NA],
    pq[13],
    ma[NA],
    ms = 0,
    lh,
    rr = 0,
    f = 0;

char fd[CR][CRS];

struct m_item {
  uint16_t f;
  uint16_t d;
} m[ML];

static const char * c[] = {
  A"33m", // Light Blue
  A"33m", // Light Blue
  A"118m", // Green
  A"226m", // Yellow
  A"214m", // Orange
  A"196m", // Red
};

void pc(unsigned char*, short unsigned int) {
  ps[rr][pp]++;
}

void ws() {
  wifi_set_opmode(0x01); // 0x01: STATION_MODE
  wifi_set_channel(1);
  wifi_set_promiscuous_rx_cb(pc);
  wifi_promiscuous_enable(1);
}

void cs() {
  memset(pq, 0, sizeof(int) * 13);
  memset(ps, 0, sizeof(int) * 13 * NA);
  memset(ma, 0, sizeof(int) * NA);
}

void lm() {
  spi_flash_read(MA, (uint32_t*)m, sizeof(m));
}

void setup() {
  Serial.begin(921600);
  lh = millis();
  cs();
  ws();
  lm();
}

void pd() {
  int mt = 0;
  int yt = (pp + 1) % NA;
  for (int i = 0; i < 13; i++) {
    int v = ps[i][pp];
    pq[i] += v - ps[i][yt];
    ps[i][yt] = 0;
    mt = mt > v ? mt : v;
  }

  ma[pp] = mt;
  ms += mt - ma[yt];
  pp = yt;
}

int mp = 0;
int ntt = 0;

void mn() {
  int now = millis();
  if (now > ntt) {
    struct m_item et = m[mp];
    int nd = (1000/et.d) * 1.2;
    int py = nd * .8;
    tone(AP, et.f, nd);
    mp = (mp + 1) % ML;
    ntt = now + nd + py;
  }
}

void os() {
  spi_flash_read(CA + (f * CRS * CR), (uint32_t*)fd, sizeof(fd));
  SPF("\033[0;0H");
  for (int i = 0; i < CR; i++) {
    mn();
    if (i >= 3 && i < 16) {
      int v = pq[i - 3];
      v = v > ms ? ms : v;
      int x = ceil(v / (float)ms * 5);
      SPF("%s", c[x]);
    }
    SPF("%30s%s\n", "", fd[i]);
  }
  f = (f + 1) % CF;
}

void loop() {
  mn();
  int n = millis();
  if (lh + 40 < n) {
    rr = (rr + 1) % 13;
    wifi_set_channel(rr + 1);
    lh = n;
    if (rr == 0) {
      pd();
    }
    if (rr % 4 == 0 && ms > 0) {
      os();
    }
  }
}
