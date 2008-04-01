//**************************************************************************
//   Copyright (C) 2004, 2005 by Petri Damstén
//   petri.damsten@iki.fi
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the
//   Free Software Foundation, Inc.,
//   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
//**************************************************************************
#include "conversion.h"
#include "videoobject.h"
#include <qcheckbox.h>
#include <qslider.h>
#include <qcombobox.h>

Conversion::Conversion(QWidget *parent, const char *name)
  : ConversionLayout(parent, name)
{
  videoBitrateSlider->setMaxValue(9300);
  videoBitrateSlider->setMinValue(1000);
  audioBitrateSlider->setMaxValue(384);
  audioBitrateSlider->setMinValue(128);
}

Conversion::~Conversion()
{
}

void Conversion::getData(ConversionParams& params) const
{
  params.m_pass = twoPassCheckBox->isChecked() ? 2 : 1;
  params.m_videoBitrate = videoBitrateSlider->value();
  params.m_audioType = audioTypeComboBox->currentItem();
  params.m_audioBitrate = audioBitrateSlider->value();
}

void Conversion::setData(const ConversionParams& params)
{
  twoPassCheckBox->setChecked(params.m_pass == 2);
  videoBitrateSlider->setValue(params.m_videoBitrate);
  audioTypeComboBox->setCurrentItem(params.m_audioType);
  audioBitrateSlider->setValue(params.m_audioBitrate);
}


