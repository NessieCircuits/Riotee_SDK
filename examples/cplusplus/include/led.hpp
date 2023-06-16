#ifndef __LED_HPP_
#define __LED_HPP_

class LED {
 public:
  LED(unsigned int pin);
  void on(void);
  void off(void);
  unsigned int _pin;

 private:
};

#endif /* __LED_HPP_ */