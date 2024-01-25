#pragma once

#include <QtCore/QObject>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
#include <QtGui/QGuiApplication>

class MyQtDialog : public QDialog {
    public:
        MyQtDialog(QWidget* parent = nullptr) : QDialog(parent) {
                QPushButton* button = new QPushButton("Test Button", this);
        }                
};