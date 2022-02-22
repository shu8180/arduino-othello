/*
 Arduino pro mini互換でのオセロプログラム
 2022/2/19
*/

// オセロ石状態定義
#define BLANK 0
#define BLACK 1
#define WHITE 2
#define MAX  3

// ゲームモード定義
// true:対コンピュータモード、false:2Playerモード
#define VS_COMPUTER_MODE true
#define VS_2P_MODE false

// スピーカーピンアサイン定義
#define SPEAKER_PIN 11

// 音の長さ(msec)を定義する(BPM=120)
#define BPM 120
#define Bt2 1000 // 2分音符（60*1000/BPM*2）
#define Bt4 500 // 4分音符（60*1000/120*1）
#define Bt8f 375 // 付点8分音符（(60*1000/BPM*(3/4))）
#define Bt8 250 // 8分音符（(60*1000/BPM*(1/2))）
#define Bt16 125 // 16分音符（(60*1000/BPM*(1/4))）
#define Bt32 63 // （(60*1000/BPM*(1/8))）
//音程を定義する
#define A5 880 //ラ
#define G5 784 //ソ
#define Fs5 740 //ファ＃
#define F5 698 //ファ
#define E5 659 //ミ
#define Ds5 622 //レ＃
#define D5 587 //レ
#define Cs5 554 //ド＃
#define C5 523 //ド
#define B4 494 //シ
#define As4 466 //ラ＃
#define A4 440 //ラ
#define Gs4 415 //ソ＃
#define G4 392 //ソ
#define Fs4 370 //ファ＃
#define F4 350 //ファ
#define E4 330 //ミ
#define Ds4 311 //レ＃
#define D4 294 //レ
#define Cs4 277 //ド＃
#define C4 262 //ド

// ボタンのピンアサイン定義
#define BTN_UP 17
#define BTN_DOWN 16
#define BTN_RIGHT 15
#define BTN_LEFT 18
#define BTN_A 14
// ボタン状態定義
#define BTN_ON LOW
#define BTN_OFF HIGH

// 手番表示LEDピンアサイン定義
#define RED_LED 12
#define GREEN_LED 19
// LED状態定義
#define LED_ON LOW
#define LED_OFF HIGH

// マトリクスLEDピンアサイン定義
static const int LED_DECODER_PIN[4] = { 13, 10, 9, 8 };
static const int LED_COM_CATHODE_PIN[8] = { 0, 1, 2, 3, 6, 7, 4, 5 };
static const int LED_RED_GREEN[16] = { 4, 5, 6, 7, 3, 2, 1, 0, 10, 8, 9, 12, 13, 14, 11, 15 };

// マトリクスLED表示用バッファ定義
byte led_write_buffer_red[8] = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000
};
byte led_write_buffer_green[8] = {
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000,
      0b00000000
};

static const bool vs_com_data[8][34] = {
  { 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};
static const bool vs_2p_data[8][28] = {
  { 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 1, 1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
};

// 石が無い　　　0
// 黒石がある　　1
// 白石がある　　2
// 盤外　　　　　-1
int board[10][10];
static int initdata[10][10] = {
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
  {-1,  0,  0,  0,  0,  0,  0,  0,  0, -1}, 
  {-1,  0,  0,  0,  0,  0,  0,  0,  0, -1}, 
  {-1,  0,  0,  0,  0,  0,  0,  0,  0, -1}, 
  {-1,  0,  0,  0,  2,  1,  0,  0,  0, -1}, 
  {-1,  0,  0,  0,  1,  2,  0,  0,  0, -1}, 
  {-1,  0,  0,  0,  0,  0,  0,  0,  0, -1}, 
  {-1,  0,  0,  0,  0,  0,  0,  0,  0, -1}, 
  {-1,  0,  0,  0,  0,  0,  0,  0,  0, -1}, 
  {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

// マトリクスLED用74HC154ラインデコーダへの出力処理
void led_4to16_line_decoder_write(byte data){
  for (byte led_cnt = 0; led_cnt < 4; ++led_cnt )
  {
    digitalWrite( LED_DECODER_PIN[led_cnt], ((data >> led_cnt) & 0x01) );
  }
}

// マトリクスLEDコモン側（カソード側）の電流吸い込み処理
void led_com_write(byte data){
  int not_data = ~data;
  for (byte led_cnt = 0; led_cnt < 8; ++led_cnt )
  {
    digitalWrite( LED_COM_CATHODE_PIN[led_cnt], ((not_data >> led_cnt) & 0x01) );
  }
}

// マトリクス制御用タイマー割り込み処理
byte led_timer_cnt = 0;
int my_led_cnt = 0;
ISR (TIMER1_COMPA_vect) {
  led_com_write(0);
  led_4to16_line_decoder_write(LED_RED_GREEN[led_timer_cnt]);
  delayMicroseconds(100);

  if(led_timer_cnt < 8){
    led_com_write(led_write_buffer_red[led_timer_cnt]);
  } else {
    led_com_write(led_write_buffer_green[led_timer_cnt - 8]);
  }

  led_timer_cnt++;
  if(led_timer_cnt > 15){
    led_timer_cnt = 0;
  }
  my_led_cnt++;
  if(my_led_cnt > 2000){
    my_led_cnt = 0;
  }

}

// LED用タイマー（タイマー１）初期化処理
void set_timer() {
  TCCR1A  = 0;
  TCCR1B  = 0;
  TCCR1B |= (1 << WGM12) | (1 << CS12);  //CTCmode //prescaler to 256
  OCR1A   = 40-1;
  TIMSK1 |= (1 << OCIE1A);
}

// LED関連初期化処理
void init_led(){
  // マトリクスLED用ピンの初期化
  for ( int i = 0; i < 4; ++i )
  {
    pinMode( LED_DECODER_PIN[i], OUTPUT );
    digitalWrite( LED_DECODER_PIN[i], LOW );
  }
  for ( int i = 0; i < 8; ++i )
  {
    pinMode( LED_COM_CATHODE_PIN[i], OUTPUT );
    digitalWrite( LED_COM_CATHODE_PIN[i], HIGH );
  }

  pinMode( RED_LED, OUTPUT );
  pinMode( GREEN_LED, OUTPUT );
  digitalWrite( RED_LED, LED_OFF );
  digitalWrite( GREEN_LED, LED_OFF );
}

// 入力ボタン初期化処理
void init_button(){
  // ボタンを入力モードに設定
  pinMode(BTN_UP,INPUT);
  pinMode(BTN_DOWN,INPUT);
  pinMode(BTN_LEFT,INPUT);
  pinMode(BTN_RIGHT,INPUT);
  pinMode(BTN_A,INPUT);
}

// 初期化処理
void setup() {
  // 入力ボタン初期化処理
  init_button();
  // LED関連初期化処理
  init_led();
  set_timer();
}

// ボタン読み込み処理
bool btn_up_value = 0;
bool btn_up_prev_status = BTN_OFF;
bool btn_down_value = 0;
bool btn_down_prev_status = BTN_OFF;
bool btn_left_value = 0;
bool btn_left_prev_status = BTN_OFF;
bool btn_right_value = 0;
bool btn_right_prev_status = BTN_OFF;
bool btn_a_value = 0;
bool btn_a_prev_status = BTN_OFF;

void btn_read(){
  btn_up_value = digitalRead( BTN_UP );
//  if((btn_up_value == BTN_ON) & (btn_up_prev_status == BTN_ON)){
//      btn_up_value = BTN_OFF;
//  } else {
//    btn_up_prev_status = btn_up_value;
//  }
  btn_down_value = digitalRead( BTN_DOWN );
//  if((btn_down_value == BTN_ON) & (btn_down_prev_status == BTN_ON)){
//      btn_down_value = BTN_OFF;
//  } else {
//    btn_down_prev_status = btn_down_value;
//  }
  btn_left_value = digitalRead( BTN_LEFT );
//  if((btn_left_value == BTN_ON) & (btn_left_prev_status == BTN_ON)){
//      btn_left_value = BTN_OFF;
//  } else {
//    btn_left_prev_status = btn_left_value;
//  }
  btn_right_value = digitalRead( BTN_RIGHT );
//  if((btn_right_value == BTN_ON) & (btn_right_prev_status == BTN_ON)){
//      btn_right_value = BTN_OFF;
//  } else {
//    btn_right_prev_status = btn_right_value;
//  }
  btn_a_value = digitalRead( BTN_A );
  if((btn_a_value == BTN_ON) & (btn_a_prev_status == BTN_ON)){
      btn_a_value = BTN_OFF;
  } else {
    btn_a_prev_status = btn_a_value;
  }
}

// 自手移動処理
int my_x = 0;
int my_y = 0;
void my_move(){
  if (btn_up_value == BTN_ON) {
    if(my_y > 0) {
      my_y--;
      tone(SPEAKER_PIN, 1960, 100);
    } else {
      tone(SPEAKER_PIN, 100, 100);
    }
  }
  if (btn_down_value == BTN_ON) {
    if(my_y < 7) {
      my_y++;
      tone(SPEAKER_PIN, 1960, 100);
    } else {
      tone(SPEAKER_PIN, 100, 100);
    }
  }
  if (btn_left_value == BTN_ON) {
    if(my_x > 0) {
      my_x--;
      tone(SPEAKER_PIN, 1960, 100);
    } else {
      tone(SPEAKER_PIN, 100, 100);
    }
  }
  if (btn_right_value == BTN_ON) {
    if(my_x < 7) {
      my_x++;
      tone(SPEAKER_PIN, 1960, 100);
    } else {
      tone(SPEAKER_PIN, 100, 100);
    }
  }
}

// オセロボード配列をマトリクスLEDバッファに変換し書き込み
void convert_board_to_write_buffer(){
  memset(led_write_buffer_red, 0, 8);
  memset(led_write_buffer_green, 0, 8);
  for(int col=1; col<9; col++){
    for(int row=1; row<9; row++){
      if(board[col][row] == BLACK){
        led_write_buffer_red[col-1] |= 1 << 7- (row-1);
      } else if(board[col][row] == WHITE){
        led_write_buffer_green[col-1] |= 1 << 7- (row-1);
      }
    }
  }
} 

// 自手の位置をマトリクスLEDバッファに変換し書き込み
void write_my_led_to_write_buffer(int player){
  if(my_led_cnt < 1000){
    if(player == BLACK){
      led_write_buffer_red[my_y] |= 1 << 7 - my_x;
    } else {
      led_write_buffer_green[my_y] |= 1 << 7 - my_x;
    }
  } else {
    byte mask_data = 1 << 7 - my_x;
    mask_data = ~mask_data;
    
    led_write_buffer_red[my_y] &= mask_data;
    led_write_buffer_green[my_y] &= mask_data;
  }
}

// 対戦相手取得処理
int aite(int player){
  return MAX - player;
}

int count_turn_over(int player, int p, int q, int d, int e)
{
    int ci;
    
    for (ci = 1; board[p+ci*d][q+ci*e] == aite(player); ci++) {};        

    if (board[p+ci*d][q+ci*e] == player) {                             
        return ci-1;   
    } else {
        return 0;   
    }
}

// 石配置可否判定処理
int is_legal_move(int player, int p, int q)
{
    if (p < 1 || p > 8 || q < 1 || q > 8) return 0;
    if (board[p][q] != 0) return 0;
    if (count_turn_over(player, p, q, -1,  0)) return 1;  /* 上 */
    if (count_turn_over(player, p, q,  1,  0)) return 1;  /* 下 */
    if (count_turn_over(player, p, q,  0, -1)) return 1;  /* 左 */
    if (count_turn_over(player, p, q,  0,  1)) return 1;  /* 右 */
    if (count_turn_over(player, p, q, -1, -1)) return 1;  /* 左上 */
    if (count_turn_over(player, p, q, -1,  1)) return 1;  /* 右上 */
    if (count_turn_over(player, p, q,  1, -1)) return 1;  /* 左下 */
    if (count_turn_over(player, p, q,  1,  1)) return 1;  /* 右下 */
    return 0;
}

// 石配置可能箇所があるか判定する処理
int exist_legal_move(int player)
{
  int li, lj;
  
  for (li = 1; li <= 8; li++) {
    for (lj = 1; lj <= 8; lj++) {
      if (is_legal_move(player, li, lj)) return 1;
    }
  }
  return 0;
}

// 石の配置と裏返しを行う処理
void set_and_turn_over(int player, int p, int q)
{
    int count, d, e, si;
  
    board[p][q] = player; /* 石を置く */
    convert_board_to_write_buffer();
    tone(SPEAKER_PIN, 880, 50);
    for (d = -1; d <= 1; d++) {      /* 上下方向 */
        for (e = -1; e <= 1; e++) {  /* 左右方向 */
            if (d == 0 && e == 0) continue; 
            count = count_turn_over(player, p, q, d, e);
            for (si = 1; si <= count; si++) {
                board[p+si*d][q+si*e] = player; /* 裏返す */
                convert_board_to_write_buffer();
                tone(SPEAKER_PIN, 2000, 50);
                delay(300);
            }
        }
    }
}

int count_stone()
{
    int i, j, c1, c2, c1_buff, c2_buff ;
  
    c1 = 0;
    c2 = 0;
    for (i = 1; i <= 8; i++) {
        for (j = 1; j <= 8; j++) {
            switch (board[i][j]) {
                case BLACK : 
                    (c1)++; 
                    break;
                case WHITE : 
                    (c2)++; 
                    break;
            }
        }
    }
    c1_buff = c1;
    c2_buff = c2;

    for (i = 1; i <= 8; i++) {
        for (j = 1; j <= 8; j++) {
          if(c1 > 0){
            board[i][j] = BLACK;
            c1--;
          } else if(c2 > 0) {
            board[i][j] = WHITE;
            c2--;
          } else {
            board[i][j] = BLANK;
          }
        }
    }
    convert_board_to_write_buffer();

    if(c1_buff > c2_buff){
      return BLACK;
    } else if (c1_buff < c2_buff){
      return WHITE;
    } else {
      return MAX;
    }
}

// 勝利した時のBGM
void bgm_win (){
  tone(SPEAKER_PIN, C5, Bt16);
  delay(Bt8);
  tone(SPEAKER_PIN, C5, Bt16);
  delay(Bt8);
  tone(SPEAKER_PIN, C5, Bt16);
  delay(Bt8);
  tone(SPEAKER_PIN, C5, Bt4);
  delay(Bt4);
  tone(SPEAKER_PIN, Gs4, Bt4);
  delay(Bt4);
  tone(SPEAKER_PIN, As4, Bt4);
  delay(Bt4);
  tone(SPEAKER_PIN, C5, Bt16);
  delay(Bt8);
  delay(Bt16);
  tone(SPEAKER_PIN, As4, Bt8);
  delay(Bt8);
  tone(SPEAKER_PIN, C5, Bt4);
  delay(Bt4);
}

// 負けた時のBGM
void bgm_lose (){
  tone(SPEAKER_PIN, A5, Bt8);
  delay(Bt8);
  tone(SPEAKER_PIN, G5, Bt8);
  delay(Bt8);
  tone(SPEAKER_PIN, A5, Bt4);
  delay(Bt4);
  tone(SPEAKER_PIN, G5, Bt8);
  delay(Bt8);
  tone(SPEAKER_PIN, F5, Bt8);
  delay(Bt8);
  tone(SPEAKER_PIN, E5, Bt8);
  delay(Bt8);
  tone(SPEAKER_PIN, D5, Bt8);
  delay(Bt8);
  tone(SPEAKER_PIN, Cs5, Bt4);
  delay(Bt4);
  tone(SPEAKER_PIN, D5, Bt2);
  delay(Bt2);
}

// コンピュータ手番処理
int computer(int player){
  int rand_x;
  int rand_y;
  int ai_cnt = 0;
  int ai_i = 1, ai_j = 1;
  while(1){
    ai_cnt++;
    if(ai_cnt > 1000){
      for (ai_i = 1; ai_i <= 8; ai_i++) {
        for (ai_j = 1; ai_j <= 8; ai_j++) {
          if(board[ai_i][ai_j] == BLANK){
            if(is_legal_move(player, ai_i, ai_j) == 1){
              rand_x = ai_j;
              rand_y = ai_i;
              break;
            }
          }
        }
      }
      if ((ai_i >= 8) & (ai_j >= 8)) {
        player = aite(player);
        break;
      }
    } else {
      rand_x = random(1,8);
      rand_y = random(1,8);
    }
    if(is_legal_move(player, rand_y, rand_x) == 1){
      set_and_turn_over(player, rand_y, rand_x);
      player = aite(player);
      break;
    }
  }
  return player;
}

// スクロール表示用データをマトリクスLED REDバッファに変換し書き込み
int scroll_cnt = 0;
void convert_scroll_data_to_write_buffer(){
    if(my_y == 0){
      for(int col=0; col < 8; col++){
        led_write_buffer_red[col] = 0;
        for(int row=0; row < 8; row++){
          led_write_buffer_red[col] |= vs_com_data[col][row + scroll_cnt] << (7-row);
        }
      }
      scroll_cnt++;
      if(scroll_cnt > (34-8)){
        scroll_cnt = 0;
      }
    } else {
      for(int col=0; col < 8; col++){
        led_write_buffer_red[col] = 0;
        for(int row=0; row < 8; row++){
          led_write_buffer_red[col] |= vs_2p_data[col][row + scroll_cnt] << (7-row);
        }
      }
      scroll_cnt++;
      if(scroll_cnt > (28-8)){
        scroll_cnt = 0;
      }
    }
} 

bool game_mode_select_window(){
  my_y = 0;
  scroll_cnt = 0;
  // 画面バッファ初期化
  memset(led_write_buffer_red, 0, 8);
  memset(led_write_buffer_green, 0, 8);

  while(1){
    btn_read();
    if (btn_up_value == BTN_ON) {
      if(my_y > 0) {
        my_y--;
        tone(SPEAKER_PIN, 1960, 100);
        scroll_cnt = 0;
      } else {
        tone(SPEAKER_PIN, 100, 100);
      }
    }
    if (btn_down_value == BTN_ON) {
      if(my_y <= 1) {
        my_y++;
        tone(SPEAKER_PIN, 1960, 100);
        scroll_cnt = 0;
      } else {
        tone(SPEAKER_PIN, 100, 100);
      }
    }
    if(btn_a_value == BTN_ON){
      tone(SPEAKER_PIN, 1960, 100);
      // true:vs コンピュータモード、false:vs 2Pモード
      bool game_mode = VS_COMPUTER_MODE;
      if(my_y > 0){
        // 2Pモード
        game_mode = VS_2P_MODE;
      }
      my_y = 0;
      return game_mode; 
    }

    convert_scroll_data_to_write_buffer();
    delay(300);
  }
} 

void player_led_write(int player){
  if(player == BLACK){
    digitalWrite( RED_LED, LED_ON );
    digitalWrite( GREEN_LED, LED_OFF );
  } else {
    digitalWrite( RED_LED, LED_OFF );
    digitalWrite( GREEN_LED, LED_ON );            
  }
}

void loop() {
  int player;
  bool game_mode = VS_COMPUTER_MODE;
  // 盤の初期化
  memcpy(board, initdata, sizeof(initdata));

  // ゲームモード選択画面
  game_mode = game_mode_select_window();

  player = BLACK;
  digitalWrite( RED_LED, LED_ON );
  digitalWrite( GREEN_LED, LED_OFF );
  while (1) {
    if (!exist_legal_move(player)) {
      player = aite(player);
      player_led_write(player);
      if (!exist_legal_move(player)) {
          break;
      }
    }

    while (1) {
      convert_board_to_write_buffer();
      btn_read();
      my_move();
      write_my_led_to_write_buffer(player);

      if (game_mode == VS_COMPUTER_MODE) {
        if(player == WHITE){
          delay(1000);
          player = computer(player);
          player_led_write(player);
          break;
        }
      }

      if(btn_a_value == BTN_ON){
        if(is_legal_move(player, my_y+1, my_x+1) == 1){
          set_and_turn_over(player, my_y+1, my_x+1);
          player = aite(player);
          player_led_write(player);
          break;
        } else {
          tone(SPEAKER_PIN, 100, 100);
        }
      }
      delay(200);
    }
  }

  int ret = count_stone();
  if(ret == BLACK){
    digitalWrite( RED_LED, LED_ON );
    digitalWrite( GREEN_LED, LED_OFF );
    bgm_win();
  } else if(ret == WHITE) {
    digitalWrite( RED_LED, LED_OFF );
    digitalWrite( GREEN_LED, LED_ON );
    bgm_lose();
  } else {
    digitalWrite( RED_LED, LED_ON );
    digitalWrite( GREEN_LED, LED_ON );
    bgm_lose();
  }

  while (1) {
    btn_read();
    if(btn_a_value == BTN_ON){
      break;
    }
  }
}
