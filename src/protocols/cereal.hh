#pragma once

#ifndef CEREAL_H_
#define CEREAL_H_

#include "iostream"
#include "protocol.hh"

class Cereal : public Protocol {
   private:
    void runRead();
    void runWrite();
};

#endif