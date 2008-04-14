#! /usr/bin/python
# -*- coding: utf-8 -*-
#
# Tool for KMediaFactory to edit dvb recordings using ProjectX
# Copyright (C) 2006-2007 by Petri Damstén <damu@iki.fi>
# License: GPL-2
#
# Uses mplex, projectx, dbus (dbus is used so python-dbus is not needed)
#

import os, sys
from popen2 import Popen4
import time
import re

APP = 'KMediaFactory'
INFO=5
SORRY=7
ERROR=8

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
              '<video><file path="' + xml + '"></video></media>'
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

def projectx():
    result = run('which projectx')
    if result[0] == 0:
        return result[1]
    result = run('locate ProjectX.jar')
    if result[0] == 0:
        return 'java -jar ' + result[1]
    if os.path.exists('/usr/share/projectx/ProjectX.jar'):
        return 'java -jar /usr/share/projectx/ProjectX.jar'
    return 'java -jar ProjectX.jar'

print 'Add DVB launched.'
kmf = KMediaFactory()

ts_files = kmf.getOpenFileNames('KMF_DVB', '*.ts *.ps *.mpg *.mpeg|Mpeg files',
                                'Add DVB file')

if len(ts_files) != 0:
    projectDir = kmf.projectDir('media', os.path.dirname(ts_files[0]))
    result = run(projectx() + ' -gui ' + ' '.join(ts_files) + ' -out ' + projectDir)
    if result[0] == 0:
        base = os.path.splitext(os.path.basename(ts_files[0]))[0]
        input_audio_ac3 = projectDir + base + '.ac3'
        input_audio_mp2 = projectDir + base + '.mp2'
        input_video = projectDir + base + '.m2v'
        output = projectDir + base + '.mpg'

        if os.path.exists(input_audio_ac3):
            input_audio = input_audio_ac3
        else:
            input_audio = input_audio_mp2
        try:
            size = os.path.getsize(input_audio) + os.path.getsize(input_video)
        except:
            kmf.message(APP, 'Making %s failed.' % output, ERROR)
            sys.exit(0)
        kmf.progressDialog(APP, 'Making %s...' % output,
                           (size / 1024))

        cmd = 'mplex -f 8 -o "' + output + '" "' + input_video + '" "' + \
                input_audio + '"'
        pipe = Popen4(cmd)
        while True:
            time.sleep(0.25)
            try:
                size = os.path.getsize(output) / 1024
            except:
                size = 0
            kmf.pdlgSetValue(size)
            if kmf.pdlgWasCancelled():
                os.kill(pipe.pid)
                break
            n = pipe.poll()
            if n != -1:
                break
        kmf.pdlgClose()
        if n == 0:
            kmf.addMediaObject(output)
        else:
            kmf.message(APP, 'Making %s failed.' % output, ERROR)
    else:
        kmf.message(APP, 'Running ProjectX failed.', ERROR)
