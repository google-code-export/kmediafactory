#! /usr/bin/python
# -*- coding: utf-8 -*-
#**************************************************************************
#   Tool for importing dvdauthor XML file to KMediaFactory
#   Copyright (C) 2008 Petri Damsten <damu@iki.fi>
#
#   This program is free software; you can redistribute it and/or modify
#   it under the terms of the GNU General Public License as published by
#   the Free Software Foundation; either version 2 of the License, or
#   (at your option) any later version.
#
#   This program is distributed in the hope that it will be useful,
#   but WITHOUT ANY WARRANTY; without even the implied warranty of
#   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#   GNU General Public License for more details.
#
#   You should have received a copy of the GNU General Public License
#   along with this program; if not, write to the
#   Free Software Foundation, Inc.,
#   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
#**************************************************************************

from kmediafactory import uiInterface, projectInterface
import os
from popen2 import Popen4
import re
from xml.dom import minidom
import glob
import time

FORMAT = '%d.%m.%Y %H:%M'
SUB_LANGUAGE = 'en'
AUDIO_LANGUAGE = 'en'

OFFSET = 0 # -200*60
CHAPTER_FORWARD = 0.5
CAP = 1.0
DURATION = 3.0

class ImportDVDAuthorXMLPlugin:
    def __init__(self):
        uiInterface().addMediaAction('application-x-cd-image', # Icon
                                     'Import DVDAuthor',       # Text
                                     'Ctrl+X',                 # Shortcut
                                     'import_dvdauthor_xml',   # Name
                                     self,                     # Object
                                     'import_xml'              # Method
        )

    def run(self, cmd):
        uiInterface().debug(cmd)
        pipe = Popen4(cmd)
        result = pipe.wait()
        output = pipe.fromchild.read()
        return(result, output.strip())

    def to_float(self, s):
        a = re.findall(ur'(\d+):(\d+):(\d+)\.(\d+)' ,s)
        f = 0.0
        if(len(a) > 0):
            f = int(a[0][0]) * 3600 + int(a[0][1]) * 60 + int(a[0][2]) + \
                float(a[0][3]) / 1000
        return f

    def to_kmf_time(self, f):
        return '%d:%2.2d:%2.2d.%3.3d' % (int(f / 3600), int(f / 60) % 60, \
                int(f) % 60, int(f * 1000) % 1000)

    def to_srt_time(self, f):
        return '%2.2d:%2.2d:%2.2d,%3.3d' % (int(f / 3600), int(f / 60) % 60, \
                int(f) % 60, int(f * 1000) % 1000)

    def import_xml(self):
        xml_files = uiInterface().getOpenFileNames('KMF_DVDAUTHOR_XML', '*.xml|XML files',
                                                   'Select dvdauthor xml file')
        if len(xml_files) > 0:
            f = open(xml_files[0])
            doc = minidom.parse(f)
            f.close()

            # Parse chapters from xml
            chapters = {}
            entries = doc.getElementsByTagName('vob')
            for entry in entries:
                file = entry.getAttribute('file')
                chapters[file] = []
                cells = entry.getAttribute('chapters').split(',')
                for (i, cell) in enumerate(cells):
                    f = self.to_float(cell)
                    if f > 0.0:
                        f += CHAPTER_FORWARD
                    chapters[file].append([f, 0.0])

            # Check if there are dv files from kino editing
            # and get dates from file names
            d = os.path.dirname(xml_files[0])
            dvs = glob.glob(os.path.join(d, '*.[Dd][Vv]'))
            dvs.sort()
            for (i, dv) in enumerate(dvs):
                a = re.findall(ur'(\d+.\d+.\d+_\d+-\d+-\d+)', dv);
                if len(a) > 0 and len(chapters[file]) > i:
                    # For last file only...
                    chapters[file][i][1] = time.mktime(time.strptime(a[0], \
                                                       '%Y.%m.%d_%H-%M-%S')) + OFFSET

            # If we have dates make subtitle file
            sub_xml = ''
            if len(chapters[file]) > 0 and chapters[file][0][1] != 0.0:
                srt = ''
                n = 1
                for (i, chp) in enumerate(chapters[file]):
                    try:
                        next = chapters[i+1][0]
                    except:
                        next = chp[0] + 2*(DURATION + CAP)
                    if next > chp[0] + DURATION + CAP:
                        srt += '%d\n' % n
                        srt += to_srt_time(chp[0] + CAP) + ' --> ' + \
                               to_srt_time(chp[0] + CAP + DURATION) + '\n'
                        srt += time.strftime(FORMAT, time.localtime(chp[1])) + '\n\n'
                        n += 1
                f = open(xml_files[0] + '.srt', 'w')
                f.write(srt)
                f.close()
                sub_xml += '<subtitle language="%s" file="%s" align="64" > \
                            <font size="28" weight="500" name="Arial" /> \
                            </subtitle>' % (SUB_LANGUAGE, xml_files[0] + '.srt')

            # Make xml for KMF
            xml = '<media plugin="KMFImportVideo" object="video"><video>'
            for file in chapters.keys():
                xml += '<file path="%s" />' % file
                for (i, chp) in enumerate(chapters[file]):
                    if chp[1] != 0.0:
                        xml += '<cell chapter="1" start="%s" name="%s" />' \
                                % (self.to_kmf_time(chp[0]), time.strftime(FORMAT, \
                                                             time.localtime(chp[1])))
                    else:
                        xml += '<cell chapter="1" start="%s" />' % self.to_kmf_time(chp[0])
            xml += '<audio language="%s" />' % AUDIO_LANGUAGE

            xml += sub_xml + '</video></media>'
            uiInterface().debug(xml)
            uiInterface().addMediaObjectFromXML(xml)

    def initPlugin(self, projectType):
        uiInterface().setActionEnabled('import_dvdauthor_xml', projectType[:3] == 'DVD')

    def supportedProjectTypes(self):
        return ['DVD-PAL', 'DVD-NTSC']

kmediafactory.registerPlugin(ImportDVDAuthorXMLPlugin())

