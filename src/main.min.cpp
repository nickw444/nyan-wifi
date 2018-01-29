#include <ESP8266WiFi.h>
extern "C" {
#include <user_interface.h>
}
int ppos,mxsum,lsthop,wch,melpos,nxttone,frm,pkts[13][NUM_AVG],sums[13],
    mxav[NUM_AVG],frm_data[CAT_ROWS][CAT_ROW_SIZE];

struct melitm {
uint16_t frq;
uint16_t dur;
} mel[MELODY_LENGTH];

#define A "\033[48;5;"
static char * colors[] = {A"33m",A"33m",A"118m",A"226m",A"214m",A"196m"};

void cb(unsigned char*, short unsigned int) {
pkts[wch][ppos]++;
}
void wifi_setup(){
wifi_set_opmode(1);
wifi_set_channel(1);
wifi_set_promiscuous_rx_cb(cb);
wifi_promiscuous_enable(1);
}
void load_mel(){
spi_flash_read(MELODY_ADDR,(uint32_t*)mel,sizeof(mel));
}
void setup(){
Serial.begin(921600);
lsthop=millis();
wifi_setup();
load_mel();
}
void prcss(){
int max = 0;
int nxtpos=(ppos+1)%NUM_AVG;
for(int i=0;i<13;i++) {
int v=pkts[i][ppos];
sums[i]+=v-pkts[i][nxtpos];
pkts[i][nxtpos]=0;
max=max>v?max:v;
}

mxav[ppos]=max;
mxsum+=max-mxav[nxtpos];
ppos=nxtpos;
}
void melnxt(){
int now=millis();
if(now>nxttone){
struct melitm entry=mel[melpos];
int note_dur=(1000/entry.dur)*1.2;
int paus_dur=note_dur*.8;
tone(AUDIO_PIN,entry.frq,note_dur);
melpos=(melpos+1)%MELODY_LENGTH;
nxttone=now+note_dur+paus_dur;
}
}
void prnt(){
spi_flash_read(CAT_ADDR+(frm*CAT_ROW_SIZE*CAT_ROWS),(uint32_t*)frm_data,sizeof(frm_data));
Serial.printf("\033[0;0H");
for(int i=0;i<CAT_ROWS;i++){
melnxt();
if(i>=3&&i<16) {
int v=sums[i-3];
v=v>mxsum?mxsum:v;
int idx=ceil(v/(float)mxsum*5);
Serial.printf("%s",colors[idx]);
}
Serial.printf("%30s%s\n", "",frm_data[i]);
}
frm=(frm+1)%CAT_FRAMES;
}
void loop(){
melnxt();
int now=millis();
if(lsthop+40<now){
wch=(wch+1)%13;
wifi_set_channel(wch + 1);
lsthop=now;
if(wch==0){
prcss();
}
if (wch%4==0&&mxsum>0){
prnt();
}
}
}
