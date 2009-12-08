#!/usr/bin/python
# -*- coding: utf-8 -*-
#**************************************************************************
#   Tool for editing DVB (or any mpg files) for KMediaFactory
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

import os
from subprocess import *
import time
import kmediafactory

APP = 'KMediaFactory'
PATHS = ['/usr/share/', '/usr/local/share/', '/opt/projectx/']
SUB_PATHS = ['java', 'projectx', '']

class DVBPlugin:
    def __init__(self):
        self.kmf = kmediafactory.interface()
        action = self.kmf.newAction('dvb_edit',         # Name
                                    'video-television', # Icon
                                    'Add DV&B',         # Text
                                    'Ctrl+B',           # Shortcut
                                    self,               # Object
                                    'edit'              # Method
        )
        self.kmf.addMediaAction(action)

    def initPlugin(self, projectType):
        self.kmf = kmediafactory.interface()
        self.kmf.setActionEnabled('dvb_edit', projectType[:3] == 'DVD')

    def run(self, cmd):
        self.kmf.debug(cmd)
        process = Popen([cmd], shell = True, stdin = PIPE, stdout = PIPE, bufsize = 512 * 1024,
                        close_fds = True, stderr = STDOUT)
        process.wait()
        output = process.stdout.read().strip()
        self.kmf.debug(output)
        return (process.returncode, output)

    def projectx(self):
        result = self.run('which projectx')
        if result[0] == 0:
            return result[1]
        for path in PATHS:
            for sub in SUB_PATHS:
                bin = os.path.join(path, sub, 'ProjectX.jar')
                if os.path.exists(bin):
                    return 'java -jar ' + bin
        result = self.run('locate ProjectX.jar')
        if result[0] == 0:
            return 'java -jar ' + result[1]
        return 'java -jar ProjectX.jar'

    def edit(self):
        ts_files = self.kmf.getOpenFileNames('KMF_DVB',
                                             '*.ts *.ps *.mpg *.mpeg *.m2p|Mpeg files',
                                             'Add DVB file')

        if len(ts_files) != 0:
            files = ''
            for f in ts_files:
                files += '"' + f + '" '
            projectDir = self.kmf.projectDir('media')
            result = self.run(self.projectx() + ' -gui ' + files + ' -out "' + projectDir + '"')
            #result = run(projectx() + " -gui '" + ' '.join(ts_files) + "' -out" + projectDir)
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
                    self.kmf.messageBox(APP, 'Making %s failed.' % output, kmediafactory.Error)
                    return
                dlg = self.kmf.progressDialog(APP, 'Making %s...' % output, (size / 1024))

                cmd = 'mplex -f 8 -o "' + output + '" "' + input_video + '" "' + \
                      input_audio + '"'
                self.kmf.debug(cmd)
                process = Popen([cmd], shell = True, close_fds = True)
                while True:
                    n = 0
                    time.sleep(0.25)
                    try:
                        size = os.path.getsize(output) / 1024
                    except:
                        size = 0
                    dlg.setValue(size)
                    if dlg.wasCancelled():
                        self.kmf.debug('Cancelled')
                        process.kill()
                        break
                    n = process.poll()
                    if n != None:
                        self.kmf.debug('Stopped')
                        break
                dlg.close()
                if n == 0:
                    xml = '<media plugin="KMFImportVideo" object="video">' + \
                          '<video><file path="' + output + '"></video></media>'
                    self.kmf.addMediaObjectFromXML(xml)
                else:
                    self.kmf.messageBox(APP, 'Making %s failed.' % output,
                                             kmediafactory.Error)
            else:
                self.kmf.messageBox(APP, 'Running ProjectX failed.',
                                         kmediafactory.Error)

    def supportedProjectTypes(self):
        return ['DVD-PAL', 'DVD-NTSC']

kmediafactory.registerPlugin(DVBPlugin())
