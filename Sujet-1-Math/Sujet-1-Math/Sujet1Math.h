#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_Sujet1Math.h"

class Sujet1Math : public QMainWindow
{
    Q_OBJECT

public:
    Sujet1Math(QWidget *parent = nullptr);
    ~Sujet1Math();

private:
    Ui::Sujet1MathClass ui;
};

