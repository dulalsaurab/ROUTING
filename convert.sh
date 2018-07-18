#!/bin/bash

function randint(n) {
  return int(n * rand());
}
{
  delay = 10 + randint(40)"ms";
  sub(/50ms/, delay, $0);
  print;
}
