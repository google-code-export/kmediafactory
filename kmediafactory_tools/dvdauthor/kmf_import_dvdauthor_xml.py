#! /usr/bin/python
# -*- coding: utf-8 -*-
#
# Tool for importing dvdauthor XML file
# Copyright (C) 2007 by Petri Damstén <damu@iki.fi>
# License: GPL-2
#
# Uses dbus (dbus is used so python-dbus is not needed)
#

import os
from popen2 import Popen4
import re
from xml.dom import minidom
import glob
import time

FORMAT = '%d.%m.%Y %H:%M'
SUB_LANGUAGE = 'fi'
AUDIO_LANGUAGE = 'fi'

OFFSET = -200*60
CHAPTER_FORWARD = 0.5
CAP = 1.0
DURATION = 3.0

def run(cmd):
    #print cmd
    pipe = Popen4(cmd)
    result = pipe.wait()
    output = pipe.fromchild.read()
    return(result, output.strip())

class KMediaFactory:
    def __init__(self):
        self.dbus = ''
        try:
            self.dbus = os.environ['KMF_DBUS']
            #print 'KMF_DBUS: ' + self.dbus
        except:
            pass

    def isRunning(self):
        if self.dbus != '':
            return True
        return False

    def projectDir(self, directory, default):
        result = self.dbus_call('projectDirectory', [directory])
        if result == '':
            return default
        return result

    def addMediaObject(self, xml):
        xml = '<media plugin="KMFImportVideo" object="video">' + \
              xml + '</media>'
        self.dbus_call('addMediaObject', [xml])

    def getOpenFileNames(self, startDir, fileFilter, caption):
        return self.dbus_call('getOpenFileNames',
                [startDir, fileFilter, caption])

    def debug(self, txt):
        self.dbus_call('debug', [str(txt)])

    def progressDialog(self, caption, label, maximum):
        self.dbus_call('progressDialog', [caption, label, maximum])

    def pdlgSetMaximum(self, maximum):
        self.dbus_call('pdlgSetMaximum', [maximum])

    def pdlgSetValue(self, value):
        self.dbus_call('pdlgSetValue', [value])

    def pdlgSetLabel(self, label):
        self.dbus_call('pdlgSetLabel', [label])

    def pdlgShowCancelButton(self, show):
        self.dbus_call('pdlgShowCancelButton', [show])

    def pdlgWasCancelled(self):
        return self.dbus_call('pdlgWasCancelled', [])

    def pdlgClose(self):
        self.dbus_call('pdlgClose', [])

    def message(self, caption, label, msgType):
        self.dbus_call('message', [caption, label, msgType])

    def dbus_call(self, func, args):
        funcResult = ''
        if self.isRunning():
            cmd = 'dbus-send --type=method_call --print-reply --dest=' \
                    + self.dbus + \
                    ' /KMediaFactory org.kde.kmediafactory.KMediaFactory.' + \
                    func + ' '
            for arg in args:
                if type(arg) in (str, unicode):
                    arg = arg.replace('"', '\\"')
                    cmd += 'string:"%s" ' % arg
                elif type(arg) == int:
                    cmd += 'int32:"%d" ' % arg
                elif type(arg) == bool:
                    cmd += 'boolean:"%d" ' % arg
            result = run(cmd)
            if result[0] == 0:
                array = False
                r = []
                for line in result[1].splitlines()[1:]:
                    line = line.strip()
                    if line.startswith('array'):
                        array = True
                        continue
                    try:
                        a = re.findall(r'(\w+) (.*)', line)
                        #print line, a
                        for item in a:
                            if item[0] == 'string':
                                r.append(unicode(item[1].strip('"')))
                            elif item[0] == 'int32':
                                r.append(int(item[1]))
                            elif item[0] == 'boolean':
                                r.append(bool(item[1] == 'true'))
                    except:
                        pass
                if array:
                    funcResult = r
                elif len(r) == 1:
                    funcResult = r[0]
        print func, args, funcResult
        return funcResult

def to_float(s):
    a = re.findall(ur'(\d+):(\d+):(\d+)\.(\d+)' ,s)
    f = 0.0
    if(len(a) > 0):
        f = int(a[0][0]) * 3600 + int(a[0][1]) * 60 + int(a[0][2]) + \
            float(a[0][3]) / 1000
    return f

def to_kmf_time(f):
    return '%d:%2.2d:%2.2d.%3.3d' % (int(f / 3600), int(f / 60) % 60, \
            int(f) % 60, int(f * 1000) % 1000)

def to_srt_time(f):
    return '%2.2d:%2.2d:%2.2d,%3.3d' % (int(f / 3600), int(f / 60) % 60, \
            int(f) % 60, int(f * 1000) % 1000)

kmf = KMediaFactory()

xml_files = kmf.getOpenFileNames('KMF_DVDAUTHOR_XML', '*.xml|XML files',
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
            f = to_float(cell)
            if f > 0.0:
                f += CHAPTER_FORWARD
            chapters[file].append([f, 0.0])

    # Check if there dv files from kino editing
    # and get dates from file names
    d = os.path.dirname(xml_files[0])
    dvs = glob.glob(os.path.join(d, '*.[Dd][Vv]'))
    dvs.sort()
    for (i, dv) in enumerate(dvs):
        a = re.findall(ur'(\d+.\d+.\d+_\d+-\d+-\d+)', dv);
        if len(a) > 0 and len(chapters[file]) > i:
            # For last file only...
            chapters[file][i][1] = time.mktime(time.strptime(a[0], \
                    '%Y.%m.%d_%H-%M-%S'))  + OFFSET

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
                srt += time.strftime(FORMAT, \
                        time.localtime(chp[1])) + '\n\n'
                n += 1
        f = open(xml_files[0] + '.srt', 'w')
        f.write(srt)
        f.close()
        sub_xml += '<subtitle language="%s" file="%s" align="64" > \
               <font size="28" weight="500" name="Arial" /> \
               </subtitle>' % (SUB_LANGUAGE, xml_files[0] + '.srt')

    # Make xml for KMF
    xml = '<video>'
    for file in chapters.keys():
        xml += '<file path="%s" />' % file
        for (i, chp) in enumerate(chapters[file]):
            if chp[1] != 0.0:
                xml += '<cell chapter="1" start="%s" name="%s" />' \
                        % (to_kmf_time(chp[0]), time.strftime(FORMAT, \
                                    time.localtime(chp[1])))
            else:
                xml += '<cell chapter="1" start="%s" />' % to_kmf_time(chp[0])
    xml += '<audio language="%s" />' % AUDIO_LANGUAGE

    xml += sub_xml + '</video>'
    kmf.debug(xml)
    kmf.addMediaObject(xml)


