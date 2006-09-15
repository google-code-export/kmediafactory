//Added by qt3to4:
#include <Q3PtrList>
/***************************************************************************
                           postfilter.h -
		Wrapper for xine's postprocessing filters.
                             -------------------
    begin                : Son Dez 7 2003
    revision             : $Revision: 1.3 $
    last modified        : $Date: 2005/02/13 11:46:19 $ by $Author: juergenk $
    copyright            : (C) 2003-2005 by Juergen Kofler, Miguel Freitas
    email                : kaffeine@gmx.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef POSTFILTER_H
#define POSTFILTER_H

/* forward declaration */
class QWidget;
class QObject;
class QString;
class Q3GroupBox;
class Q3TextEdit;
class KPushButton;

#include <klineedit.h>
#include <knuminput.h>
#include <kcombobox.h>
//#include <kdialogbase.h>

#include <qcheckbox.h>

#include <xine.h>


class PostFilterParameter : public QObject
{
  Q_OBJECT
public:
  PostFilterParameter(const QString&, int offset, QWidget* parent)
                      : QObject(parent), m_offset(offset)
  {}
  ~PostFilterParameter() {};

  virtual void setValue( const QString& ) = 0;
  virtual QString getValue() = 0;
  virtual QWidget *getWidget() = 0;

protected:
  int m_offset;
};


class PostFilterParameterInt : public PostFilterParameter
{
  Q_OBJECT
public:
  PostFilterParameterInt(const QString& name, int offset, int value, int min, int max, QWidget* parent);
  ~PostFilterParameterInt() {};

  void setValue( const QString &value )
    { int i = value.toInt(); m_numInput->setValue(i); slotIntValue(i); }
  QString getValue()
    { QString s; s.sprintf("%d", m_numInput->value()); return s; }
  QWidget *getWidget() { return m_numInput; }

signals:
  void signalIntValue( int, int );

public slots:
  void slotIntValue(int val) { emit signalIntValue(m_offset, val); }

private:
  KIntNumInput* m_numInput;
};


class PostFilterParameterDouble : public PostFilterParameter
{
  Q_OBJECT
public:
  PostFilterParameterDouble(const QString& name, int offset, double value, double min, double max, QWidget* parent);
  ~PostFilterParameterDouble() {};

  void setValue( const QString &value )
    { double d = value.toDouble(); m_numInput->setValue(d); slotDoubleValue(d); }
  QString getValue()
    { QString s; s.sprintf("%lf",m_numInput->value()); return s; }
  QWidget *getWidget() { return m_numInput; }

signals:
  void signalDoubleValue(int, double);

public slots:
  void slotDoubleValue(double val) { emit signalDoubleValue(m_offset, val); }

private:
  KDoubleNumInput* m_numInput;
};


class PostFilterParameterChar : public PostFilterParameter
{
  Q_OBJECT
public:
  PostFilterParameterChar(const QString& name, int offset, char *value, int size, QWidget* parent);
  ~PostFilterParameterChar() {};

  void setValue(const QString &value)
    { m_charInput->setText(value); slotCharValue(value); }
  QString getValue() { return m_charInput->text(); }
  QWidget *getWidget() { return m_charInput; }

signals:
  void signalCharValue(int, const QString&);

public slots:
  void slotCharValue(const QString& val) { emit signalCharValue(m_offset, val); }

private:
  KLineEdit* m_charInput;
};


class PostFilterParameterCombo : public PostFilterParameter
{
  Q_OBJECT
public:
  PostFilterParameterCombo(const QString& name, int offset, int value, char **enums, QWidget* parent);
  ~PostFilterParameterCombo() {};

  void setValue(const QString &value) { m_comboBox->setCurrentItem(value); slotIntValue(m_comboBox->currentIndex()); }
  QString getValue() { return m_comboBox->currentText(); }
  QWidget *getWidget() { return m_comboBox; }

signals:
  void signalIntValue(int, int);

public slots:
  void slotIntValue(int val) { emit signalIntValue(m_offset, val); }

private:
  KComboBox* m_comboBox;
};


class PostFilterParameterBool : public PostFilterParameter
{
  Q_OBJECT
public:
  PostFilterParameterBool(const QString& name, int offset, bool value, QWidget* parent);
  ~PostFilterParameterBool() {};

  void setValue(const QString &value)
    { bool b = (bool)value.toInt(); m_checkBox->setChecked(b); slotBoolValue(b); }
  QString getValue()
    { QString s; s.sprintf("%d",(int)m_checkBox->isChecked()); return s; }
  QWidget *getWidget() { return m_checkBox; }

signals:
  void signalIntValue(int, int);

public slots:
  void slotBoolValue(bool val) { emit signalIntValue(m_offset, (int)val); }

private:
  QCheckBox* m_checkBox;
};


class PostFilterHelp : public KDialog
{
  Q_OBJECT
public:
  PostFilterHelp(QWidget *parent=0, const char *name=0, const QString& text = QString::null);
  ~PostFilterHelp();

private:
  Q3TextEdit *m_textEdit;
};


class PostFilter : public QObject
{
   Q_OBJECT
public:
   PostFilter(const QString& name, xine_t* engine, xine_audio_port_t* audioDriver,
              xine_video_port_t* videoDriver, QWidget *parent);
  ~PostFilter();

  xine_post_in_t* getInput() const;
  xine_post_out_t* getOutput() const;
  void setConfig(const QString &);
  QString getConfig();


signals:
  void signalDeleteMe( PostFilter* me );


private slots:
  void slotDeletePressed() { emit signalDeleteMe(this); }
  void slotApplyIntValue(int offset, int val);
  void slotApplyDoubleValue(int offset, double val);
  void slotApplyCharValue(int offset, const QString& val);
  void slotHelpPressed();

private:
  xine_t* m_xineEngine;
  xine_post_t* m_xinePost;
  xine_post_api_t* m_xinePostAPI;
  xine_post_api_descr_t* m_xinePostDescr;
  xine_post_api_parameter_t* m_xinePostParameter;
  char* m_data;

  Q3GroupBox* m_groupBox;
  QString m_filterName;

  Q3PtrList<PostFilterParameter> m_parameterList;
};

#endif
