byte *char_to_byte(char *ch, int len) {
  byte *byx;
  byx = (byte *)malloc(len);

  for (int i = 0; i < len; i++) {
    byx[i] = ch[i];
  }
  return byx;
}
